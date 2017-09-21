#ifndef PATH_TRACKING_TRACK_HANDLER_H
#define PATH_TRACKING_TRACK_HANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <clickhouse/client.h>

class TrackHandler final : public Poco::Net::HTTPRequestHandler {

    static thread_local clickhouse::Client client;

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;

};

#endif //PATH_TRACKING_TRACK_HANDLER_H
