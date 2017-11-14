#ifndef PATH_TRACKING_FACTORY_H
#define PATH_TRACKING_FACTORY_H

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include <regex>

class Factory final : public Poco::Net::HTTPRequestHandlerFactory {
    typedef std::function<Poco::Net::HTTPRequestHandler*()> handlerFactory;

    struct Endpoint {
        handlerFactory handler;
        std::string method;
    };

    std::vector<std::pair<std::regex, Endpoint>> routes;

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;

public:
    typedef Poco::SharedPtr<Factory> Ptr;

    template <class T>
    static auto wrap(auto&&... args) {
        return [=] { return new T(args...); };
    };

    void route(const std::string& location, const handlerFactory& handlerFactory, const std::string& method);
};

#endif //PATH_TRACKING_FACTORY_H
