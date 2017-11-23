#ifndef PATH_TRACKING_DELETE_FRIEND_HANDLER_HPP
#define PATH_TRACKING_DELETE_FRIEND_HANDLER_HPP

#include "auth_handler.hpp"

class DeleteFriendHandler : public AuthHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    DeleteFriendHandler(mongocxx::pool &pool);
};


#endif //PATH_TRACKING_DELETE_FRIEND_HANDLER_HPP
