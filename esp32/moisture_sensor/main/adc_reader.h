#ifndef ADC_READER_H
#define ADC_READER_H

#include "esp_adc_cal.h"

#include <cstdint>
#include <vector>


enum class AdcGpioPin
{
  GPIO32 = 0,
  GPIO33,
  GPIO34,
  GPIO35,
  GPIO36,
  GPIO37,
  GPIO38,
  GPIO39
};

class AdcReader
{
public:
  AdcReader(std::vector<AdcGpioPin> const& input_pins);
  std::vector<std::uint32_t> getReadings(int const num_avg_samples);

private:
  std::vector<adc1_channel_t> const channels_;
  esp_adc_cal_characteristics_t adc_characteristics_;
};

#endif // ADC_READER_H
