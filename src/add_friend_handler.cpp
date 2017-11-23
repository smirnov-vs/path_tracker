#include "add_friend_handler.hpp"
#include "utils.hpp"

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::stream;
using Json = nlohmann::json;

void AddFriendHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const User &user) {
    std::string email;
    try {
        Json json = Json::parse(readAll(request.stream()));
        if (json.find("email") == json.end() || (email = (std::string)json["email"]).empty()) {
            sendBadRequest(response);
            return;
        }
    } catch (std::exception &) {
        sendBadRequest(response);
        return;
    }

    auto doc_filter = document()
            << "_id" << bsoncxx::oid(user.id)
            << finalize;

    auto doc_value = document()
            << "$addToSet" << bsoncxx::builder::stream::open_document
            << "friends" << email
            << bsoncxx::builder::stream::close_document
            << finalize;

    auto client = pool.acquire();

    usersCollection(client).update_one(doc_filter.view(), doc_value.view());

    response.send();
}

AddFriendHandler::AddFriendHandler(mongocxx::pool &pool)
        : AuthHandler(pool) {}
