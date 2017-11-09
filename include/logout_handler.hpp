#ifndef PATH_TRACKING_LOGOUT_HANDLER_HPP
#define PATH_TRACKING_LOGOUT_HANDLER_HPP

#include "auth_handler.hpp"

#include <mongocxx/pool.hpp>

class LogoutHandler final : public AuthHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    LogoutHandler(mongocxx::pool& pool);
};


#endif //PATH_TRACKING_LOGOUT_HANDLER_HPP
