# About **Serial Config tool**
- Author: Kweon Hyeok-jin

# Overview
- This tool is user-friendly configuration tool for mission specific serial communication interfaces
- This tool automatically generated some source code
    - `default_cfe_srl_mission_cfg.h` in `/cfe/modules/srl/config/`
    - `cfe_srl_init.c` in `/cfe/modules/srl/fsw/src/`
- This tool read `Interface_config.json` which determine the serial interfaces configuration

# How to use
- End user just revise `Interface_config.json` only
- After reivision, manually execute `generator.py` in `tools/serial-configtool/`
- Or, just run the build script from `make prep`

# Write `.json` file
- Every interfaces are single element of **one list which is the value of key "interfaces"**
- So, end user just revise each element like
```json
{
    "type": "i2c",
    "ready": false,
    "name": "I2C0",
    "DevName": "/dev/i2c-0",
    "MutexID": 0
}
```
## Common element
- Every interfaces (i.e. protocol) has common keys
    1. type : protocol type. MUST type in **lower case**
    2. ready : readiness flag. MUST type in **bool**
    3. name : Nickname of each interface. MUST type **device file name in upper case**
    4. DevName : Device file path. Must type exact same file path
    5. MutexID : **Deprecated**
- Some interfaces has unique keys
### UART series
1. baudrate
### GPIO
1. line : line number of each gpiochip
2. default : default value of gpio. Must type `1` or `0`
3. direction (NOT supported now): default gpio direction Must type `in` or `out`