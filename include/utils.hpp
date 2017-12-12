#ifndef PATH_TRACKING_UTILS_HPP
#define PATH_TRACKING_UTILS_HPP

#include <istream>

#include <mongocxx/collection.hpp>
#include <mongocxx/pool.hpp>

#include <Poco/Net/HTTPServerResponse.h>

#include "user.hpp"
#include "curl.hpp"

std::string readAll(std::istream& stream);

mongocxx::collection usersCollection(mongocxx::pool::entry& client);

void sendBadRequest(Poco::Net::HTTPServerResponse& response);
void sendUnauthorized(Poco::Net::HTTPServerResponse& response);

std::string generateToken();
std::string sha256(const std::string& value);

bool isIntersects(const Area& area, float latitude, float longitude, float accuracy);

void sendPush(const Curl& curl, const std::string& userName, const std::string& areaName, const std::string& token);

#endif //PATH_TRACKING_UTILS_HPP
