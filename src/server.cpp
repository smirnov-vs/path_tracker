#include "server.hpp"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocketImpl.h>

#include "json.hpp"

using namespace clickhouse;
using namespace std::chrono_literals;
using Json = nlohmann::json;

struct ServerSocketImpl final : public Poco::Net::ServerSocketImpl
{
    using Poco::Net::SocketImpl::init;
};

class Socket final : public Poco::Net::Socket
{
public:
    Socket(const std::string& address)
            : Poco::Net::Socket(new ServerSocketImpl())
    {
        const Poco::Net::SocketAddress socket_address(address);
        auto socket = (ServerSocketImpl*)impl();
        socket->init(socket_address.af());
        socket->setReuseAddress(true);
        socket->setReusePort(false);
        socket->bind(socket_address, false);
        socket->listen();
    }
};

int Server::main(const std::vector<std::string>& args) {
    Client client(ClientOptions().SetHost("localhost"));
    client.Execute("USE tracking");

    isRunning = true;
    std::thread worker(&Server::logWorker, this, std::ref(client));

    Poco::Net::HTTPServerParams::Ptr parameters = new Poco::Net::HTTPServerParams();
    parameters->setTimeout(10000);
    parameters->setMaxQueued(5000);
    parameters->setMaxThreads(8);

    const Poco::Net::ServerSocket socket(Socket("0.0.0.0:8000"));
    Poco::Net::HTTPServer server(new Factory(logs, logsMutex), socket, parameters);

    server.start();
    waitForTerminationRequest();
    server.stop();

    isRunning = false;
    workerCv.notify_one();
    worker.join();

    return 0;
}

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
