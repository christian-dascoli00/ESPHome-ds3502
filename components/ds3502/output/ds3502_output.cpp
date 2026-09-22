#include "ds3502_output.h"
#include <cmath>
#include <algorithm>

namespace esphome {
namespace ds3502 {

void DS3502Output::write_state(float state) {
  if (std::isnan(state))
    state = 0.0f;

  int wiper_value = static_cast<int>(std::round(state * 127.0f));
  wiper_value = std::max(0, std::min(127, wiper_value));

  this->parent_->set_wiper(wiper_value);
}

}  // namespace ds3502
}  // namespace esphome