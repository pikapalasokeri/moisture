#ifndef CAPACITANCE_READER_H
#define CAPACITANCE_READER_H

#include "adc_reader.h"
#include "pwm_controller.h"

#include <cstdint>
#include <vector>

class CapacitanceReader
{
public:
  CapacitanceReader(int const num_sensors);

  std::vector<std::uint32_t> getReadings();

private:
  AdcReader adc_reader_;
  PwmController pwm_controller_;
};

#endif // CAPACITANCE_READER_H
