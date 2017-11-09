#ifndef PATH_TRACKING_ME_HANDLER_H
#define PATH_TRACKING_ME_HANDLER_H

#include "auth_handler.hpp"

#include <mongocxx/pool.hpp>

class MeHandler final : public AuthHandler {

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    MeHandler(mongocxx::pool& pool);
};


#endif //PATH_TRACKING_ME_HANDLER_H
