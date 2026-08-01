# AC-SENSE (ESPHome external component)

Custom ESPHome component for the **AC-SENSE** board - a chain of shift-register
boards that detect AC presence on multiple channels and report each channel as
a `binary_sensor`.

The board chain is read over a simple 3-wire `CLOCK` / `GRAB` / `DATA`
protocol. To see the board itself visit [my Tindie listing](https://www.tindie.com/products/tuksinet/ac-sense-daisy-chain-ac-presence-module/)

## Installation

Reference this repository as an [external component](https://esphome.io/components/external_components.html)
in your ESPHome YAML:

```yaml
external_components:
  - source: github://Tuksi-Net/AC-Sense-ESPHome-Component
    components: [ac_sense]
```

Or, if you've cloned/vendored it locally:

```yaml
external_components:
  - source:
      type: local
      path: components
    components: [ac_sense]
```

## Configuration

```yaml
ac_sense:
  id: my_ac_sense
  pin_clock: GPIOXX     # GPIO for CLOCK
  pin_grab: GPIOXX      # GPIO for GRAB
  pin_data: GPIOXX      # GPIO for DATA input
  num_channels: 8       # number of modules in chain
  update_interval: 50ms # interval of query status of entire chain

binary_sensor:
  - platform: ac_sense
    ac_sense_id: my_ac_sense
    channel: 1
    name: "AC Unit 1"
  - platform: ac_sense
    ac_sense_id: my_ac_sense
    channel: 2
    name: "AC Unit 2"
  ..... add more as required .....
```

### `ac_sense` component

| Option               | Type                   | Default | Description                                                                |
|----------------------|------------------------|---------|----------------------------------------------------------------------------|
| `id`                 | ID                     | -       | Manually specify the ID used for referencing this component.               |
| `pin_clock`          | Pin schema (required)  | -       | Output pin driving the shift-register clock line.                          |
| `pin_grab`           | Pin schema (required)  | -       | Output pin driving the latch/grab line.                                    |
| `pin_data`           | Pin schema (required)  | -       | Input pin reading the shifted-out data line.                               |
| `num_channels`       | int, 1-64 (required)   | -       | Number of channels (shift-register bits) to read.                          |
| `clock_half_us`      | int                    | `10`    | Half-period of the clock pulse, in microseconds.                           |
| `grab_settle_us`     | int                    | `50`    | Settle time after asserting `GRAB` before shifting, in microseconds.       |
| `update_interval`    | time                   | `50ms`  | How often to poll all channels (standard `PollingComponent` option).       |

### `binary_sensor` platform (`ac_sense`)

| Option         | Type                             | Default   | Description                                                  |
|----------------|----------------------------------|-----------|--------------------------------------------------------------|
| `ac_sense_id`  | ID                               | -         | The `ac_sense` component to attach this sensor to.           |
| `channel`      | int, 1-`num_channels` (required) | -         | Which channel (1-based) this sensor reports.                 |
| `device_class` | string                           | `power`   | Standard `binary_sensor` device class.                       |

Any channel without a `binary_sensor` registered is still polled (a warning is
logged at startup) but its state is simply discarded.

## How it works

Every `update_interval`, the component:

1. Pulses `CLOCK` once with `GRAB` low and samples `DATA` for channel 1
   (live, pre-latch read of board 0).
2. Raises `GRAB` high and waits `grab_settle_us` for all flip-flops to latch.
3. Pulses `CLOCK` once per remaining channel, sampling `DATA` after each
   pulse.
4. Publishes state changes only (a log line is emitted per channel whenever
   AC presence changes).

`GRAB` is left high between updates and only dropped briefly at the start of
the next cycle.

## License

MIT licensed.
