#include "auth_handler.hpp"
#include "utils.hpp"

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::stream;

void to_json(nlohmann::json& j, const User& u) {
    j = {
            {"id",      u.id},
            {"email",   u.email},
            {"friends", u.friends},
    };
}

void AuthHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
    Poco::Net::NameValueCollection cookies;
    request.getCookies(cookies);

    auto it = cookies.find("token");
    if (it == cookies.end()) {
        sendUnauthorized(response);
        return;
    }

    auto client = pool.acquire();
    auto users = usersCollection(client);
    auto result = users.find_one(document() << "token" << it->second << finalize);
    if (result) {
        auto view = result->view();
        std::vector<std::string> friends;
        auto friendsView = view["friends"];
        if (friendsView) {
            auto dbFriends = friendsView.get_array().value;
            for (const auto &i : dbFriends)
                friends.emplace_back(i.get_utf8().value.to_string());
        }

        User user{view["_id"].get_oid().value.to_string(),
                  view["email"].get_utf8().value.to_string(),
                  friends
        };
        handleRequest(request, response, user);
    } else {
        sendUnauthorized(response);
    }
}

AuthHandler::AuthHandler(mongocxx::pool &pool)
        : pool(pool) {}

