#include "capacitance_reader.h"

#include <chrono>
#include <thread>

CapacitanceReader::CapacitanceReader()
  : adc_reader_{std::vector<AdcGpioPin>{AdcGpioPin::GPIO35}}
  , pwm_controller_{std::vector<int>{21}}
{
}

std::vector<std::uint32_t>
CapacitanceReader::getReadings()
{
  auto activity{pwm_controller_.getScopedActivity()};

  constexpr int millis{5000};
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));

  constexpr int num_samples{64};
  return adc_reader_.getReadings(num_samples);
}
