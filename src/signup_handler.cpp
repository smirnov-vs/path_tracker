#include "signup_handler.hpp"
#include "utils.hpp"
#include "json.hpp"

#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>

using namespace bsoncxx::builder::stream;
using Json = nlohmann::json;

void SignupHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
    Json json;
    try {
        json = Json::parse(readAll(request.stream()));
        if (json.find("email") == json.end() || json.find("password") == json.end()) {
            sendBadRequest(response);
            return;
        }
    } catch (std::exception &) {
        sendBadRequest(response);
        return;
    }

    auto token = generateToken();
    auto doc_value = document()
            << "email" << (std::string) json["email"]
            << "password" << sha256(json["password"])
            << "token" << token
            << finalize;

    auto client = pool.acquire();

    try {
        usersCollection(client).insert_one(doc_value.view());
    } catch (mongocxx::bulk_write_exception& e) {
        sendBadRequest(response);
        return;
    }

    Poco::Net::HTTPCookie cookie("token", token);
    cookie.setHttpOnly(true);
    //cookie.setSecure(true);
    cookie.setMaxAge(60 * 60 * 24 * 30);

    response.addCookie(cookie);
    response.send();
}

SignupHandler::SignupHandler(mongocxx::pool &pool)
        : pool(pool) {}
