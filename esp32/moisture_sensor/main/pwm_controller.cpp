#include "pwm_controller.h"

#include "esp_err.h"
#include "driver/gpio.h"

#include <array>
#include <cassert>
#include <cstring>

namespace
{
constexpr std::array<ledc_channel_t, PwmController::MAX_CHANNELS> s_channels{
  LEDC_CHANNEL_0,
  LEDC_CHANNEL_1};
} // namespace

PwmController::PwmController(std::vector<int> const& gpio_pins)
{
  assert(gpio_pins.size() <= MAX_CHANNELS);

  ledc_timer_config_t ledc_timer;
  std::memset(&ledc_timer, 0, sizeof(ledc_timer));
  ledc_timer.duty_resolution = LEDC_TIMER_1_BIT; // resolution of PWM duty
  ledc_timer.freq_hz = 800000;                   // frequency of PWM signal
  ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;   // timer mode
  ledc_timer.timer_num = LEDC_TIMER_1;           // timer index
  ledc_timer.clk_cfg = LEDC_AUTO_CLK;            // Auto select the source clock
  esp_err_t const err{ledc_timer_config(&ledc_timer)};
  if (err != ESP_OK)
  {
    printf("Unable to config ledc timer: %d.", err);
  }

  for (std::size_t i = 0; i < gpio_pins.size(); ++i)
  {
    ledc_channel_config_t basic_config;
    std::memset(&basic_config, 0, sizeof(basic_config));
    basic_config.channel = s_channels[i];
    basic_config.duty = 0;
    basic_config.gpio_num = gpio_pins[i];
    basic_config.speed_mode = LEDC_LOW_SPEED_MODE;
    basic_config.hpoint = 0;
    basic_config.timer_sel = LEDC_TIMER_1;

    ledc_channel_config_t start_config{basic_config};
    start_config.duty = 1;
    ledc_channel_config_t stop_config{basic_config};
    stop_config.duty = 0;

    start_channel_configs_.push_back(start_config);
    stop_channel_configs_.push_back(stop_config);
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
  for (std::size_t i = 0; i < start_channel_configs_.size(); ++i)
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
  for (std::size_t i = 0; i < stop_channel_configs_.size(); ++i)
  {
    auto const& config = stop_channel_configs_[i];
    esp_err_t const err{ledc_channel_config(&config)};
    if (err != ESP_OK)
    {
      printf("Unable to config stop ledc channel %d: %d.", i, err);
    }

    esp_err_t const err2{ledc_stop(config.speed_mode, config.channel, 0U)};
    if (err2 != ESP_OK)
    {
      printf("Unable to stop ledc channel %d: %d.", i, err2);
    }
  }
}
