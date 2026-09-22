#pragma once
#include "esphome/core/helpers.h"
#include "esphome/components/output/float_output.h"
#include "../ds3502.h"

namespace esphome {
namespace ds3502 {

class DS3502Output : public output::FloatOutput, public Parented<DS3502Component> {
 protected:
  void write_state(float state) override;
};

}  // namespace ds3502
}  // namespace esphome