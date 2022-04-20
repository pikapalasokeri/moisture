#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "driver/ledc.h"

#include <vector>

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

  PwmController(std::vector<int> const& gpio_pins);

  ScopedActivation getScopedActivity();

private:
  void start();
  void stop();

  std::vector<ledc_channel_config_t> start_channel_configs_;
  std::vector<ledc_channel_config_t> stop_channel_configs_;
};

#endif // PWM_CONTROLLER_H
