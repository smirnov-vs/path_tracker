#ifndef PATH_TRACKING_LOG_HANDLER_H
#define PATH_TRACKING_LOG_HANDLER_H

#include "auth_handler.hpp"

#include <queue>
#include <mutex>

struct Log {
    std::string json;
    std::string userId;
    std::time_t time;

    Log(const std::string& json, const std::string& userId, time_t time);
};

typedef std::vector<Log> Logs;

class LogHandler final : public AuthHandler {

    Logs& logs;
    std::mutex& logsMutex;

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    LogHandler(mongocxx::pool& pool, Logs& logs, std::mutex& logsMutex);
};

#endif //PATH_TRACKING_LOG_HANDLER_H
