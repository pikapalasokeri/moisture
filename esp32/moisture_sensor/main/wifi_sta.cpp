#include "wifi_sta.h"

#include <cstdio>
#include <cstring>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

namespace
{
/* The examples use WiFi configuration that you can set via project
 configuration menu

 If you'd rather not, just change the below entries to strings with
 the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about
 * two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_SUCCESSFUL_DISCONNECT_BIT BIT2

const char* TAG = "wifi_sta";

int s_retry_num = 0;
bool s_should_disconnect = false;

esp_event_handler_instance_t s_instance_any_id;
esp_event_handler_instance_t s_instance_got_ip;

void
event_handler(void* arg,
              esp_event_base_t event_base,
              int32_t event_id,
              void* event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    if (s_should_disconnect)
    {
      xEventGroupClearBits(s_wifi_event_group, 0xFF);
      xEventGroupSetBits(s_wifi_event_group, WIFI_SUCCESSFUL_DISCONNECT_BIT);
      ESP_LOGI(TAG, "disconnect to the AP by choice");
    }
    else if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
    {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAG, "retry to connect to the AP");
    }
    else
    {
      xEventGroupClearBits(s_wifi_event_group, 0xFF);
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
      ESP_LOGI(TAG, "connect to the AP fail");
    }
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupClearBits(s_wifi_event_group, 0xFF);
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}
} // namespace

bool
is_connected()
{
  EventBits_t bits = xEventGroupGetBits(s_wifi_event_group);

  return (bits & WIFI_CONNECTED_BIT);
}

void
wifi_sta_init()
{
  s_should_disconnect = false;
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &s_instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &s_instance_got_ip));

  wifi_pmf_config_t pmf_cfg;
  pmf_cfg.capable = true;
  pmf_cfg.required = false;

  wifi_sta_config_t sta;
  std::memset(&sta, 0, sizeof(sta));

  std::sprintf(
    reinterpret_cast<char*>(&sta.ssid[0]), "%s", EXAMPLE_ESP_WIFI_SSID);
  std::sprintf(
    reinterpret_cast<char*>(&sta.password[0]), "%s", EXAMPLE_ESP_WIFI_PASS);
  sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  sta.pmf_cfg = pmf_cfg;

  wifi_config_t wifi_config;
  std::memset(&wifi_config, 0, sizeof(wifi_config));
  wifi_config.sta = sta;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or
   * connection failed for the maximum number of re-tries (WIFI_FAIL_BIT). The
   * bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, // Clear on exit.
                                         pdFALSE, // Wait for all bits.
                                         portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we
   * can test which event actually happened. */

  if (bits & WIFI_CONNECTED_BIT)
  {
    ESP_LOGI(TAG,
             "connected to ap SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID,
             EXAMPLE_ESP_WIFI_PASS);
  }
  else if (bits & WIFI_FAIL_BIT)
  {
    ESP_LOGI(TAG,
             "Failed to connect to SSID:%s, password:%s",
             EXAMPLE_ESP_WIFI_SSID,
             EXAMPLE_ESP_WIFI_PASS);
  }
  else
  {
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
  }

  /* The event will not be processed after unregister */
}

void
wifi_sta_deinit()
{
  if (is_connected())
  {
    s_should_disconnect = true;
    if (esp_wifi_disconnect() == ESP_OK)
    {
      ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        IP_EVENT, IP_EVENT_STA_GOT_IP, s_instance_got_ip));
      ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        WIFI_EVENT, ESP_EVENT_ANY_ID, s_instance_any_id));
      vEventGroupDelete(s_wifi_event_group);

      ESP_LOGI(TAG, "wifi disconnected");
      if (esp_wifi_stop() == ESP_OK)
      {
        ESP_LOGI(TAG, "wifi stopped");
        if (esp_wifi_deinit() == ESP_OK)
        {
          ESP_LOGI(TAG, "wifi deinited");
        }
      }
    }
  }
}
