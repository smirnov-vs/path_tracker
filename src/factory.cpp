#include "factory.hpp"

Poco::Net::HTTPRequestHandler* Factory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    if (request.getMethod() != Poco::Net::HTTPRequest::HTTP_POST)
        return nullptr;

    if (request.getURI() == "/log")
        return new LogHandler(logs, logsMutex);

    return nullptr;
}

Factory::Factory(logs_t& logs, std::mutex& logsMutex)
        : logs(logs), logsMutex(logsMutex) {}
