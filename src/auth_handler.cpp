#include "auth_handler.hpp"
#include "utils.hpp"

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::stream;

auto convertFriends(const bsoncxx::document::element& friendsView) {
    std::vector<std::string> friends;
    if (friendsView) {
        auto dbFriends = friendsView.get_array().value;
        for (const auto& i : dbFriends)
            friends.emplace_back(i.get_utf8().value.to_string());
    }
    return friends;
}

auto convertAreas(const bsoncxx::document::element& areasView) {
    std::vector<Area> areas;
    if (areasView) {
        auto dbAreas = areasView.get_array().value;
        for (const auto& i : dbAreas) {
            auto document = i.get_document().value;
            areas.emplace_back(document["_id"].get_oid().value.to_string(),
                               document["latitude"].get_double().value,
                               document["longitude"].get_double().value,
                               document["radius"].get_double().value);
        }
    }
    return areas;
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
        User user{view["_id"].get_oid().value.to_string(),
                  view["email"].get_utf8().value.to_string(),
                  convertFriends(view["in_friends"]),
                  convertFriends(view["out_friends"]),
                  convertAreas(view["areas"])};
        handleRequest(request, response, user);
    } else {
        sendUnauthorized(response);
    }
}

AuthHandler::AuthHandler(mongocxx::pool &pool)
        : pool(pool) {}

