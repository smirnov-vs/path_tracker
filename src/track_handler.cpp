#include "track_handler.hpp"
#include "format.hpp"
#include "utils.hpp"

#include <Poco/URI.h>

using namespace clickhouse;
using Json = nlohmann::json;

static constexpr size_t SECONDS_IN_DAY = 24*60*60;
thread_local Client TrackHandler::client(ClientOptions().SetHost("localhost"));

inline std::time_t today(std::time_t timeOfDay) {
    return timeOfDay - (timeOfDay % SECONDS_IN_DAY);
}

inline std::time_t tomorrow(std::time_t timeOfDay) {
    return today(timeOfDay) + SECONDS_IN_DAY;
}

inline bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

inline auto findKey(const std::vector<auto>& vector, const std::string& value) {
    return std::find_if(vector.begin(), vector.end(), [&](const auto &pair) { return pair.first == value; });
}

void TrackHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) {
    Poco::URI uri(request.getURI());
    const auto params = uri.getQueryParameters();
    const auto it = findKey(params, "time");
    if (it == params.end() || !is_number(it->second)) {
        sendBadRequest(response);
        return;
    }
    std::time_t time = std::stol(it->second, nullptr, 10);

    const auto query = format("SELECT toString(time), latitude, longitude, accuracy, speed FROM tracking.logs "
                                      "WHERE id = '{}' AND time BETWEEN toDateTime({}) AND toDateTime({})",
                              user.id, today(time), tomorrow(time));
    auto array = Json::array();
    client.Select(query,
                  [&](const Block &block) {
                      for (size_t i = 0; i < block.GetRowCount(); ++i) {
                          Json json;
                          json["time"] = block[0]->As<ColumnString>()->At(i);
                          json["latitude"] = block[1]->As<ColumnFloat64>()->At(i);
                          json["longitude"] = block[2]->As<ColumnFloat64>()->At(i);
                          json["accuracy"] = block[3]->As<ColumnFloat64>()->At(i);
                          json["speed"] = block[4]->As<ColumnFloat64>()->At(i);
                          array += std::move(json);
                      }
                  }
    );

    response.setContentType("application/json");
    response.send() << array;
}

TrackHandler::TrackHandler(mongocxx::pool &pool)
        : AuthHandler(pool) {}
