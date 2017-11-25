#include "delete_area_handler.hpp"
#include "utils.hpp"

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::stream;
using Json = nlohmann::json;

void DeleteAreaHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const User &user) {
    Json json;
    try {
        json = Json::parse(readAll(request.stream()));
        if (json.find("id") == json.end()) {
            sendBadRequest(response);
            return;
        }
    } catch (std::exception&) {
        sendBadRequest(response);
        return;
    }

    auto client = pool.acquire();
    auto users = usersCollection(client);

    auto doc_filter = document()
            << "_id" << bsoncxx::oid(user.id)
            << finalize;
    auto doc_value = document()
            << "$pull" << bsoncxx::builder::stream::open_document
            << "areas" << bsoncxx::builder::stream::open_document
            << "_id" << bsoncxx::oid((std::string)json["id"])
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document
            << finalize;

    auto result = users.update_one(doc_filter.view(), doc_value.view());
    if (!result || result->modified_count() == 0) {
        sendBadRequest(response);
        return;
    }

    response.send();
}

DeleteAreaHandler::DeleteAreaHandler(mongocxx::pool &pool)
        : AuthHandler(pool) {}
