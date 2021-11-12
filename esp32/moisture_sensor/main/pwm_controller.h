#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "driver/ledc.h"

#include <array>

class PwmController
{
private:
  class PrivateCreationTag{};

public:
  class ScopedActivation
  {
  public:
    ScopedActivation(PwmController* pwm_controller, PrivateCreationTag tag);
    ~ScopedActivation();

  private:
    PwmController* pwm_controller_;
  };

  static constexpr int MAX_CHANNELS{2};

  PwmController(int const num_sensors);

  ScopedActivation getScopedActivity();

private:
  void start();
  void stop();

  int const num_sensors_;

  std::array<ledc_channel_config_t, MAX_CHANNELS> start_channel_configs_;
  std::array<ledc_channel_config_t, MAX_CHANNELS> stop_channel_configs_;
};

#endif // PWM_CONTROLLER_H
