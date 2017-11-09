#include "log_handler.hpp"
#include "json.hpp"
#include "utils.hpp"

Log::Log(const std::string& json, time_t time)
        : json(json), time(time) {}

void LogHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    auto json = readAll(request.stream());
    const auto now = time(nullptr);
    {
        std::lock_guard lock(logsMutex);
        logs.emplace_back(std::move(json), now);
    }

    response.send();
}

LogHandler::LogHandler(Logs& logs, std::mutex& logsMutex)
        : logs(logs), logsMutex(logsMutex) {}
