#include "adc_reader.h"

#include "driver/adc.h"
#include "driver/gpio.h"

#include <array>

namespace
{
#define DEFAULT_VREF 1100

// [...]  ADC1 (8 channels, attached to GPIOs 32 - 39) [...]
//  ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO36 */
//  ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO37 */
//  ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO38 */
//  ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO39 */
//  ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO32 */
//  ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO33 */
//  ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO34 */
//  ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO35 */

constexpr std::array<adc1_channel_t, AdcReader::MAX_CHANNELS> s_channels{
  ADC1_CHANNEL_6,  // GPIO34
  ADC1_CHANNEL_7}; // GPIO35
constexpr adc_bits_width_t width = ADC_WIDTH_BIT_12;
constexpr adc_atten_t atten = ADC_ATTEN_DB_0;
constexpr adc_unit_t unit = ADC_UNIT_1;

void
check_efuse()
{
  // Check if TP is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
  {
    printf("eFuse Two Point: Supported\n");
  }
  else
  {
    printf("eFuse Two Point: NOT supported\n");
  }
  // Check Vref is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
  {
    printf("eFuse Vref: Supported\n");
  }
  else
  {
    printf("eFuse Vref: NOT supported\n");
  }
}

void
print_char_val_type(esp_adc_cal_value_t val_type)
{
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
  {
    printf("Characterized using Two Point Value\n");
  }
  else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
  {
    printf("Characterized using eFuse Vref\n");
  }
  else
  {
    printf("Characterized using Default Vref\n");
  }
}
} // namespace

AdcReader::AdcReader(int const num_channels)
  : num_channels_{num_channels}
{
  assert(num_channels_ >= 0);
  assert(num_channels_ <= s_channels.size());

  adc1_config_width(width);

  for (int i = 0; i < num_channels_; ++i)
  {
    adc1_channel_t const channel{s_channels[i]};
    adc1_config_channel_atten(channel, atten);
  }

  esp_adc_cal_value_t const val_type = esp_adc_cal_characterize(
    unit, atten, width, DEFAULT_VREF, &adc_characteristics_);

  check_efuse();
  print_char_val_type(val_type);
}

std::vector<std::uint32_t>
AdcReader::getReadings(int const num_avg_samples)
{
  std::vector<std::uint32_t> result(num_channels_, 0U);

  for (int channel_ix = 0; channel_ix < num_channels_; ++channel_ix)
  {
    adc1_channel_t const channel{s_channels[channel_ix]};
    std::uint32_t adc_reading{0};
    // Multisampling
    for (int i = 0; i < num_avg_samples; i++)
    {
      adc_reading += adc1_get_raw(channel);
    }
    adc_reading /= num_avg_samples;

    // Convert adc_reading to voltage in mV.
    std::uint32_t const voltage{
      esp_adc_cal_raw_to_voltage(adc_reading, &adc_characteristics_)};
    result[channel_ix] = voltage;
  }

  return result;
}