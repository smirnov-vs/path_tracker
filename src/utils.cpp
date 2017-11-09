#include <utils.hpp>

#include <mongocxx/client.hpp>
#include <openssl/sha.h>

#include <random>
#include <sstream>
#include <iomanip>

std::string readAll(std::istream& stream) {
    return std::string(std::istream_iterator<char>(stream), std::istream_iterator<char>());
}

mongocxx::collection usersCollection(mongocxx::pool::entry& client) {
    return (*client)["tracking"]["users"];
}

void sendBadRequest(Poco::Net::HTTPServerResponse& response) {
    response.setStatus(Poco::Net::HTTPServerResponse::HTTP_BAD_REQUEST);
    response.setReason(Poco::Net::HTTPServerResponse::HTTP_REASON_BAD_REQUEST);
    response.send();
}

void sendUnauthorized(Poco::Net::HTTPServerResponse& response) {
    response.setStatus(Poco::Net::HTTPServerResponse::HTTP_UNAUTHORIZED);
    response.setReason(Poco::Net::HTTPServerResponse::HTTP_REASON_UNAUTHORIZED);
    response.send();
}

void sendNotFound(Poco::Net::HTTPServerResponse& response) {
    response.setStatus(Poco::Net::HTTPServerResponse::HTTP_NOT_FOUND);
    response.setReason(Poco::Net::HTTPServerResponse::HTTP_REASON_NOT_FOUND);
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

std::string sha256(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];

    return ss.str();
}
