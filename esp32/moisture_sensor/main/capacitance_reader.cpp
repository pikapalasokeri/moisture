#include "capacitance_reader.h"

#include <chrono>
#include <thread>

CapacitanceReader::CapacitanceReader()
  : adc_reader_{std::vector<AdcGpioPin>{AdcGpioPin::GPIO34, AdcGpioPin::GPIO35}}
  , pwm_controller_{std::vector<int>{22, 23}}
{
}

std::vector<std::uint32_t>
CapacitanceReader::getReadings()
{
  constexpr int num_samples{64};
  constexpr int millis{3000};

  auto activity{pwm_controller_.getScopedActivity()};

  std::this_thread::sleep_for(std::chrono::milliseconds(millis));

  return adc_reader_.getReadings(num_samples);
}
