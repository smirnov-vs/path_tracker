#include "curl.hpp"
#include "json.hpp"

#include <curl/curl.h>

Curl::Curl() {
    curl_global_init(CURL_GLOBAL_ALL);
}

Curl::~Curl() {
    curl_global_cleanup();
}

bool Curl::sendPostMessage(const std::string& url, const std::string& message, const Headers& headers) const {
    CURL* curl = curl_easy_init();

    if (curl) {
        curl_slist *headers_list = nullptr;
        for (const auto& header : headers) {
            headers_list = curl_slist_append(headers_list, header.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

        auto res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            return false;

        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}
