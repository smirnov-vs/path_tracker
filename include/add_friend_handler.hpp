#ifndef PATH_TRACKING_ADD_FRIEND_HANDLER_HPP
#define PATH_TRACKING_ADD_FRIEND_HANDLER_HPP

#include "auth_handler.hpp"

class AddFriendHandler final : public AuthHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    AddFriendHandler(mongocxx::pool& pool);
};


#endif //PATH_TRACKING_ADD_FRIEND_HANDLER_HPP
