#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

#include "lwip/err.h"

#include "capacitance_reader.h"
#include "post_http.h"
#include "wifi_sta.h"

const char* TAG = "main";

extern "C" void
app_main(void)
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);


  CapacitanceReader cap_reader{2};
  std::vector<std::uint32_t> const values{cap_reader.getReadings()};

  wifiStaInit();

  for (int i = 0; i < values.size(); ++i)
  {
    auto const v{values[i]};
    post_http(v, i, wifiGetSSID());
  }

  wifiStaDeinit();

  constexpr int deep_sleep_sec = 3600;
  ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
  esp_deep_sleep(1000000LL * deep_sleep_sec);
}
