#pragma once

#include <vector>

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace ac_sense {

// Reads a chain of shift-register boards over a custom GRAB/CLOCK/DATA
// protocol:
//
// Bit 0 (live read, pre-latch):
//   GRAB LOW -> CLOCK high -> CLOCK low -> sample DATA
//   DATA is the live output of board 0's Schmitt/NAND chain, bypassing
//   its flip-flop.
//
// Bits 1..N-1 (latched read):
//   GRAB HIGH -> latches all flip-flops simultaneously.
//   For each bit: CLOCK high -> CLOCK low -> sample DATA.
//   GRAB stays HIGH for the remainder of the read (and between reads).
class AcSenseComponent : public PollingComponent {
 public:
  void set_pin_clock(GPIOPin *pin) { pin_clock_ = pin; }
  void set_pin_grab(GPIOPin *pin) { pin_grab_ = pin; }
  void set_pin_data(GPIOPin *pin) { pin_data_ = pin; }
  void set_num_channels(uint8_t num_channels) { channels_.resize(num_channels, nullptr); }
  void set_clock_half_us(uint32_t us) { clock_half_us_ = us; }
  void set_grab_settle_us(uint32_t us) { grab_settle_us_ = us; }

  // index is 0-based; channel 1 in YAML maps to index 0.
  void register_channel(uint8_t index, binary_sensor::BinarySensor *sensor);

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  GPIOPin *pin_clock_{nullptr};
  GPIOPin *pin_grab_{nullptr};
  GPIOPin *pin_data_{nullptr};
  uint32_t clock_half_us_{10};
  uint32_t grab_settle_us_{50};
  std::vector<binary_sensor::BinarySensor *> channels_;
};

}  // namespace ac_sense
}  // namespace esphome
