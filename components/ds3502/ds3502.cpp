#include "ds3502.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include <cmath>
#include <algorithm>

namespace esphome {
namespace ds3502 {

static const char *const TAG = "ds3502";

static const uint32_t EEPROM_WRITE_DELAY_MS = 100;

std::string DS3502Component::get_log_prefix() const {
  if (!this->name_.empty())
    return this->name_;
  char buf[8];
  snprintf(buf, sizeof(buf), "0x%02X", this->address_);
  return std::string(buf);
}

void DS3502Component::write_mode_(bool persist) {
  uint8_t cr = persist ? 0x00 : DS3502_MODE_BIT;    // 0x80 = SRAM/volatile mode, 0x00 = EEPROM/persistent mode
  this->write_byte(DS3502_REG_CR, cr);
}

bool DS3502Component::ensure_mode_(bool persist) {
  uint8_t cr = 0;
  if (!this->read_byte(DS3502_REG_CR, &cr)) {
    this->status_set_warning();
    return false;
  }
  this->status_clear_warning();

  uint8_t expected = persist ? 0x00 : DS3502_MODE_BIT;
  if (cr != expected) {
    ESP_LOGW(TAG, "[%s] Unexpected mode (read 0x%02X, expected 0x%02X): changing mode", this->get_log_prefix().c_str(),
             cr, expected);
    this->write_mode_(persist);
  }
  return true;
}

void DS3502Component::set_wiper(uint8_t value) {
  if (value > 127)
    value = 127;

  if (!this->ensure_mode_(this->restore_value_)) {
    ESP_LOGW(TAG, "[%s] Not available", this->get_log_prefix().c_str());
    return;
  }

  if (!this->write_byte(DS3502_REG_WR_IVR, value)) {
    ESP_LOGW(TAG, "[%s] Wiper writing failed", this->get_log_prefix().c_str());
    this->status_set_warning();
    return;
  }
  ESP_LOGD(TAG, "[%s] Wiper set to %u", this->get_log_prefix().c_str(), value);
}

uint8_t DS3502Component::get_wiper() {
  uint8_t value = 0;
  if (!this->read_byte(DS3502_REG_WR_IVR, &value))
    this->status_set_warning();
  return value;
}

void DS3502Component::save_current_to_eeprom() {
  uint8_t current = this->get_wiper();
  ESP_LOGD(TAG, "[%s] Saving (%u) to EEPROM", this->get_log_prefix().c_str(), current);

  this->write_mode_(true);
  bool write_ok = this->write_byte(DS3502_REG_WR_IVR, current);
  this->set_timeout(EEPROM_WRITE_DELAY_MS, [this, write_ok, current]() {
    this->write_mode_(this->restore_value_);
    if (!write_ok) {
      ESP_LOGE(TAG, "[%s] Saving failed", this->get_log_prefix().c_str());
    } else {
      ESP_LOGI(TAG, "[%s] %u saved to EEPROM", this->get_log_prefix().c_str(), current);
      }
  });
}

void DS3502Component::dump_config() {
  ESP_LOGCONFIG(TAG, "DS3502 [%s]:", this->get_log_prefix().c_str());
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Restore value: %s", YESNO(this->restore_value_));
}

void DS3502Output::write_state(float state) {
  if (std::isnan(state))
    state = 0.0f;

  int wiper_value = static_cast<int>(std::round(state * 127.0f));
  wiper_value = std::max(0, std::min(127, wiper_value));

  this->parent_->set_wiper(wiper_value);
}

}  // namespace ds3502
}  // namespace esphome