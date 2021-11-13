#include "post_http.h"

#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_system.h"

#include <array>
#include <cstring>
#include <iomanip>
#include <ios>
#include <sstream>

namespace
{
char const* TAG = "post_http";

esp_err_t
handleHttpEvent(esp_http_client_event_t* evt)
{
  switch (evt->event_id)
  {
    case HTTP_EVENT_ERROR:
      ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
      printf("%.*s", evt->data_len, (char*)evt->data);
      break;
    case HTTP_EVENT_ON_DATA:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response(evt->client)) {
        printf("%.*s", evt->data_len, (char*)evt->data);
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}

std::string
getFormattedMacAddress()
{
  std::array<std::uint8_t, 6> mac;
  esp_efuse_mac_get_default(mac.data());

  std::stringstream out;
  out << std::hex;
  for (int i = 0; i < mac.size(); ++i)
  {
    out << std::setfill('0') << std::setw(2) << int(mac[i]);
    if (i < mac.size() - 1)
    {
      out << ":";
    }
  }
  return std::string(out.str());
}

class HttpClient final
{
public:
  HttpClient(char const* url)
    : url_{url}
  {
    std::memset(&config_, 0, sizeof(config_));
    config_.event_handler = handleHttpEvent;
    config_.url = url_;
    config_.timeout_ms = 5000;
    client_ = esp_http_client_init(&config_);
  }

  bool post(std::string const post_data)
  {
    esp_http_client_set_method(client_, HTTP_METHOD_POST);
    esp_http_client_set_header(
      client_, "Content-Type", "application/json; charset=utf-8");

    ESP_LOGI(TAG, "Posting to %s", config_.url);

    esp_err_t err{esp_http_client_set_post_field(
      client_, post_data.data(), post_data.size())};
    if (err != ESP_OK)
    {
      ESP_LOGI(TAG, "Failed to set post data.");
      return false;
    }

    err = esp_http_client_perform(client_);
    if (err == ESP_OK)
    {
      ESP_LOGI(TAG,
               "Status = %d, content_length = %d",
               esp_http_client_get_status_code(client_),
               esp_http_client_get_content_length(client_));
    }
    else
    {
      ESP_LOGI(TAG, "Failed to post http.");
    }

    return err == ESP_OK;
  }

  ~HttpClient() { esp_http_client_cleanup(client_); }

private:
  char const* url_;
  esp_http_client_config_t config_;
  esp_http_client_handle_t client_;
};

} // namespace

bool
post_http(std::uint32_t const raw_value,
          int const sensor_id,
          std::string const location)
{
  bool ret{false};
  HttpClient http_client{"http://pikapalasokeri.se:8080/new_moisture_reading"};

  std::stringstream ss;

  std::string const mac_address{getFormattedMacAddress()};

  // clang-format off
  ss << "{\"raw_value\":" << raw_value << ","
     <<  "\"sensor_id\":\"" << mac_address << " - "<< sensor_id << "\","
     <<  "\"location\":\"" << location << "\","
     <<  "\"sensor_type\":\"moisture\","
     <<  "\"hash\":" << 0
     << "}";
  // clang-format on
  std::string const post_data(ss.str());
  ESP_LOGI(TAG, "%s", post_data.c_str());
  http_client.post(post_data);

  return ret;
}
