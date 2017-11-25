#include "me_handler.hpp"

using Json = nlohmann::json;

void MeHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const User& user) {
    response.setContentType("application/json");
    response.send() << (Json)user;
}

MeHandler::MeHandler(mongocxx::pool &pool)
        : AuthHandler(pool) {}
