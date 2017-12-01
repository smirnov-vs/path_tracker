#include "user.hpp"

Coordinate::Coordinate(double latitude, double longitude)
        : latitude(latitude), longitude(longitude) {}

Area::Area(const std::string &id, const std::string &name, const std::vector<Coordinate> &coordinates)
        : id(id), name(name), coordinates(coordinates) {}

void to_json(nlohmann::json& j, const Coordinate& coordinate) {
    j = {
            {"latitude",  coordinate.latitude},
            {"longitude", coordinate.longitude},
    };
}

void to_json(nlohmann::json& j, const Area& area) {
    j = {
            {"id",          area.id},
            {"name",        area.name},
            {"coordinates", area.coordinates},
    };
}

void to_json(nlohmann::json& j, const User& user) {
    j = {
            {"id",          user.id},
            {"email",       user.email},
            {"in_friends",  user.in_friends},
            {"out_friends", user.out_friends},
            {"areas",       user.areas},
    };
}

void from_json(const nlohmann::json& j, Coordinate& coordinate) {
    coordinate.latitude = j["latitude"];
    coordinate.longitude = j["longitude"];
}
