#include "factory.hpp"

Factory::Factory(logs_t& logs, std::mutex& logsMutex)
        : logs(logs), logsMutex(logsMutex) {}

Poco::Net::HTTPRequestHandler* Factory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    for (auto&& [regex, handler] : routes) {
        if (std::regex_match(request.getURI(), regex))
            return handler();
    }

    return nullptr;
}

void Factory::route(const std::string& url, const Factory::handlerFactory& handlerFactory) {
    routes.emplace_back(url, handlerFactory);
}
