#include "me_handler.hpp"

using Json = nlohmann::json;

void MeHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const User& user) {
    Json json = user;
    response.send() << json;
}

MeHandler::MeHandler(mongocxx::pool &pool)
        : AuthHandler(pool) {}
