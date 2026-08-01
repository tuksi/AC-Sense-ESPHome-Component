from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NUM_CHANNELS

CODEOWNERS = ["@janezdolinar"]

ac_sense_ns = cg.esphome_ns.namespace("ac_sense")
AcSenseComponent = ac_sense_ns.class_("AcSenseComponent", cg.PollingComponent)

CONF_AC_SENSE_ID = "ac_sense_id"
CONF_PIN_CLOCK = "pin_clock"
CONF_PIN_GRAB = "pin_grab"
CONF_PIN_DATA = "pin_data"
CONF_CLOCK_HALF_US = "clock_half_us"
CONF_GRAB_SETTLE_US = "grab_settle_us"

MAX_CHANNELS = 64

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(AcSenseComponent),
        cv.Required(CONF_PIN_CLOCK): pins.gpio_output_pin_schema,
        cv.Required(CONF_PIN_GRAB): pins.gpio_output_pin_schema,
        cv.Required(CONF_PIN_DATA): pins.gpio_input_pin_schema,
        cv.Required(CONF_NUM_CHANNELS): cv.int_range(min=1, max=MAX_CHANNELS),
        cv.Optional(CONF_CLOCK_HALF_US, default=10): cv.int_range(min=1),
        cv.Optional(CONF_GRAB_SETTLE_US, default=50): cv.int_range(min=1),
    }
).extend(cv.polling_component_schema("50ms"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    clock_pin = await cg.gpio_pin_expression(config[CONF_PIN_CLOCK])
    cg.add(var.set_pin_clock(clock_pin))
    grab_pin = await cg.gpio_pin_expression(config[CONF_PIN_GRAB])
    cg.add(var.set_pin_grab(grab_pin))
    data_pin = await cg.gpio_pin_expression(config[CONF_PIN_DATA])
    cg.add(var.set_pin_data(data_pin))

    cg.add(var.set_num_channels(config[CONF_NUM_CHANNELS]))
    cg.add(var.set_clock_half_us(config[CONF_CLOCK_HALF_US]))
    cg.add(var.set_grab_settle_us(config[CONF_GRAB_SETTLE_US]))
