#include "factory.hpp"

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

Poco::Net::HTTPRequestHandler* Factory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    for (const auto& [regex, endpoint] : routes) {
        if (endpoint.method == request.getMethod() && std::regex_search(request.getURI(), regex)) {
            return endpoint.handler();
        }
    }

    return nullptr;
}

void Factory::route(const std::string& location, const handlerFactory& handlerFactory, const std::string& method) {
    routes.emplace_back(location, Endpoint{handlerFactory, method});
}
