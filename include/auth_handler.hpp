#ifndef PATH_TRACKING_AUTH_HANDLER_HPP
#define PATH_TRACKING_AUTH_HANDLER_HPP

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <mongocxx/pool.hpp>

#include "user.hpp"

class AuthHandler : public Poco::Net::HTTPRequestHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override final;

protected:
    mongocxx::pool& pool;

    virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) = 0;

public:
    AuthHandler(mongocxx::pool& pool);
};


#endif //PATH_TRACKING_AUTH_HANDLER_HPP
