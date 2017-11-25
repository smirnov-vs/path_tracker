#ifndef PATH_TRACKING_USER_HPP
#define PATH_TRACKING_USER_HPP

#include "json.hpp"

struct Area {
    std::string id;
    double latitude;
    double longitude;
    double radius;

    Area(const std::string& id, double latitude, double longitude, double radius);
};

struct User {
    std::string id;
    std::string email;
    std::vector<std::string> in_friends;
    std::vector<std::string> out_friends;
    std::vector<Area> areas;
};

void to_json(nlohmann::json& j, const User& user);

#endif //PATH_TRACKING_USER_HPP
