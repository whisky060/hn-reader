#include <curl/curl.h>

#include <iostream>
#include <string>

#include "model/ArticleScraper.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  auto *response = static_cast<std::string *>(userp);
  response->append(static_cast<char *>(contents), size * nmemb);
  return size * nmemb;
}

int main() {
  const std::string kUrl = "https://www.tomshardware.com/tech-industry/big-tech/lg-strongly-denies-tv-security-claims-says-tracking-and-snooping-concerns-not-true-online-investigation-claims-216-000-000-spy-tvs-record-audio";

  curl_global_init(CURL_GLOBAL_ALL);
  CURL *handle = curl_easy_init();
  if (handle == nullptr) {
    std::cerr << "ERROR: curl_easy_init failed\n";
    curl_global_cleanup();
    return 1;
  }

  std::string html;
  long http_code = 0;

  curl_easy_setopt(handle, CURLOPT_URL, kUrl.c_str());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &html);
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(handle, CURLOPT_TIMEOUT, 10L);
  curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");
  curl_easy_setopt(handle, CURLOPT_USERAGENT,
                   "Mozilla/5.0 (compatible; hn-tui-reader/1.0)");

  CURLcode res = curl_easy_perform(handle);
  if (res != CURLE_OK) {
    std::cerr << "ERROR: curl failed: " << curl_easy_strerror(res) << "\n";
    curl_easy_cleanup(handle);
    curl_global_cleanup();
    return 1;
  }
  curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code);
  curl_easy_cleanup(handle);
  curl_global_cleanup();

  if (http_code > 400) {
    std::cerr << "ERROR: http code " << http_code << "\n";
    return 1;
  }

  std::cerr << "[fetched " << html.size() << " bytes from " << kUrl << "]\n";

  ArticleScraper scraper;
  std::string article = scraper(html);
  std::cout << article << "\n";
  std::cerr << "[scraped " << article.size() << " bytes]\n";

  return 0;
}
