#include "logout_handler.hpp"
#include "utils.hpp"

using namespace bsoncxx::builder::stream;

void LogoutHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const User& user) {
    Poco::Net::HTTPCookie cookie("token");
    cookie.setMaxAge(0);
    response.addCookie(cookie);
    response.send();
}

LogoutHandler::LogoutHandler(mongocxx::pool &pool)
        : AuthHandler(pool) {}
