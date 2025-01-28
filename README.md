# ESPHome component for "Tuya" FS05R dimmer

The component adds support for FS05R dimmer, often branded as "Tuya" but sold
by many different vendors.

## Credits

The implementation is based on great community work, mostly [Reverse engineering of the unusual protocol of the Tuya FS-05R dimmer based on UART ](https://www.elektroda.com/rtvforum/topic4039890.html) and [Integrating the Tuya FS-05R WiFi Dimmer Switch with ESPHome](https://nils.schimmelmann.us/2025-01-11-integrating-tuya-fs-05r-wifi-dimmer-switch-esphome/).

Their work on determing the protocol specifics is so much appreciated!

### Configuration variables

* **min_value** (*Optional*, number): Minimal value for the brightness as sent to dimmer, defaults to 0
* **max_value** (*Optional*, number): Same but for maximum value - the dimmer allows up to 1000, defaults to 1000
* **uart_id** (*Optional*, [ID](https://esphome.io/guides/configuration-types#config-id)): Manually specified ID of the UART hub
* **output_id** (*Optional*, [ID](https://esphome.io/guides/configuration-types#config-id)): The ID to declare output with.
* All other options from [Light](https://esphome.io/components/light/) component.

## Example

```yaml
external_components:
  source: github://hostcc/esphome-component-tuya-fs05r

light:
  - platform: tuya_fs05r
    name: Dimmer
```
