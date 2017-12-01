#include "server.hpp"
#include "utils.hpp"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocketImpl.h>

#include <thread>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace clickhouse;
using namespace std::chrono_literals;
using Json = nlohmann::json;
using namespace bsoncxx::builder::stream;

void Server::logWorker(Client& client) {
    std::mutex timerMutex;
    std::unique_lock timerLock(timerMutex);

    while (isRunning) {
        workerCv.wait_for(timerLock, 5s);

        Logs localLogs;
        {
            std::lock_guard lock(logsMutex);
            std::swap(logs, localLogs);
        }

        if (localLogs.empty())
            continue;

        Block block;
        auto idColumn = std::make_shared<ColumnString>();
        auto timeColumn = std::make_shared<ColumnDateTime>();
        auto latitudeColumn = std::make_shared<ColumnFloat64>();
        auto longitudeColumn = std::make_shared<ColumnFloat64>();
        auto accuracyColumn = std::make_shared<ColumnFloat64>();
        auto speedColumn = std::make_shared<ColumnFloat64>();

        for (const auto& log : localLogs) {
            try {
                const auto json = Json::parse(log.json);
                const float latitude = json["latitude"];
                const float longitude = json["longitude"];
                const float accuracy = json["accuracy"];
                const float speed = json["speed"];

                time_t pendingTime = 0;
                if (auto it = json.find("time"); it != json.end())
                    pendingTime = (time_t)*it;

                idColumn->Append(log.userId);
                timeColumn->Append(pendingTime == 0 ? log.time : pendingTime);
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
            client.Insert("tracking.logs", block);
    }
}

int Server::main(const std::vector<std::string>& args) {
    Client clickhouse(ClientOptions().SetHost("localhost"));

    mongocxx::instance instance;
    mongocxx::pool pool {mongocxx::uri()};
    {
        auto client = pool.acquire();
        auto users = usersCollection(client);
        users.create_index(document() << "email" << 1 << finalize, mongocxx::options::index().unique(true));
    }

    isRunning = true;
    std::thread worker(&Server::logWorker, this, std::ref(clickhouse));

    Factory::Ptr factory = new Factory();
    factory->route("^/api/log/?$", Factory::wrap<LogHandler>(std::ref(pool), std::ref(logs), std::ref(logsMutex)), Poco::Net::HTTPRequest::HTTP_POST);
    factory->route("^/api/track/?", Factory::wrap<TrackHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_GET);

    factory->route("^/api/signup/?$", Factory::wrap<SignupHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_POST);
    factory->route("^/api/session/?$", Factory::wrap<MeHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_GET);
    factory->route("^/api/session/?$", Factory::wrap<SigninHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_POST);
    factory->route("^/api/session/?$", Factory::wrap<LogoutHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_DELETE);

    factory->route("^/api/friends/?$", Factory::wrap<AddFriendHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_POST);
    factory->route("^/api/friends/?$", Factory::wrap<DeleteFriendHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_DELETE);

    factory->route("^/api/areas/?$", Factory::wrap<AddAreaHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_POST);
    factory->route("^/api/areas/?$", Factory::wrap<DeleteAreaHandler>(std::ref(pool)), Poco::Net::HTTPRequest::HTTP_DELETE);

    Poco::Net::ServerSocket socket(Poco::Net::SocketAddress("127.0.0.1", 8000));
    Poco::Net::HTTPServerParams::Ptr parameters = new Poco::Net::HTTPServerParams();
    parameters->setTimeout(Poco::Timespan(15, 0));
    parameters->setMaxQueued(1024);
    auto threads = std::thread::hardware_concurrency();
    parameters->setMaxThreads(threads != 0 ? threads : 2);

    Poco::Net::HTTPServer server(factory, socket, parameters);

    server.start();
    std::cout << "Server started" << std::endl;
    waitForTerminationRequest();
    server.stop();

    isRunning = false;
    workerCv.notify_one();
    worker.join();

    return 0;
}

POCO_SERVER_MAIN(Server)
