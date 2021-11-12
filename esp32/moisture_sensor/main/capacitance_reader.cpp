#include "capacitance_reader.h"

CapacitanceReader::CapacitanceReader(int const num_sensors)
  : adc_reader_{num_sensors}
  , pwm_controller_{num_sensors}
{
}

std::vector<std::uint32_t>
CapacitanceReader::getReadings()
{
  auto activity{pwm_controller_.getScopedActivity()};

  // pause a few seconds.
  constexpr int num_samples{64};
  return adc_reader_.getReadings(num_samples);
}
