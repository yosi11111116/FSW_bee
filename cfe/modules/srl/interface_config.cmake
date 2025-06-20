##################################################################
#
# cFE Serial Comm. (SRL) module's Data configuration file
# End user only revise this recipe to configure CubaSat data interface
#
# Author : HyeokJin Kweon
##################################################################

###################################################################
# Unit Test stub flag
# If true, the 'hardware open & R/W' function is substituted with a stub function
# i.e. `FASLE` is real
set(ENABLE_UT       FALSE)

# Define interface names
# If data interface is changed, revise lower script
set (DATA_INTERFACES I2C0 I2C1 I2C2 UART RS422 CAN GPIO CSP SOCAT)

# Define interface device file
set (I2C0_DEV "/dev/i2c-0")
set (I2C1_DEV "/dev/i2c-1")
set (I2C2_DEV "/dev/i2c-2")
set (UART_DEV "/dev/ttyS0")
set (RS422_DEV "/dev/ttyS1")
set (CAN_DEV "can0")
set (SOCAT_DEV "/dev/pts/4")

# Each data interface readiness flags
# If false, corresponding interface initialization will be skipped.
set(I2C0_READY      FALSE)
set(I2C1_READY      FALSE)
set(I2C2_READY      FALSE)
set(UART_READY      FALSE)
set(RS422_READY     FALSE)
set(CAN_READY       FALSE)
set(GPIO_READY      FALSE)
set(CSP_READY       FALSE)

# Imaginary serial stream `socat` flag
set(SOCAT_READY       TRUE)
###################################################################