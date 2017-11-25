#ifndef PATH_TRACKING_DELETE_AREA_HANDLER_HPP
#define PATH_TRACKING_DELETE_AREA_HANDLER_HPP

#include "auth_handler.hpp"

class DeleteAreaHandler : public AuthHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    DeleteAreaHandler(mongocxx::pool &pool);
};


#endif //PATH_TRACKING_DELETE_AREA_HANDLER_HPP
