#ifndef PATH_TRACKING_LOG_HANDLER_H
#define PATH_TRACKING_LOG_HANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <queue>
#include <mutex>

struct Log {
    std::string json;
    time_t time;

    Log(const std::string& json, time_t time);
};

typedef std::queue<Log> logs_t;

class LogHandler final : public Poco::Net::HTTPRequestHandler {

    logs_t& logs;
    std::mutex& logsMutex;

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;

public:
    LogHandler(logs_t& logs, std::mutex& logsMutex);
};

#endif //PATH_TRACKING_LOG_HANDLER_H
