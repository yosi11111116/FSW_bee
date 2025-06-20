# Command to start the test application
#  CFE_ES_START_APP_CC = 4
#
# typedef struct CFE_ES_StartAppCmd_Payload
# {
#      char Application[CFE_MISSION_MAX_API_LEN]; //CFE_MISSION_MAX_API_LEN = 20
#      char AppEntryPoint[CFE_MISSION_MAX_API_LEN];
#      char AppFileName[CFE_MISSION_MAX_PATH_LEN]; //CFE_MISSION_MAX_PATH_LEN = 64
#      CFE_ES_MemOffset_t StackSize; //uint32
#      CFE_ES_ExceptionAction_Enum_t ExceptionAction; //uint8
#      undocumented SPARE uint8
#      CFE_ES_TaskPriority_Atom_t Priority; //uint16
# } CFE_ES_StartAppCmd_Payload_t;

# get
./cmdUtil --pktid=0x1875 --cmdcode=30 --endian=LE

# set
# ./cmdUtil --pktid=0x1874 --cmdcode=10 --endian=LE --uint16=0x0001 --int16=1
