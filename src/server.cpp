#include "server.hpp"
#include "json.hpp"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocketImpl.h>

#include <thread>

using namespace clickhouse;
using namespace std::chrono_literals;
using Json = nlohmann::json;

void Server::logWorker(Client& client) {
    std::mutex timerMutex;
    std::unique_lock<std::mutex> timerLock(timerMutex);

    while (isRunning) {
        workerCv.wait_for(timerLock, 5s);

        logs_t localLogs;
        {
            std::lock_guard<std::mutex> lock(logsMutex);
            std::swap(logs, localLogs);
        }

        if (localLogs.empty())
            continue;

        Block block;
        auto idColumn = std::make_shared<ColumnUInt64>();
        auto timeColumn = std::make_shared<ColumnDateTime>();
        auto latitudeColumn = std::make_shared<ColumnFloat32>();
        auto longitudeColumn = std::make_shared<ColumnFloat32>();
        auto accuracyColumn = std::make_shared<ColumnFloat32>();
        auto speedColumn = std::make_shared<ColumnFloat32>();

        while (!localLogs.empty()) {
            try {
                Log log = std::move(localLogs.front());
                localLogs.pop();

                Json json = Json::parse(log.json);
                uint64_t id = json["id"];
                float latitude = json["latitude"];
                float longitude = json["longitude"];
                float accuracy = json["accuracy"];
                float speed = json["speed"];

                idColumn->Append(id);
                timeColumn->Append(log.time);
                latitudeColumn->Append(latitude);
                longitudeColumn->Append(longitude);
                accuracyColumn->Append(accuracy);
                speedColumn->Append(speed);
            } catch (std::exception&) {}
        }

        block.AppendColumn("id", idColumn);
        block.AppendColumn("time", timeColumn);
        block.AppendColumn("latitude", latitudeColumn);
        block.AppendColumn("longitude", longitudeColumn);
        block.AppendColumn("accuracy", accuracyColumn);
        block.AppendColumn("speed", speedColumn);

        if (block.GetRowCount() > 0)
            client.Insert("logs", block);
    }
}

int Server::main(const std::vector<std::string>& args) {
    Client client(ClientOptions().SetHost("localhost"));
    client.Execute("USE tracking");

    isRunning = true;
    std::thread worker(&Server::logWorker, this, std::ref(client));

    Poco::Net::HTTPServerParams::Ptr parameters = new Poco::Net::HTTPServerParams();
    parameters->setTimeout(Poco::Timespan(1, 0));
    parameters->setMaxQueued(1024);
    parameters->setMaxThreads(8);

    Poco::Net::ServerSocket socket(Poco::Net::SocketAddress("127.0.0.1", 8000));

    Factory::Ptr factory = new Factory(logs, logsMutex);
    factory->route("^/log/?$", Factory::getFactory<LogHandler>(std::ref(logs), std::ref(logsMutex)));

    Poco::Net::HTTPServer server(factory, socket, parameters);

    server.start();
    waitForTerminationRequest();
    server.stop();

    isRunning = false;
    workerCv.notify_one();
    worker.join();

    return 0;
}

POCO_SERVER_MAIN(Server)
