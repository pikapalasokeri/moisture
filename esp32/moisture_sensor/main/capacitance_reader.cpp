#include "capacitance_reader.h"

CapacitanceReader::CapacitanceReader(int const num_sensors)
  : adc_reader_{std::vector<AdcGpioPin>{AdcGpioPin::GPIO33}}
  , pwm_controller_{std::vector<int>{21}}
{
}

std::vector<std::uint32_t>
CapacitanceReader::getReadings()
{
  auto activity{pwm_controller_.getScopedActivity()};

  // TODO: pause a few seconds.
  constexpr int num_samples{64};
  return adc_reader_.getReadings(num_samples);
}
