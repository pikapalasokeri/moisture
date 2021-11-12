#ifndef CAPACITANCE_READER_H
#define CAPACITANCE_READER_H

#include "esp_adc_cal.h"
#include "pwm_controller.h"

#include <cstdint>
#include <vector>

class CapacitanceReader
{
public:
  CapacitanceReader(int const num_sensors);

  std::vector<std::uint32_t> get_readings();

private:
  int const num_sensors_;
  esp_adc_cal_characteristics_t adc_characteristics_;
  PwmController pwm_controller_;
};

#endif // CAPACITANCE_READER_H
