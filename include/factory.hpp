#ifndef PATH_TRACKING_FACTORY_H
#define PATH_TRACKING_FACTORY_H

#include "log_handler.hpp"

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include <regex>

class Factory final : public Poco::Net::HTTPRequestHandlerFactory {
    typedef std::function<Poco::Net::HTTPRequestHandler*()> handlerFactory;

    std::vector<std::pair<std::regex, handlerFactory>> routes;
    logs_t& logs;
    std::mutex& logsMutex;

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;

public:
    Factory(logs_t& client, std::mutex& logsMutex);
};

#endif //PATH_TRACKING_FACTORY_H
