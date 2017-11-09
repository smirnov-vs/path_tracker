#ifndef PATH_TRACKING_SIGNUP_HANLDER_H
#define PATH_TRACKING_SIGNUP_HANLDER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <mongocxx/pool.hpp>

class SignupHandler final : public Poco::Net::HTTPRequestHandler {

    mongocxx::pool& pool;

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;

public:
    SignupHandler(mongocxx::pool& pool);
};


#endif //PATH_TRACKING_SIGNUP_HANLDER_H
