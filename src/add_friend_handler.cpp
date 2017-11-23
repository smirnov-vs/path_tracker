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

    auto client = pool.acquire();
    auto users = usersCollection(client);

    {
        auto doc_filter = document()
                << "email" << email
                << finalize;
        auto doc_value = document()
                << "$addToSet" << bsoncxx::builder::stream::open_document
                << "in_friends" << user.email
                << bsoncxx::builder::stream::close_document
                << finalize;

        auto result = users.update_one(doc_filter.view(), doc_value.view());
        if (!result || result->modified_count() == 0) {
            sendBadRequest(response);
            return;
        }
    }

    {
        auto doc_filter = document()
                << "_id" << bsoncxx::oid(user.id)
                << finalize;

        auto doc_value = document()
                << "$addToSet" << bsoncxx::builder::stream::open_document
                << "out_friends" << email
                << bsoncxx::builder::stream::close_document
                << finalize;


        auto result = users.update_one(doc_filter.view(), doc_value.view());
        if (!result) {
            sendBadRequest(response);
            return;
        }
    }

    response.send();
}

AddFriendHandler::AddFriendHandler(mongocxx::pool &pool)
        : AuthHandler(pool) {}
