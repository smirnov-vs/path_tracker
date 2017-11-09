#include "track_handler.hpp"
#include "json.hpp"
#include "format.hpp"
#include "utils.hpp"

#include <Poco/URI.h>

using namespace clickhouse;
using Json = nlohmann::json;

static constexpr size_t SECONDS_IN_DAY = 24*60*60;
thread_local Client TrackHandler::client(ClientOptions().SetHost("localhost"));

inline time_t today() {
    time_t now = time(nullptr);
    return now - (now % SECONDS_IN_DAY);
}

inline time_t tomorrow() {
    return today() + SECONDS_IN_DAY;
}

inline bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void TrackHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    Poco::URI uri(request.getURI());
    const auto params = uri.getQueryParameters();
    const auto it = std::find_if(params.begin(), params.end(), [](const auto &pair) { return pair.first == "id"; });
    if (it == params.end() || !is_number(it->second)) {
        sendBadRequest(response);
        return;
    }

    const auto query = format("SELECT toString(time), latitude, longitude, accuracy, speed FROM tracking.logs "
                                      "WHERE id = {} AND time BETWEEN toDateTime({}) AND toDateTime({})",
                              it->second, today(), tomorrow());
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
                          array += json;
                      }
                  }
    );

    response.setContentType("application/json");
    response.send() << array;
}
