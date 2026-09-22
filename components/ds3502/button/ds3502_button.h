#pragma once
#include "esphome/core/helpers.h"
#include "esphome/components/button/button.h"
#include "../ds3502.h"

namespace esphome {
namespace ds3502 {

class DS3502Button : public button::Button, public Parented<DS3502Component> {
 protected:
  void press_action() override { this->parent_->save_current_to_eeprom(); }
};

}  // namespace ds3502
}  // namespace esphome