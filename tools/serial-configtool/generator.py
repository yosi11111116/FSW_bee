import sys
import json
from genfunction import *

with open('../../cfe/modules/srl/Interface_config.json') as fp:
    config = json.load(fp)
interfaces = config['interfaces']    
namearr = Get_general_srl_namearr(interfaces)

# Generate cfe_srl_mission_cfg.h
with open('../../cfe/modules/srl/config/default_cfe_srl_mission_cfg.h', 'w') as f:
    f.write("/* Auto-Generated file */\n")
    f.write("#ifndef CFE_SRL_MISSION_CFG_H\n")
    f.write("#define CFE_SRL_MISSION_CFG_H\n")
    f.write("\n")
    f.write('#include "cfe_srl_interface_cfg.h"\n\n')

    f.write("typedef enum {\n")
    for name in namearr:
        f.write(f"\tCFE_SRL_{name}_HANDLE_INDEXER,\n")
    f.write("} CFE_SRL_Handle_Indexer_t;\n\n")

    f.write("/* \# of General serial device */\n")
    f.write("#define CFE_SRL_GNRL_DEVICE_NUM\t\t")
    f.write(f"(CFE_SRL_{namearr[-1]}_HANDLE_INDEXER + 1)\n\n")
    f.write("/* \# of used gpio pin */\n")
    f.write(f"#define CFE_SRL_TOT_GPIO_NUM\t\t{Get_gpio_num(config['interfaces'])}\n\n")
    f.write("typedef enum {\n")
    for iface in interfaces:
        if iface['type'] == 'gpio':
            f.write(f"\tCFE_SRL_{iface['name'].upper()}_GPIO_INDEXER,\n")
    f.write("} CFE_SRL_GPIO_Indexer_t;\n\n")
    f.write("#endif /* CFE_SRL_MISSION_CFG_H */")




# Generate EarlyInit function
with open('../../cfe/modules/srl/fsw/src/cfe_srl_init.c', 'w') as f:
    f.write("/* Auto-Generated file. Never change this code! */\n")
    f.write("/**\n * Required header files\n*/\n")
    f.write('#include "cfe_srl_module_all.h"\n\n')
    f.write("/**\n * Global data\n*/\n")
    f.write('CFE_SRL_IO_Handle_t *Handles[CFE_SRL_GNRL_DEVICE_NUM];\n')
    f.write("/**************************************************\n * Index of Each device\n")
    for i, name in enumerate(namearr):
        f.write(f" * {i} : {name} Handle\n")
    f.write(" **************************************************/\n\n")
    f.write("CFE_SRL_GPIO_Handle_t *GPIO[CFE_SRL_TOT_GPIO_NUM];\n\n\n")

    f.write("/************************************************************************\n")
    f.write(" * Early Initialization function executed at cFE ES\n")
    f.write(" * Append object to `cfe_es_objtab.c`\n")
    f.write(" * Declaration is located at\n")
    f.write(" * `cfe/modules/core_private/fsw/inc/cfe_srl_core_internal.h`\n")
    f.write(" ************************************************************************/\n")
    f.write('int32 CFE_SRL_EarlyInit(void) {\n')
    f.write('\tint32 Status;\n\n')
    f.write('\tStatus = CFE_SRL_PriorInit();\n')
    f.write('\tif(Status != CFE_SUCCESS) return Status;\n')
    f.write('\tCFE_ES_WriteToSysLog("%s: Prior Initialized.", __func__);\n')
    f.write("\t/**************************************************\n")
    f.write("\t * Serial Comm. Init\n \t * Only `ready == true` interface is initialized\n")
    f.write("\t **************************************************/\n")

    for iface in interfaces:
        if not iface['ready']:
            continue
        if iface['type']=="i2c":
            Write_i2c_handle_init(f, iface)
        elif iface['type']=='spi':
            Write_spi_handle_init(f, iface)
        elif iface['type'] in ['uart', 'rs232', 'rs422', 'rs485']:
            Write_uart_handle_init(f, iface)
        elif iface['type'] == 'can':
            Write_can_handle_init(f, iface)
        elif iface['type'] == 'socat':
            Write_socat_handle_init(f, iface)

        if iface['type'] == 'gpio':
            Write_gpio_init(f, iface)
            f.write("\tif (Status != CFE_SUCCESS) {\n")
            f.write(f"\t\tCFE_ES_WriteToSysLog(\"%s: GPIO {iface['name']} Initialization failed! RC=%d\\n\", __func__, Status);\n")
            f.write(f"\t\treturn CFE_SRL_GPIO_INIT_ERR;\n")
            f.write("\t}\n\n")
        else:
            f.write("\tif (Status != CFE_SUCCESS) {\n")
            f.write(f"\t\tCFE_ES_WriteToSysLog(\"%s: {iface['name']} Initialization failed! RC=%d\\n\", __func__, Status);\n")
            f.write(f"\t\treturn CFE_SRL_{iface['name'].upper()}_INIT_ERR;\n")
            f.write("\t}\n\n")

    f.write("return CFE_SUCCESS;\n")
    f.write("}\n")

print("Code generation Successfully done.")

