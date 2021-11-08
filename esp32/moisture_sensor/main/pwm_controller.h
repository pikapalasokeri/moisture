#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "driver/ledc.h"

#include <array>

class PwmController
{
public:
  static constexpr int MAX_CHANNELS{2};

  PwmController(int const num_sensors);

  void start();

  void stop();
private:
  int const num_sensors_;

  std::array<ledc_channel_config_t, MAX_CHANNELS> start_channel_configs_;
  std::array<ledc_channel_config_t, MAX_CHANNELS> stop_channel_configs_;
};

#endif  // PWM_CONTROLLER_H
