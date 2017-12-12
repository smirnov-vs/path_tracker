#include "utils.hpp"
#include "format.hpp"

#include <mongocxx/client.hpp>
#include <openssl/sha.h>

#include <random>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <curl/curl.h>

std::string readAll(std::istream& stream) {
    return std::string(std::istream_iterator<char>(stream), std::istream_iterator<char>());
}

mongocxx::collection usersCollection(mongocxx::pool::entry& client) {
    return (*client)["tracking"]["users"];
}

void sendBadRequest(Poco::Net::HTTPServerResponse& response) {
    response.setStatusAndReason(Poco::Net::HTTPServerResponse::HTTP_BAD_REQUEST);
    response.send();
}

void sendUnauthorized(Poco::Net::HTTPServerResponse& response) {
    response.setStatusAndReason(Poco::Net::HTTPServerResponse::HTTP_UNAUTHORIZED);
    response.send();
}

std::string generateToken() {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<uint64_t> uniform(0, UINT64_MAX);
    auto first = uniform(generator);
    auto second = uniform(generator);
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << first << second;
    return ss.str();
}

std::string sha256(const std::string& value) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, value.c_str(), value.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << (int)hash[i];

    return ss.str();
}

inline double metersToDegrees(double meters) {
    return meters / 111111;
}

bool isIntersects(const Area& area, float latitude, float longitude, float accuracy) {
    using Point = boost::geometry::model::d2::point_xy<double>;
    using Polygon = boost::geometry::model::polygon<Point>;

    Point circle_center(longitude, latitude);
    boost::geometry::model::multi_polygon<Polygon> circle;
    boost::geometry::strategy::buffer::point_circle point_strategy(360);
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(metersToDegrees(accuracy));
    boost::geometry::strategy::buffer::join_round join_strategy;
    boost::geometry::strategy::buffer::end_round end_strategy;
    boost::geometry::strategy::buffer::side_straight side_strategy;
    boost::geometry::buffer(circle_center, circle, distance_strategy, side_strategy,
                            join_strategy, end_strategy, point_strategy);

    Polygon area_polygon;
    std::vector<Point> points;
    for (const auto &coord : area.coordinates)
        points.emplace_back(coord.longitude, coord.latitude);
    boost::geometry::assign_points(area_polygon, points);

    return boost::geometry::intersects(circle, area_polygon);
}

void sendPush(const std::string& userName, const std::string& areaName, const std::string& token) {
    nlohmann::json pushMessage = {
            {
                "data", {
                    {"message", format("User <{}> just have left area <{}>", userName, areaName)},
                },
            },
            { "to", token },
    };

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Authorization: key=AIzaSyAfBYc0Y2_QravZwAEiS184TbL1UusAoME");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "https://gcm-http.googleapis.com/gcm/send");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pushMessage.dump().c_str());

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
}