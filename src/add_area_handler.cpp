#include "add_area_handler.hpp"
#include "utils.hpp"

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::stream;
using Json = nlohmann::json;

void AddAreaHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const User &user) {
    Json json;
    try {
        json = Json::parse(readAll(request.stream()));
        if (json.find("name") == json.end() || json.find("coordinates") == json.end()) {
            sendBadRequest(response);
            return;
        }
    } catch (std::exception&) {
        sendBadRequest(response);
        return;
    }

    std::string name = json["name"];
    std::vector<Coordinate> coordinates = json["coordinates"];

    auto client = pool.acquire();
    auto users = usersCollection(client);

    auto doc_filter = document()
            << "_id" << bsoncxx::oid(user.id)
            << finalize;

    bsoncxx::builder::basic::array array_builder;
    for (const auto& c : coordinates)
        array_builder.append(document() << "latitude" << c.latitude << "longitude" << c.longitude << finalize);

    auto doc_value = document()
            << "$push" << open_document
            << "areas" << open_document
            << "_id" << bsoncxx::oid()
            << "name" << name
            << "coordinates" << array_builder
            << close_document
            << close_document
            << finalize;

    auto result = users.update_one(doc_filter.view(), doc_value.view());
    if (!result || result->modified_count() == 0) {
        sendBadRequest(response);
        return;
    }

    response.send();
}

AddAreaHandler::AddAreaHandler(mongocxx::v_noabi::pool &pool)
        : AuthHandler(pool) {}
