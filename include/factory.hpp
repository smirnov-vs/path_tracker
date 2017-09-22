#ifndef PATH_TRACKING_FACTORY_H
#define PATH_TRACKING_FACTORY_H

#include "log_handler.hpp"
#include "track_handler.hpp"

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include <regex>

class Factory final : public Poco::Net::HTTPRequestHandlerFactory {
    typedef std::function<Poco::Net::HTTPRequestHandler*()> handlerFactory;

    std::vector<std::pair<std::regex, handlerFactory>> routes;

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;

public:
    typedef Poco::SharedPtr<Factory> Ptr;

    template <class T>
    static auto wrap(auto&&... args) {
        return [=] { return new T(args...); };
    };

    void route(const std::string& url, const handlerFactory& handlerFactory);
};

#endif //PATH_TRACKING_FACTORY_H
