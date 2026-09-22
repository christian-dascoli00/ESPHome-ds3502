# ESPHome - DS3502 Digital Potentiometer

ESPHome integration to control one or more DS3502 I2C digital potentiometers.

This component exposes each DS3502 as an [Output](https://esphome.io/components/output/) component, so its wiper position can be driven by any ESPHome entity (a `number`, etc). A dedicated `button` is also provided to permanently save the current wiper position to the chip's non-volatile memory.

The idea for this component comes from [TheStigh/ds3502](https://github.com/TheStigh/ds3502), with the addition of the `restore_value` option to choose whether the wiper position should be saved to and restored from the chip's EEPROM by default, and address selection via the A0/A1 pins.

To use this integration, add the configuration below to your ESPHome YAML file.

Choose your I2C GPIO pins and specify them under `i2c`, then declare one `ds3502` entry per potentiometer connected to the bus.

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/christian-dascoli00/ESPHome-ds3503
      ref: main
    components: [ds3502]

i2c:
  sda: GPIO21    # Your choice
  scl: GPIO22    # Your choice

ds3502:
  - id: pot1
    a0: false          # false = A0 tied to GND, true = A0 tied to VCC
    a1: false          # false = A1 tied to GND, true = A1 tied to VCC
    # address: 0x28    # Optional manual override, see below
    restore_value: false

output:
  - platform: ds3502
    id: pot_output1
    ds3502_id: pot1

button:
  - platform: ds3502
    name: "Save"
    ds3502_id: pot1
```

See [`ds3502.yaml`](./ds3502.yaml) for a complete configuration example with two potentiometers, including how to control each potentiometer with a `number` entity.

`ds3502:`
- `id` (Required, ID): ID used to reference this component instance.
- `i2c_id` (Optional, ID): ID of the `i2c` bus this component is attached to. Only needed if you have multiple I2C buses configured.
- `a0` (Optional, boolean): State of the chip's A0 address pin. `false` if A0 is tied to GND, `true` if tied to VCC. Used together with `a1` to compute the I2C address (`0x28`-`0x2B`). Defaults to `false`.
- `a1` (Optional, boolean): State of the chip's A1 address pin. `false` if A1 is tied to GND, `true` if tied to VCC. Defaults to `false`.
- `address` (Optional, int): I2C `address` option. Defaults to `0x28`. If `a0`/`a1` are also set, they overwrite whatever `address` you specify. Use `address` on its own (without `a0`/`a1`) if you need to manually set an address.
- `restore_value` (Optional, boolean): Whether to write the wiper position to the chip's non-volatile memory (EEPROM) every time it is set. Enable this option if you need the potentiometer to resume its last commanded position after a power cycle. Defaults to `false`. Note: EEPROM has a limited number of write cycles. Frequent writes wear it out.

`a0` and `a1` must be specified together. When both are set, the address is computed as:

| A1  | A0  | Address |
|-----|-----|---------|
| GND | GND | `0x28`  |
| GND | VCC | `0x29`  |
| VCC | GND | `0x2A`  |
| VCC | VCC | `0x2B`  |

`output:`
- `platform`: `ds3502`
- `id` (Required, ID): ID used to reference this output, e.g. from `output.set_level` actions.
- `ds3502_id` (Optional, ID): ID of the `ds3502` component to control, if you have multiple instances configured. Defaults to the only configured instance.
- All other options from [Output](https://esphome.io/components/output/index.html#config-output).

The wiper position is set via the standard `output.set_level` action, with `level` ranging from `0.0` to `1.0`. This maps internally to the DS3502's 7-bit wiper register (`0`-`127`).

`button:`
- `platform`: `ds3502`
- `ds3502_id` (Optional, ID): ID of the `ds3502` component to save, if you have multiple instances configured. Defaults to the only configured instance.
- All other options from [Button](https://esphome.io/components/button/index.html#config-button).

Pressing this button writes the potentiometer's current wiper position into the DS3502's EEPROM / non-volatile memory, so it survives a power cycle. This `button` is entirely optional: if `restore_value` is `false`, the wiper position is only saved to EEPROM when you press the button; if `restore_value` is `true`, the position is already saved to EEPROM on every change, making the button useless.