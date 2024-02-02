// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "curl_json.h"
#include "util.h"

#include <openssl/evp.h>

static size_t writer(char *in, size_t size, size_t nmemb, std::string *out)
{
      out->append((char*)in, size * nmemb);
      return size * nmemb;
}

void getHttpsJson(std::string url, JsonDownload* reply, int headerType)
{
    reply->failed = false;
    reply->complete = false;
    reply->URL = url;
    std::string response_string;

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl;
    CURLcode res;

    struct curl_slist *headers=NULL; // init to NULL is important
    switch(headerType) {
      case STANDARD_HEADERS:
          headers = curl_slist_append(headers, "Accept: application/json");
          break;
      case CG_HEADERS:
          headers = curl_slist_append(headers, "Accept: application/json");
          break;
      case GITHUB_HEADERS:
          headers = curl_slist_append(headers, "Accept: application/vnd.github+json");
          headers = curl_slist_append(headers, "User-Agent: PRCYcoin");
          break;
    }

    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");

    curl = curl_easy_init();
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_URL, reply->URL.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        res = curl_easy_perform(curl);

        if(CURLE_OK == res) {
            char *ct;
            /* ask for the content-type */
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if((CURLE_OK == res) && ct) {
                reply->response = std::move(response_string);
                reply->failed = false;
                reply->complete = true;
            }
        } else {
          reply->response = "";
          reply->failed = false;
          reply->complete = false;
        }
    }
    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();
}
