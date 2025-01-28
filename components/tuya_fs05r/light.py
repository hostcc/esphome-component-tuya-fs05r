'''
ESPHome component for "Tuya" FS05R dimmer.

SPDX-License-Identifier: Apache-2.0
Copyright (c) 2025 Ilia Sotnikov
'''
from typing import Dict, Any

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_OUTPUT_ID,
    CONF_MIN_VALUE,
    CONF_MAX_VALUE,
)
from esphome.components import light
from esphome.components import tuya
from esphome.components import uart


DEPENDENCIES = ['uart']
# See the comment below re: 'tyua' component
AUTO_LOAD = ['tuya']
CODEOWNERS = ['@hostcc']
TUYA_FS05R_MAX_BRIGHTNESS_VALUE = 1000.0

tuya_fs05r_ns = cg.esphome_ns.namespace('tuya_fs05r')
TuyaFS05R = tuya_fs05r_ns.class_(
    'TuyaFS05R', light.LightOutput, uart.UARTDevice, cg.Component
)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(TuyaFS05R),
        cv.Optional(CONF_MIN_VALUE, default=0.0):
            cv.float_range(max=TUYA_FS05R_MAX_BRIGHTNESS_VALUE),
        cv.Optional(CONF_MAX_VALUE, default=TUYA_FS05R_MAX_BRIGHTNESS_VALUE):
            cv.float_range(max=TUYA_FS05R_MAX_BRIGHTNESS_VALUE),
    }
).extend(uart.UART_DEVICE_SCHEMA)


def validate_min_max_values(config: Dict[str, Any]):
    '''
    Validates the min and max values.
    '''
    if config[CONF_MIN_VALUE] >= config[CONF_MAX_VALUE]:
        raise cv.Invalid("The 'min_value' must be less than 'max_value'")


FINAL_VALIDATE_SCHEMA = validate_min_max_values

# The platform isn't needed, aside from ESPHome pulling in the component's code
# to be reused programmatically. That is because the dimmer's MCU doesn't
# support Tuya MCU protocol - the generated code shouldn't contain any
# references directly.
delattr(tuya, 'to_code')


async def to_code(config: Dict[str, Any]) -> None:
    '''
    Produces the code for the component.
    '''
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    cg.add(var.set_min_value(config[CONF_MIN_VALUE]))
    cg.add(var.set_max_value(config[CONF_MAX_VALUE]))
    await cg.register_component(var, config)
    await light.register_light(var, config)
    await uart.register_uart_device(var, config)
