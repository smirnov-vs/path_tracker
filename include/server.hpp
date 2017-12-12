#ifndef PATH_TRACKING_SERVER_H
#define PATH_TRACKING_SERVER_H

#include "factory.hpp"

#include "log_handler.hpp"
#include "track_handler.hpp"
#include "signup_handler.hpp"
#include "signin_handler.hpp"
#include "me_handler.hpp"
#include "logout_handler.hpp"
#include "add_friend_handler.hpp"
#include "delete_friend_handler.hpp"
#include "add_area_handler.hpp"
#include "delete_area_handler.hpp"
#include "curl.hpp"

#include <Poco/Util/ServerApplication.h>

#include <atomic>
#include <condition_variable>

class Server final : public Poco::Util::ServerApplication {

    static Curl curl;

    Logs logs;
    std::mutex logsMutex;
    std::atomic_bool isRunning;
    std::condition_variable workerCv;

    void logWorker(clickhouse::Client& client);

    int main(const std::vector<std::string>& args) override;

};


#endif //PATH_TRACKING_SERVER_H
