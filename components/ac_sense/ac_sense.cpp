#include "ac_sense.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ac_sense {

static const char *const TAG = "ac_sense";

void AcSenseComponent::register_channel(uint8_t index, binary_sensor::BinarySensor *sensor) {
  if (index >= channels_.size()) {
    ESP_LOGE(TAG, "Channel %u is out of range (num_channels: %u)", index + 1, channels_.size());
    return;
  }
  channels_[index] = sensor;
}

void AcSenseComponent::setup() {
  pin_clock_->setup();
  pin_grab_->setup();
  pin_data_->setup();
  pin_clock_->digital_write(false);
  pin_grab_->digital_write(false);
}

void AcSenseComponent::update() {
  // Bit 0: GRAB LOW, clock once, sample live (pre-latch) output.
  pin_grab_->digital_write(false);
  delayMicroseconds(clock_half_us_);

  pin_clock_->digital_write(true);
  delayMicroseconds(clock_half_us_);
  pin_clock_->digital_write(false);
  delayMicroseconds(clock_half_us_);

  bool bit0 = pin_data_->digital_read();

  // Latch all flip-flops.
  pin_grab_->digital_write(true);
  delayMicroseconds(grab_settle_us_);

  for (uint8_t ch = 0; ch < channels_.size(); ch++) {
    bool ac_present;

    if (ch == 0) {
      ac_present = bit0;
    } else {
      pin_clock_->digital_write(true);
      delayMicroseconds(clock_half_us_);
      pin_clock_->digital_write(false);
      delayMicroseconds(clock_half_us_);
      ac_present = pin_data_->digital_read();
    }

    auto *sensor = channels_[ch];
    if (sensor == nullptr)
      continue;

    if (!sensor->has_state() || sensor->state != ac_present) {
      sensor->publish_state(ac_present);
      ESP_LOGI(TAG, "Channel %u: %s", ch + 1, ac_present ? "AC PRESENT" : "AC LOST");
    }
  }
  // GRAB is left HIGH (idle state); it's only pulsed LOW at the start of
  // the next read cycle.
}

void AcSenseComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "AC Detector:");
  ESP_LOGCONFIG(TAG, "  Channels: %u", channels_.size());
  LOG_PIN("  Clock Pin: ", pin_clock_);
  LOG_PIN("  Grab Pin: ", pin_grab_);
  LOG_PIN("  Data Pin: ", pin_data_);
  ESP_LOGCONFIG(TAG, "  Clock half-period: %u us", clock_half_us_);
  ESP_LOGCONFIG(TAG, "  Grab settle time: %u us", grab_settle_us_);
  for (uint8_t ch = 0; ch < channels_.size(); ch++) {
    if (channels_[ch] == nullptr) {
      ESP_LOGW(TAG, "  Channel %u has no binary_sensor registered", ch + 1);
    }
  }
}

}  // namespace ac_sense
}  // namespace esphome
