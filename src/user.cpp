#include "user.hpp"

Area::Area(const std::string &id, double latitude, double longitude, double radius)
        : id(id), latitude(latitude), longitude(longitude), radius(radius) {}

void to_json(nlohmann::json& j, const Area& area) {
    j = {
            {"id",        area.id},
            {"latitude",  area.latitude},
            {"longitude", area.longitude},
            {"radius",    area.radius},
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
