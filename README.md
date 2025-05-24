# Konsool Meshtasti UI Port

## Usage

- Use the buttons with *shapes* to navigate between the different pages in the UI
- Use the arrow up and down keys to navigate

## Building

```
make prepare
make build
```

## Flashing
```
make flash
```

## Features

### Implemented

- Full display support
- Full keyboard support
- Connecting to the C6 over serial
- Battery level
- Screen sleep
- LittleFS persistent storage

### Not Implemented Yet

- P4 sleep
- SD card support
- Maps

---


Based on the [device-ui](https://github.com/meshtastic/device-ui/tree/master) library from meshtastic
