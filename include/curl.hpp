#ifndef PATH_TRACKING_CURL_HPP
#define PATH_TRACKING_CURL_HPP

#include <string>
#include <vector>

struct Curl {
    using Headers = std::vector<std::string>;

    bool sendPostMessage(const std::string& url, const std::string& message, const Headers& headers = {}) const;

    Curl();
    ~Curl();
};

#endif //PATH_TRACKING_CURL_HPP
