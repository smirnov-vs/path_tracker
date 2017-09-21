#include "factory.hpp"

template <class T, class ...Args>
inline auto getFactory(Args&&... args ) {
    return [=] { return new T(std::move(args)...); };
};

Poco::Net::HTTPRequestHandler* Factory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    if (request.getMethod() != Poco::Net::HTTPRequest::HTTP_POST)
        return nullptr;

    for (auto&& [regex, handler] : routes) {
        if (std::regex_match(request.getURI(), regex))
            return handler();
    }

    return nullptr;
}

Factory::Factory(logs_t& logs, std::mutex& logsMutex)
        : logs(logs), logsMutex(logsMutex) {
    routes.emplace_back("^/log/?$", getFactory<LogHandler>(std::ref(logs), std::ref(logsMutex)));
}
