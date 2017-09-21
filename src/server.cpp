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
            while (!logs.empty()) {
                localLogs.push(logs.front());
                logs.pop();
            }
        }

        if (localLogs.empty())
            continue;

        Block block;
        auto id = std::make_shared<ColumnUInt64>();
        auto time = std::make_shared<ColumnDateTime>();
        auto latitude = std::make_shared<ColumnFloat32>();
        auto longitude = std::make_shared<ColumnFloat32>();
        auto accuracy = std::make_shared<ColumnFloat32>();
        auto speed = std::make_shared<ColumnFloat32>();

        while (!localLogs.empty()) {
            Log log = std::move(localLogs.front());
            Json json = Json::parse(log.json);
            localLogs.pop();

            id->Append((uint64_t)json["id"]);
            time->Append(log.time);
            latitude->Append((float)json["latitude"]);
            longitude->Append((float)json["longitude"]);
            accuracy->Append((float)json["accuracy"]);
            speed->Append((float)json["speed"]);
        }

        block.AppendColumn("id", id);
        block.AppendColumn("time", time);
        block.AppendColumn("latitude", latitude);
        block.AppendColumn("longitude", longitude);
        block.AppendColumn("accuracy", accuracy);
        block.AppendColumn("speed", speed);

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
    socket.setReuseAddress(true);

    Poco::Net::HTTPServer server(new Factory(logs, logsMutex), socket, parameters);

    server.start();
    waitForTerminationRequest();
    server.stop();

    isRunning = false;
    workerCv.notify_one();
    worker.join();

    return 0;
}

POCO_SERVER_MAIN(Server)
