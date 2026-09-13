import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ADDRESS, CONF_ID

CODEOWNERS = ["@christian-dascoli00"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True

ds3502_ns = cg.esphome_ns.namespace("ds3502")
DS3502Component = ds3502_ns.class_("DS3502Component", cg.Component, i2c.I2CDevice)

CONF_RESTORE_VALUE = "restore_value"
CONF_A0 = "a0"
CONF_A1 = "a1"


def validate_address_selection(config):
    has_a1 = CONF_A1 in config
    has_a0 = CONF_A0 in config

    if has_a1 != has_a0:
        raise cv.Invalid("'a1' e 'a0' must be specified together")

    if has_a1 and has_a0:
        # ────── Address ───────
        # A1=GND, A0=GND -> 0x28
        # A1=GND, A0=VCC -> 0x29
        # A1=VCC, A0=GND -> 0x2A
        # A1=VCC, A0=VCC -> 0x2B
        # ──────────────────────
        address = 0x28
        if config[CONF_A1]:
            address += 0x02
        if config[CONF_A0]:
            address += 0x01
        config[CONF_ADDRESS] = address

    return config


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DS3502Component),
            cv.Optional(CONF_A1): cv.boolean,
            cv.Optional(CONF_A0): cv.boolean,
            cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
        }
    )
    .extend(i2c.i2c_device_schema(0x28))
    .extend(cv.COMPONENT_SCHEMA),
    validate_address_selection,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_restore_value(config[CONF_RESTORE_VALUE]))

    if config[CONF_ID].is_manual:
        cg.add(var.set_name(str(config[CONF_ID])))