/*
 * harness: standalone debugging tool for the article scraping pipeline.
 *
 * Fetches the URL given as its only argument with curl, feeds the HTML
 * into ArticleScraper with debug logging enabled, and prints the
 * scraper's LOG_INFO/LOG_DEBUG output to stdout (progress and errors
 * go to stderr). The extracted ContentBlock is discarded; the tool is
 * only meant to exercise and debug the scraper outside the full TUI.
 *
 * Usage: harness [url]
 */
#include <cstdlib>
#include <curl/curl.h>

#include <iostream>
#include <string>

#include "model/ArticleScraper/ArticleScraper.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  auto *response = static_cast<std::string *>(userp);
  response->append(static_cast<char *>(contents), size * nmemb);
  return size * nmemb;
}

int main(int argc, char *argv[]) {
  if(argc <= 1 or argc > 2)
  {
    std::cout << "Incorecct Usage: harness [url]" << std::endl;

    return EXIT_FAILURE;
  }
  auto kUrl = argv[1];
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *handle = curl_easy_init();
  if (handle == nullptr) {
    std::cerr << "ERROR: curl_easy_init failed\n";
    curl_global_cleanup();
    return 1;
  }

  std::string html;
  long http_code = 0;

  curl_easy_setopt(handle, CURLOPT_URL, kUrl);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &html);
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(handle, CURLOPT_TIMEOUT, 100L);
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

  ArticleScraper scraper(true);
  auto resutl = scraper(kUrl, html);
  return EXIT_SUCCESS;
}
