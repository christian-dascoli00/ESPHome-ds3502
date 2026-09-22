import esphome.config_validation as cv
from esphome.components import button
import esphome.codegen as cg

from .. import DS3502Component, ds3502_ns

DEPENDENCIES = ["ds3502"]

CONF_DS3502_ID = "ds3502_id"
DS3502Button = ds3502_ns.class_("DS3502Button", button.Button)

CONFIG_SCHEMA = button.button_schema(DS3502Button).extend(
    {
        cv.GenerateID(CONF_DS3502_ID): cv.use_id(DS3502Component),
    }
)


async def to_code(config):
    var = await button.new_button(config)

    parent = await cg.get_variable(config[CONF_DS3502_ID])
    cg.add(var.set_parent(parent))