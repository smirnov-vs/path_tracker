#include "signin_handler.hpp"
#include "utils.hpp"
#include "json.hpp"

#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>

using namespace bsoncxx::builder::stream;
using Json = nlohmann::json;

void SigninHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
    Json json;
    try {
        json = Json::parse(readAll(request.stream()));
        if (json.find("email") == json.end() || json.find("password") == json.end()) {
            sendBadRequest(response);
            return;
        }
    } catch (std::exception &) {
        sendUnauthorized(response);
        return;
    }

    auto client = pool.acquire();
    auto users = usersCollection(client);
    auto result = users.find_one(document() << "email" << (std::string)json["email"] << "password" << sha256(json["password"]) << finalize);
    if (result) {
        auto view = result->view();

        Poco::Net::HTTPCookie cookie("token", view["token"].get_utf8().value.to_string());
        cookie.setPath("/");
        cookie.setHttpOnly(true);
        //cookie.setSecure(true);
        cookie.setMaxAge(60 * 60 * 24 * 30);

        response.addCookie(cookie);
        response.send();
    } else {
        sendUnauthorized(response);
    }
}

SigninHandler::SigninHandler(mongocxx::pool &pool)
        : pool(pool) {}
