#ifndef PATH_TRACKING_TRACK_HANDLER_H
#define PATH_TRACKING_TRACK_HANDLER_H

#include "auth_handler.hpp"

#include <clickhouse/client.h>

class TrackHandler final : public AuthHandler {

    static thread_local clickhouse::Client client;

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const User& user) override;

public:
    TrackHandler(mongocxx::pool& pool);
};

#endif //PATH_TRACKING_TRACK_HANDLER_H
