#include "log_handler.hpp"
#include "json.hpp"

inline std::string readAll(std::istream& stream) {
    return std::string(std::istream_iterator<char>(stream), std::istream_iterator<char>());
}

Log::Log(const std::string& json, time_t time)
        : json(json), time(time) {}

void LogHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    {
        std::lock_guard<std::mutex> lock(logsMutex);
        logs.emplace(readAll(request.stream()), time(nullptr));
    }
    response.setStatus(Poco::Net::HTTPServerResponse::HTTP_OK);
    response.send().flush();
}

LogHandler::LogHandler(logs_t& logs, std::mutex& logsMutex)
        : logs(logs), logsMutex(logsMutex) {}
