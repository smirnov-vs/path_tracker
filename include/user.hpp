#ifndef PATH_TRACKING_USER_HPP
#define PATH_TRACKING_USER_HPP

#include "json.hpp"

struct Coordinate {
    double latitude;
    double longitude;

    Coordinate() = default;
    Coordinate(double latitude, double longitude);
};

struct Area {
    std::string id;
    std::string name;
    std::vector<Coordinate> coordinates;

    Area(const std::string& id, const std::string& name, const std::vector<Coordinate>& coordinates);
};

struct User {
    std::string id;
    std::string email;
    std::vector<std::string> in_friends;
    std::vector<std::string> out_friends;
    std::vector<Area> areas;
};

void from_json(const nlohmann::json& j, Coordinate& coordinate);
void to_json(nlohmann::json& j, const User& user);

#endif //PATH_TRACKING_USER_HPP
