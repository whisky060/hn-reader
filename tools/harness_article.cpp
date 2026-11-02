/*
 * harness-article: renders a scraped article like the real app does.
 *
 * Synchronous companion to `harness`: fetches the URL given as its only
 * argument with curl, runs the result through ArticleScraper (debug
 * logging off), and then displays the extracted ContentBlock in a full
 * FTXUI terminal UI by reusing the real ArticleView plus the app's
 * BaseTemplate (art loading state, scrolling, Esc to exit, Ctrl+O to
 * open the url in the browser, bottom shortcut bar).
 *
 * The UI is only started after scraping has finished, so a scrape failure
 * is reported on stderr and exits before any window appears.
 *
 * Usage: harness-article [url]
 */
#include <cstdlib>
#include <curl/curl.h>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#include <ftxui/component/app.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <image_view.hpp>

#include "concurrency/FetchResult.hpp"
#include "model/ArticleScraper/ArticleScraper.hpp"
#include "model/ContentBlock/ContentBlock.hpp"
#include "model/app_model.hpp"
#include "view/ArticleView/ArticleView.hpp"
#include "view/template/BaseTemplate.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  auto *response = static_cast<std::string *>(userp);
  response->append(static_cast<char *>(contents), size * nmemb);
  return size * nmemb;
}

int main(int argc, char *argv[]) {
  if(argc <= 1 or argc > 2)
  {
    std::cout << "Incorecct Usage: harness-article [url]" << std::endl;

    return EXIT_FAILURE;
  }
  const std::string kUrl = argv[1];
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

  auto content = std::make_shared<ContentBlock>();
  try {
    ArticleScraper scraper;
    *content = scraper(kUrl, html);
  } catch (const std::exception &e) {
    std::cerr << "ERROR: scraper failed: " << e.what() << "\n";
    return 1;
  } catch (...) {
    std::cerr << "ERROR: scraper failed with unknown exception\n";
    return 1;
  }

  ftxui::App app = ftxui::App::Fullscreen();
  ftxui::setOnImageLoadedCallback([&]() {
    app.PostEvent(ftxui::Event::Custom);
  });

  auto view = ArticleView(
      [&content](ArticleCallback cb) {
        cb(FetchResult<ContentBlock>::Success(*content));
      },
      app.ExitLoopClosure(), kUrl);

  app.Loop(BaseTemplate(view));
  return EXIT_SUCCESS;
}
