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

    std::string email = json["email"];
    std::string password = json["password"];

    auto client = pool.acquire();
    auto users = usersCollection(client);
    auto result = users.find_one(document() << "email" << email << "password" << sha256(password) << finalize);
    if (result) {
        auto view = result->view();

        if (auto it = json.find("gcm_token"); it != json.end()) {
            std::string gcm_token = *it;

            auto doc_filter = document()
                    << "email" << email
                    << finalize;
            auto doc_value = document()
                    << "$set" << open_document
                    << "gcm_token" << gcm_token
                    << close_document
                    << finalize;

            auto result = users.update_one(doc_filter.view(), doc_value.view());
            if (!result || result->modified_count() == 0) {
                sendUnauthorized(response);
                return;
            }
        }

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
