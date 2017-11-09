#ifndef PATH_TRACKING_UTILS_HPP
#define PATH_TRACKING_UTILS_HPP

#include <istream>

#include <mongocxx/collection.hpp>
#include <mongocxx/pool.hpp>

#include <Poco/Net/HTTPServerResponse.h>

std::string readAll(std::istream& stream);

mongocxx::collection usersCollection(mongocxx::pool::entry& client);

void sendBadRequest(Poco::Net::HTTPServerResponse& response);
void sendUnauthorized(Poco::Net::HTTPServerResponse& response);
void sendNotFound(Poco::Net::HTTPServerResponse& response);

std::string generateToken();
std::string sha256(const std::string& str);

#endif //PATH_TRACKING_UTILS_HPP
