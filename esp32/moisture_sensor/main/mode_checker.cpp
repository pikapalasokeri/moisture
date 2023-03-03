#include "mode_checker.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <chrono>
#include <cstdint>
#include <thread>

namespace
{
const char* TAG = "mode_checker";
}

Mode getMode()
{
  constexpr gpio_num_t pin{GPIO_NUM_23};

  gpio_reset_pin(pin);

  esp_err_t const err1{gpio_pullup_en(pin)};
  if (err1 != ESP_OK)
  {
    ESP_LOGI(TAG, "enable pullup failed.\n");
    return Mode::MoistureReading;
  }

  esp_err_t const err2{gpio_set_direction(pin, GPIO_MODE_INPUT)};
  if (err2 != ESP_OK)
  {
    ESP_LOGI(TAG, "set direction failed.\n");
    return Mode::MoistureReading;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  std::int32_t const level{gpio_get_level(pin)};
  gpio_reset_pin(pin);
  if (level == 0)
  {
    return Mode::Pinging;
  }
  else
  {
    return Mode::MoistureReading;
  }
}
