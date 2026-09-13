#pragma once
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace ds3502 {

static const uint8_t DS3502_REG_WR_IVR = 0x00;
static const uint8_t DS3502_REG_CR = 0x02;
static const uint8_t DS3502_MODE_BIT = 0x80; // 0x80 = volatile mode

class DS3502Component : public Component, public i2c::I2CDevice {
 public:
  void dump_config() override;

  void set_restore_value(bool restore) { this->restore_value_ = restore; }
  void set_name(const std::string &name) { this->name_ = name; }

  void set_wiper(uint8_t value);
  uint8_t get_wiper();

  void save_current_to_eeprom();

  std::string get_log_prefix() const;

 protected:
  bool ensure_mode_(bool persist);
  void write_mode_(bool persist);

  bool restore_value_{false};
  std::string name_{};
};

class DS3502Output : public output::FloatOutput, public Parented<DS3502Component> {
 protected:
  void write_state(float state) override;
};

class DS3502SaveButton : public button::Button, public Parented<DS3502Component> {
 protected:
  void press_action() override { this->parent_->save_current_to_eeprom(); }
};

}  // namespace ds3502
}  // namespace esphome