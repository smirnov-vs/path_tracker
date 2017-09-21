#include "log_handler.hpp"
#include "json.hpp"

inline std::string readAll(std::istream& stream) {
    return std::string(std::istream_iterator<char>(stream), std::istream_iterator<char>());
}

Log::Log(const std::string& json, time_t time)
        : json(json), time(time) {}

void LogHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    if (request.getMethod() != Poco::Net::HTTPRequest::HTTP_POST) {
        response.setStatus(Poco::Net::HTTPServerResponse::HTTP_NOT_IMPLEMENTED);
        response.setReason("Not Implemented");
    } else {
        std::lock_guard<std::mutex> lock(logsMutex);
        logs.emplace(readAll(request.stream()), time(nullptr));
    }

    response.send();
}

LogHandler::LogHandler(logs_t& logs, std::mutex& logsMutex)
        : logs(logs), logsMutex(logsMutex) {}
