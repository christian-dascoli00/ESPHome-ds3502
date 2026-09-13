import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID

from . import DS3502Component, ds3502_ns

DEPENDENCIES = ["ds3502"]

CONF_DS3502_ID = "ds3502_id"
DS3502Output = ds3502_ns.class_("DS3502Output", output.FloatOutput)

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(DS3502Output),
        cv.GenerateID(CONF_DS3502_ID): cv.use_id(DS3502Component),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await output.register_output(var, config)

    parent = await cg.get_variable(config[CONF_DS3502_ID])
    cg.add(var.set_parent(parent))