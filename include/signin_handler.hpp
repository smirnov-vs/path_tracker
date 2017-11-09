#ifndef PATH_TRACKING_LOGIN_HANDLER_H
#define PATH_TRACKING_LOGIN_HANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <mongocxx/pool.hpp>

class SigninHandler final : public Poco::Net::HTTPRequestHandler {

    mongocxx::pool& pool;

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;

public:
    SigninHandler(mongocxx::pool& pool);
};


#endif //PATH_TRACKING_LOGIN_HANDLER_H
