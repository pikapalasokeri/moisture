#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"

#include "capacitance_reader.h"
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

  // read analog values.

  CapacitanceReader cap_reader{2};

  for (int i = 0; i < 10; ++i)
  {
    std::vector<std::uint32_t> const readings{cap_reader.getReadings()};

    for (auto v : readings)
    {
      ESP_LOGI(TAG, "reading: %d", v);
    }
  }

  wifiStaInit();
  //upload analog values.
  wifiStaDeinit();
}
