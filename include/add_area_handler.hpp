#ifndef PATH_TRACKING_ADD_AREA_HANDLER_HPP
#define PATH_TRACKING_ADD_AREA_HANDLER_HPP

#include "auth_handler.hpp"

class AddAreaHandler : public AuthHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    AddAreaHandler(mongocxx::pool &pool);
};

#endif //PATH_TRACKING_ADD_AREA_HANDLER_HPP
