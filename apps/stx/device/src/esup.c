#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <esup.h>
#include <rs485.h>
#include <crc32.h>
#include <byteswap.h>
#include "cfe_srl.h"
#include "default_cfe_srl_mission_cfg.h"
#include "default_cfe_srl_extern_typedefs.h"

// CFE_SRL_RS422_Handle_t *RS422;
uint32_t Timeout = 100;

uint16_t modid_forscan = 0;

// ESUP Encoder: ESUP packet을 만드는 과정
static uint16_t ESUP_Encoder(uint16_t comm_stt, uint16_t comm, uint16_t type, void * data, uint16_t length, uint16_t padlen, ESUP_Packet_t * packet)
{   
    if(!packet)
        return 0;

    packet->header.header = ESUP_HEADER;                              // Header
    packet->header.mod_id = modid_forscan;                            // Module ID
    packet->header.length = length;                                   // Data Length
    packet->header.com_stt = comm_stt;                                // Command Status
    packet->header.command = comm;                                    // Command
    packet->header.type = type;                                       // Type
    memcpy(packet->DCP, data, length);                                // 인자로 받은 data를 ESUP packet의 데이터 영역인 DCP로 data length만큼 복사
    uint32_t c32 = crc32(0, packet, sizeof(ESUP_Header_t) + length);  // CRC32 계산
    memcpy(packet->DCP + length, &c32, sizeof(c32));                  // CRC32 bit 추가
    memset(packet->DCP + length + sizeof(c32), 0, sizeof(padlen));    // zero padding
    // ESUP packet 완성

    return length + padlen + sizeof(ESUP_Header_t);                   // ESUP packet의 전체 길이 return
    
}

// ESUP Decoder: XTX로부터 받은 ESUP packet 해석 
/*
   RS485_READ의 결과가 ESUP_Decoder의 인자로 들어감
   data: RS485_READ를 통해 읽은 데이터가 저장되어 있는 readbuf를 가리킴
   length: RS485_READ를 통해 읽은 데이터 바이트 수
   packet: 초기화 안 된 packet 구조체를 가리킴
*/
static uint16_t ESUP_Decoder(uint8_t * data, uint16_t length, ESUP_Packet_t * packet)
{
    if(!data)
        return 0;

    if(!packet)
    {
        packet = (ESUP_Packet_t *)malloc(length);  // 읽은 데이터 길이만큼 packet에 메모리 할당
    }

    memcpy(packet, data, length);  // 읽은 데이터를 packet에 복사

    char headerbuf[5];                          // ESUP packet의 header 출력 공간
    memset(headerbuf, 0, sizeof(headerbuf));    // 0으로 초기화
    memcpy(headerbuf, data, sizeof(uint32_t));  // 읽은 데이터의 초기 4바이트를 headerbuf에 복사
    headerbuf[4] = 0;                           // 문자열의 마지막은 null

    // ESUP packet 정보 출력
    printf("----ESUP Decoder----\n");
    printf("Header         : %s\n", headerbuf);                  // Header: ESUP in ASCII(0x50555345 in HEX)
    printf("Module ID      : %hx\n", packet->header.mod_id);     // Module ID
    printf("Data Length    : %04hx\n", packet->header.length);     // Data Length
    printf("Command Status : %04hx\n", packet->header.com_stt);   // Command Status
    printf("Command        : %04hx\n", packet->header.command);  // Command
    printf("Type           : %04hx\n", packet->header.type);     // Type
    printf("Data           : \n");                               // Data
    for(int i = 0; i < packet->header.length; i++)
    {
        printf("%02hhx\t", packet->DCP[i]);  // 1 바이트씩 unsigned char 형태로 출력
        if(i % 10 == 9)
            printf("\n");
    }

    /*
    // DIR Command 후 GetResult Command의 데이터 영역 해독 (파일 개수, 파일 이름, 파일 크기)  
    if (packet->header.com_stt == ESUP_INSIG && packet->header.command == FILESYS_CC_DIR && packet->header.type == FILESYS_TP_NA && packet->DCP[0] == 0)
    */
   
    printf("\n");

    return length;  // Data length 반환
}

// ESUP
/*
   1. ESUP packet 생성 (ESUP_Encoder)
   2. 해당 packet RS485 통신으로 XTX에 write (RS485_WRITE)
   3. XTX의 응답을 RS485 통신으로 read (RS485_READ)
   4. Read한 내용을 ESUP packet 형식에 맞게 출력 (ESUP_Decoder)
*/
uint16_t ESUP(uint16_t comm_stt, uint16_t comm, uint16_t type, void * txdata, uint16_t txlength, void * rxdata, uint16_t rxlength)
{   

    CFE_SRL_IO_Handle_t *Handle = CFE_SRL_ApiGetHandle(CFE_SRL_UART_HANDLE_INDEXER);

    ESUP_Packet_t * packet = NULL;  // XTX에 write할 ESUP packet을 가리키는 포인터
    ESUP_Packet_t * reply = NULL;   // XTX로부터 read한 데이터를 저장할 ESUP packet을 가리키는 포인터
    uint16_t retu16 = 0;
    CFE_SRL_IO_Param_t Params;

    // int CFE_SRL_OK = 0;             //cfe/modeule/srl에서 참고
    int retd32 = 0;

    char readbuf[ESUP_MAX_LEN];           // RS485_READ 결과 저장할 readbuf
    memset(readbuf, 0, sizeof(readbuf));  // readbuf 0으로 초기화

    if(!txdata)        // 데이터가 없다면
        txlength = 0;  // data length = 0

    uint16_t lencal = sizeof(ESUP_Header_t);  // Add Header Length
    lencal += txlength + 4;                     // Add Datafield length & crc32 length
    int padlen = 16 - lencal % 16;            // Add Padding length
    lencal += padlen;

    printf("Write Bytes : %d\n", lencal);      // XTX에 write하는 ESUP packet의 길이 출력
    packet = (ESUP_Packet_t *)malloc(lencal);  // 해당 길이만큼 메모리 할당
    
    modid_forscan = 0x2008;  // Module ID
    
    // XTX에 write할 ESUP packet 생성
    retu16 = ESUP_Encoder(comm_stt, comm, type, txdata, txlength, padlen, packet);
    if(!retu16)  // retu16: 생성한 ESUP packet의 전체 길이
    {
        printf("ESUP Write Failed!\n");
        return retu16;
    }
    
    // // 생성한 ESUP packet을 RS422 통신으로 XTX에 write
    // retd32 = CFE_SRL_ApiWrite(RS422, packet, lencal, 0);
    // if(retd32 != CFE_SRL_OK)  // retd32: write한 데이터 길이
    // {
    //     printf("RS485 Write Failed!\n");
    //     return retu16;
    // }
    // printf("\nESUP_write success via RS485.\n");
    // printf("--------------------\n");
    // usleep(30000);
     
    
    // XTX의 응답을 RS485 통신으로 read
    // retd32 = RS485_READ(readbuf, sizeof(readbuf));
    Params.TxData = packet;
    Params.TxSize = lencal;
    Params.RxData = rxdata;
    Params.RxSize = rxlength;
    Params.Timeout = Timeout;

    retd32 = CFE_SRL_ApiRead(Handle, &Params);
    if(retd32 <= 0)  // retd32: read한 데이터 길이
    {
        printf("RS485 has no reply.\n");
        printf("%d\n", retd32); // 또는 필요한 형식에 따라
        return retu16;
    }
    
    // Read한 내용을 ESUP packet 형식에 맞게 출력
    retu16 = ESUP_Decoder((uint8_t*)rxdata, (uint16_t)retd32, reply);
    if(!retu16)  // retu16: 받은 ESUP packet에서 데이터 영역의 길이
    {
        printf("ESUP Read Failed!\n");
        return retu16;
    }
    printf("ESUP_read success via RS485.\n");

    free(packet);
    
    return retu16;
}

uint16_t ESUP_modscan(uint16_t comm_stt, uint16_t comm, uint16_t type, void * data, uint16_t length)
{
    ESUP_Packet_t * packet = NULL;
    // ESUP_Packet_t * reply = NULL;
    uint16_t retu16 = 0;
    int retd32 = 0;

    char readbuf[ESUP_MAX_LEN];
    memset(readbuf, 0, sizeof(readbuf));

    if(!data)
        length = 0;

    //Add Header Length
    uint16_t lencal = sizeof(ESUP_Header_t);
    //Add Datafield length & crc32 length
    lencal += length + 4;
    //Add Padding length
    int padlen = 16 - lencal % 16;
    lencal += padlen;

    packet = (ESUP_Packet_t *)malloc(lencal);

    for(modid_forscan = 0; modid_forscan < 65535; modid_forscan++)
    {
        if(modid_forscan % (65535/1000) == 0)
            printf("Scanning : %hupercent\n", 5 * modid_forscan / (65535/20));
        retu16 = ESUP_Encoder(comm_stt, comm, type, data, length, padlen, packet);
        if(!retu16)
        {
            printf("ESUP Write Failed!\n");
            return retu16;
        }
        
        retd32 = RS485_WRITE(packet, lencal);
        if(retd32 <= 0)
        {
            printf("RS485 Write Failed!\n");
            return retu16;
        }
        // printf("ESUP_write success via RS485.\n");
        // usleep(1000);
        retd32 = RS485_READ(readbuf, sizeof(readbuf));
        if(retd32 <= 0)
        {
            // printf("RS485 has no reply.\n");
            // return retu16;
        }
        else
        {
            printf("Module ID : %hu, length : %d\n", modid_forscan, retd32);
            break;
        }
        usleep(10000); 
    }
    
    return retu16;
}