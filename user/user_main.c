#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "user_interface.h"

#include "espconn.h"

#include "user_interface.h"

#include "driver/uart.h"

#include "../z80emu/z80emu.h"

unsigned char RAM[RAMSIZE];
const unsigned char ROM81[8192]={
#include "../open81.h"
};

#include "../zxgal.h"

void ICACHE_FLASH_ATTR
load_prog(void)
{
  int i;
  for(i=0;i<ZXPROG_LG;i++) Z80_WRITE_BYTE(0x4009+i,zxprog[i]);
  os_printf("Prog. loaded\n");
}

unsigned char kbd_table[8];		// 0->FE, 1=>FD, .. 7=>7F
volatile unsigned int slow=0;

LOCAL struct espconn ptrespconn;
LOCAL Z80_STATE state;

#define user_procTaskPrio 0
#define user_procTaskQueueLen 1
os_event_t user_procTaskQueue[user_procTaskQueueLen];

static void ICACHE_FLASH_ATTR
loop(os_event_t *events)
{
     if (slow) { Z80Emulate(&state, 40000);
                 os_delay_us(15000); } 
          else
                 Z80Emulate(&state, 100000);

     system_os_post(user_procTaskPrio,0,0);
}

LOCAL void ICACHE_FLASH_ATTR
user_devicefind_recv(void *arg, char *pusrdata, unsigned short length)
{
     int i;

     if (length>=2) {
       if ((pusrdata[0]>='0')&&(pusrdata[0]<='7'))
         kbd_table[pusrdata[0]-'0']=pusrdata[1];
         // os_printf("Got : %c %02x\n",pusrdata[0],pusrdata[1]);
         // for(i=0;i<8;i++)
         //  os_printf("%02x ",kbd_table[i]);
         //os_printf("\n");
      }
     
     if (length==1) {
       if (pusrdata[0]=='F') slow=0;
       if (pusrdata[0]=='S') slow=1;
     }

     // system_print_meminfo();
     // for(i=0;i<128;i++) { os_printf("%02x ",RAM[i]);
     // if ((i&15)==15) os_printf("\n"); }

     espconn_send(&ptrespconn, &RAM[(RAM[0x0c]+256*RAM[0x0d])&0x1FFF], 792);
}

void user_rf_pre_init(void)
{
}

void user_init(void)
{
    int i;

    char ssid[32] = "OpenWrt";
    char password[64] = "gtwireless";
    struct station_config stationConf;

    uart_init(115200,115200);

    os_printf("SDK version:%s\n", system_get_sdk_version());
    wifi_set_opmode(STATION_MODE);

    stationConf.bssid_set = 0; //need not check MAC address of AP
   
    os_memcpy(&stationConf.ssid, ssid, 32);
    os_memcpy(&stationConf.password, password, 64);
    wifi_station_set_config(&stationConf);

    system_print_meminfo();

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,FUNC_GPIO2);
    GPIO_OUTPUT_SET(2, 1);

    for(i=0;i<8;i++) kbd_table[i]=0xff;

    os_printf("Z80 Reset\n");
    Z80Reset(&state);

    ptrespconn.type = ESPCONN_UDP;
    ptrespconn.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
    ptrespconn.proto.udp->local_port = 1025;
    espconn_regist_recvcb(&ptrespconn, user_devicefind_recv);
    espconn_create(&ptrespconn);

    system_os_task(loop, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
    system_os_post(user_procTaskPrio,0,0);

    os_printf("End\n");
}
