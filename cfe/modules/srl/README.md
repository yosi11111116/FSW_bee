# Core Module Serial (srl) user guide
- Author : Hyeok-jin Kweon
- Astrodynamics and Control Lab, Yonsei University

# Overview
- This is user developed core module, which is located at cFE layer
- This module provide serial communication API for various protocol
    - I2C
    - SPI
    - CAN
    - UART
    - GPIO
    - SOCAT (virtual device)

# How to Use
## Configuration setting
- Firstly, end user should configure the mission specific data interface of host(OBC)
- Only need to know is, **edit `Interface_config.json` and build cFS**
> Check `README.md` in `tools/serial-configtool` for specific description

## Using API
- If you correctly configure the data interface, the **initialization is automatically processed**
    - In Early Init function from cFE service execution
- End user just use some API function such as `CFE_SRL_ApiGetHandle()`,`CFE_SRL_ApiWrite()` or `CFE_SRL_ApiRead()`
- If user want serial communication, 
    1. then **get serial IO Handle pointer(`CFE_SRL_IO_Handle_t *`)** via Api function
    > The Parameter is **Handle Indexer**
    > The Indexer enum value is `CFE_SRL_{Interface Name}_HANDLE_INDEXER`
    2. **Use API read, write function** by using IO Handle

## Example
```C
/* Get Handle */
CFE_SRL_IO_Handle_t *Handle = CFE_SRL_ApiGetHandle(CFE_SRL_SPI_HANDLE_INDEXER);

/* Parameter structure */
CFE_SRL_IO_Param_t Params = {0,};

/* Parameter setting */
uint8_t TxBuf[2] = {0x00, 0x01};
uint8_t RxBuf[2] = {0,};
Params.TxData = TxBuf;
Params.TxSize = sizeof(TxBuf);
Params.RxData = RxBuf;
Params.RxSize = sizeof(RxBuf);
Params.Timeout = 10

/* Execute API */
CFE_SRL_ApiRead(Handle, &Params);
```