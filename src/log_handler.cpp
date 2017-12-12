#include "log_handler.hpp"
#include "utils.hpp"

Log::Log(const std::string& json, const User& user, std::time_t time)
        : json(json), user(user), time(time) {}

void LogHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) {
    auto json = readAll(request.stream());
    const auto now = time(nullptr);
    {
        std::lock_guard lock(logsMutex);
        logs.emplace_back(std::move(json), user, now);
    }

    response.send();
}

LogHandler::LogHandler(mongocxx::pool& pool, Logs& logs, std::mutex& logsMutex)
        : AuthHandler(pool), logs(logs), logsMutex(logsMutex) {}
