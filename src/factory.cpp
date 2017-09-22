#include "factory.hpp"

Poco::Net::HTTPRequestHandler* Factory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    for (const auto& [regex, handler] : routes) {
        if (std::regex_search(request.getURI(), regex))
            return handler();
    }

    return nullptr;
}

void Factory::route(const std::string& url, const Factory::handlerFactory& handlerFactory) {
    routes.emplace_back(url, handlerFactory);
}
