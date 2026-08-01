import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_CHANNEL, DEVICE_CLASS_POWER

from . import CONF_AC_SENSE_ID, AcSenseComponent, MAX_CHANNELS

DEPENDENCIES = ["ac_sense"]

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_POWER,
).extend(
    {
        cv.GenerateID(CONF_AC_SENSE_ID): cv.use_id(AcSenseComponent),
        cv.Required(CONF_CHANNEL): cv.int_range(min=1, max=MAX_CHANNELS),
    }
)


async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    hub = await cg.get_variable(config[CONF_AC_SENSE_ID])
    cg.add(hub.register_channel(config[CONF_CHANNEL] - 1, var))
