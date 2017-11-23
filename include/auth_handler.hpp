#ifndef PATH_TRACKING_AUTH_HANDLER_HPP
#define PATH_TRACKING_AUTH_HANDLER_HPP

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <mongocxx/pool.hpp>

#include <json.hpp>

struct User {
    std::string id;
    std::string email;
    std::vector<std::string> friends;
};

void to_json(nlohmann::json& j, const User& p);

class AuthHandler : public Poco::Net::HTTPRequestHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override final;

protected:
    mongocxx::pool& pool;

    virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) = 0;

public:
    AuthHandler(mongocxx::pool& pool);
};


#endif //PATH_TRACKING_AUTH_HANDLER_HPP
