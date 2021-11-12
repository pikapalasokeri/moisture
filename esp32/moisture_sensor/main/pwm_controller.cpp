#include "pwm_controller.h"

#include "esp_err.h"

#include <cassert>
#include <cstring>

namespace
{
constexpr std::array<int, PwmController::MAX_CHANNELS> s_gpio_pins{3, 21};
constexpr std::array<ledc_channel_t, PwmController::MAX_CHANNELS> s_channels{
  LEDC_CHANNEL_0,
  LEDC_CHANNEL_1};
} // namespace

PwmController::PwmController(int const num_sensors)
  : num_sensors_{num_sensors}
{
  assert(num_sensors_ >= 0);
  assert(num_sensors_ <= MAX_CHANNELS);

  ledc_timer_config_t ledc_timer;
  std::memset(&ledc_timer, 0, sizeof(ledc_timer));
  ledc_timer.duty_resolution = LEDC_TIMER_1_BIT; // resolution of PWM duty
  ledc_timer.freq_hz = 200000;                   // frequency of PWM signal
  ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;   // timer mode
  ledc_timer.timer_num = LEDC_TIMER_1;           // timer index
  ledc_timer.clk_cfg = LEDC_AUTO_CLK;            // Auto select the source clock
  esp_err_t const err{ledc_timer_config(&ledc_timer)};
  if (err != ESP_OK)
  {
    printf("Unable to config ledc timer: %d.", err);
  }

  for (int i = 0; i < MAX_CHANNELS; ++i)
  {
    ledc_channel_config_t basic_config;
    std::memset(&basic_config, 0, sizeof(basic_config));
    basic_config.channel = s_channels[i];
    basic_config.duty = 0;
    basic_config.gpio_num = s_gpio_pins[i];
    basic_config.speed_mode = LEDC_LOW_SPEED_MODE;
    basic_config.hpoint = 0;
    basic_config.timer_sel = LEDC_TIMER_1;

    ledc_channel_config_t start_config{basic_config};
    start_config.duty = 1;
    ledc_channel_config_t stop_config{basic_config};
    stop_config.duty = 0;

    start_channel_configs_[i] = start_config;
    stop_channel_configs_[i] = stop_config;
  }
}

PwmController::ScopedActivation
PwmController::getScopedActivity()
{
  return ScopedActivation{this, PrivateCreationTag()};
}

PwmController::ScopedActivation::ScopedActivation(PwmController* pwm_controller,
                                                  PrivateCreationTag tag)
  : pwm_controller_{pwm_controller}
{
  static_cast<void>(tag);
  pwm_controller_->start();
}

PwmController::ScopedActivation::~ScopedActivation()
{
  pwm_controller_->stop();
}

void
PwmController::start()
{
  for (int i = 0; i < num_sensors_; ++i)
  {
    esp_err_t const err{ledc_channel_config(&start_channel_configs_[i])};
    if (err != ESP_OK)
    {
      printf("Unable to config start ledc channel %d: %d.", i, err);
    }
  }
}

void
PwmController::stop()
{
  for (int i = 0; i < num_sensors_; ++i)
  {
    esp_err_t const err{ledc_channel_config(&stop_channel_configs_[i])};
    if (err != ESP_OK)
    {
      printf("Unable to config start ledc channel %d: %d.", i, err);
    }
  }
}
