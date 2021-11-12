#ifndef ADC_READER_H
#define ADC_READER_H

#include "esp_adc_cal.h"

#include <cstdint>
#include <vector>

// Pin mapping:
// AdcReader |
//  channel  | GPIO pin
//        0  |  34
//        1  |  35
class AdcReader
{
public:
  static constexpr size_t MAX_CHANNELS{2};
  AdcReader(int const num_channels);
  std::vector<std::uint32_t> getReadings(int const num_avg_samples);

private:
  int num_channels_;
  esp_adc_cal_characteristics_t adc_characteristics_;
};

#endif // ADC_READER_H
