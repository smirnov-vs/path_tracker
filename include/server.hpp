#ifndef PATH_TRACKING_SERVER_H
#define PATH_TRACKING_SERVER_H

#include "factory.hpp"

#include <Poco/Util/ServerApplication.h>

#include <atomic>
#include <condition_variable>

class Server final : public Poco::Util::ServerApplication {

    logs_t logs;
    std::mutex logsMutex;
    std::atomic_bool isRunning;
    std::condition_variable workerCv;

    void logWorker(clickhouse::Client& client);

    int main(const std::vector<std::string>& args) override;

};


#endif //PATH_TRACKING_SERVER_H
