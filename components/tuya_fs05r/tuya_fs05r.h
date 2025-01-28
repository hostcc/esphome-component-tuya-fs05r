// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2025 Ilia Sotnikov

#pragma once
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/tuya/tuya.h"

using esphome::tuya::Tuya;
using esphome::tuya::TuyaCommand;
using esphome::tuya::TuyaCommandType;

// Since dimmer's MCU doesn't implement full Tuya MCU protocol, no direct
// inheritance needed - only `send_raw_command_()` method should be exposed
// publicly
class TuyaMinimal : public Tuya {
 public:
  void send_raw_command(TuyaCommand command) {
    this->send_raw_command_(command);
  }
};

namespace esphome {
namespace tuya_fs05r {

const char *TAG = "tuya_fs05r";

class TuyaFS05R :
  public light::LightOutput, public uart::UARTDevice, public Component {
 public:
  void setup() override {
    this->tuya_ = new TuyaMinimal();
    // Wire the UART to Tuya component instance
    this->tuya_->set_uart_parent(this->parent_);
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "Tuya FS05R:");
    ESP_LOGCONFIG(TAG, "  Max Value: %.1f", this->max_value_);
    ESP_LOGCONFIG(TAG, "  Min Value: %.1f", this->min_value_);
  }

  void set_min_value(float min_value) { this->min_value_ = min_value; }

  void set_max_value(float max_value) { this->max_value_ = max_value; }

  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
    return traits;
  }

  void write_state(light::LightState *state) override {
    float brightness;
    state->current_values_as_brightness(&brightness);

    // Convert brightness to value within the range the MCU uses
    int value = (
      brightness * (this->max_value_ - this->min_value_)) + this->min_value_;

    // Gamma correction could result in brightness below 0.001,
    // which results a value of 0 - treat the case as 1 to avoid
    // brightness adjustment turning off the light while not being at 0%
    if (value == 0 && brightness > 0.0) {
      value = 1;
    }

    // Split value into two bytes
    uint8_t value_h = (value >> 8) & 0xFF;
    uint8_t value_l = value & 0xFF;
    auto payload = std::vector<uint8_t>{0x00, value_h, value_l};

    // No feedback expected, just send it
    ESP_LOGD(TAG,
      "Setting brightness: %f, resulting value: 0x%04x", brightness, value);
    this->tuya_->send_raw_command(
      TuyaCommand{.cmd = (TuyaCommandType) 0x30, .payload = payload});
  }

 protected:
  TuyaMinimal *tuya_{};
  float min_value_;
  float max_value_;
};

}  // namespace tuya_fs05r
}  // namespace esphome

