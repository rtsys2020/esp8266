/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/1/23, v1.0 create this file.
*******************************************************************************/

#include "user_config.h"

uint8_t buffer[1300];

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static void wifi_connect(os_event_t *events);

#define UDP_CONNECTION 1
#define GPIO_BLINK_PIN BIT2
at_linkConType linkTemp;
static void tcpclient_connect_cb(void *arg);
void tcpclient_recv(void *arg, char *pdata, unsigned short len);
static void tcpclient_discon_cb(void *arg);
static void tcpclient_recon_cb(void *arg, sint8 errType);
static volatile os_timer_t send_timer;
static void tcpclient_sent_cb(void *arg);
void udpclient_recv(void *arg, char *pdata, unsigned short len);
//void delay_ms(uint32_t dly)
//{
//	for(int i=0;i<dly;i++)
//		os_delay_us(1000);
//}

/**
  * @brief  Tcp client disconnect success callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */
static void tcpclient_discon_cb(void *arg)
{
	struct espconn *pespconn = (struct espconn *)arg;
	at_linkConType *linkTemp = (at_linkConType *)pespconn->reverse;
	uint8_t idTemp;
	char temp[16];
	os_printf("dicconect\r\n");
	if(pespconn == NULL)
	{
	return;
	}

	if(pespconn->proto.tcp != NULL)
	{
	os_free(pespconn->proto.tcp);
	}
	os_free(pespconn);

	linkTemp->linkEn = FALSE;
//  os_printf("disconnect\r\n");

//  os_printf("con EN? %d\r\n", pLink[0].linkEn);

	if(linkTemp->pCon->type == ESPCONN_TCP)
	{

	  espconn_disconnect(linkTemp->pCon);
	}
	else
	{
	  espconn_delete(linkTemp->pCon);
	  os_free(linkTemp->pCon->proto.udp);
	  os_free(linkTemp->pCon);
	}
}

/**
  * @brief  Client received callback function.
  * @param  arg: contain the ip link information
  * @param  pdata: received data
  * @param  len: the lenght of received data
  * @retval None
  */
void tcpclient_recv(void *arg, char *pdata, unsigned short len)
{
  struct espconn *pespconn = (struct espconn *)arg;
  at_linkConType *linkTemp = (at_linkConType *)pespconn->reverse;
  char temp[32];

  os_printf("recv\r\n");

}

/**
  * @brief  Tcp client connect success callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */
static void tcpclient_connect_cb(void *arg)
{
  struct espconn *pespconn = (struct espconn *)arg;
  at_linkConType *linkTemp = (at_linkConType *)pespconn->reverse;
  char temp[16];

  os_printf("tcp client connect\r\n");
 // os_printf("pespconn %p\r\n", pespconn);

  linkTemp->linkEn = TRUE;
  linkTemp->teType = teClient;
  linkTemp->repeaTime = 0;
  espconn_regist_disconcb(pespconn, tcpclient_discon_cb);
  espconn_regist_recvcb(pespconn, tcpclient_recv);////////
  espconn_regist_sentcb(pespconn, tcpclient_sent_cb);///////
  uint8_t data[] = "salam\r\n";
  espconn_sent(pespconn,data , strlen(data));
  //espconn_sent(pespconn,data , strlen(data));
}

/**
  * @brief  Tcp client connect repeat callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */
static void tcpclient_recon_cb(void *arg, sint8 errType)
{
  struct espconn *pespconn = (struct espconn *)arg;
  at_linkConType *linkTemp = (at_linkConType *)pespconn->reverse;
  struct ip_info ipconfig;
  os_timer_t sta_timer;
  char temp[16];

  //os_printf("at_tcpclient_recon_cb %d\r\n", linkTemp->repeaTime);


    linkTemp->repeaTime++;
    if(linkTemp->repeaTime >= 100)
    {
      os_printf("repeat over %d\r\n", linkTemp->repeaTime);
//      specialAtState = TRUE;
//      at_state = at_statIdle;
      linkTemp->repeaTime = 0;
//      os_printf("err %d\r\n", errType);

      if(pespconn->proto.tcp != NULL)
      {
        os_free(pespconn->proto.tcp);
      }
      os_free(pespconn);
      linkTemp->linkEn = false;
      os_printf("disconnect\r\n");
      //  os_printf("con EN? %d\r\n", pLink[0].linkEn);

      return;
    }

    os_printf("link repeat %d\r\n", linkTemp->repeaTime);
    pespconn->proto.tcp->local_port = espconn_port();
    espconn_connect(pespconn);

}

/**
  * @brief  Client send over callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */

static void tcpclient_sent_cb(void *arg)
{
	static uint8_t count=0;
	struct espconn *pespconn = (struct espconn *)arg;
//	os_free(at_dataLine);
	//os_timer_arm(&send_timer, 10, 0);
	os_printf("send_cb  num:%d\r\n",count);
  //system_os_post(user_procTaskPrio, UDP_CONNECTION, (os_param_t)pespconn);
}

/**
  * @brief  Client received callback function.
  * @param  arg: contain the ip link information
  * @param  pdata: received data
  * @param  len: the lenght of received data
  * @retval None
  */
void udpclient_recv(void *arg, char *pdata, unsigned short len)
{
  struct espconn *pespconn = (struct espconn *)arg;
  at_linkConType *linkTemp = (at_linkConType *)pespconn->reverse;
  char temp[32];

  os_printf("recv\r\n");
//  if(linkTemp->changType == 0) //if when sending, receive data???
//  {
//    os_memcpy(pespconn->proto.udp->remote_ip, linkTemp->remoteIp, 4);
//    pespconn->proto.udp->remote_port = linkTemp->remotePort;
//  }
//  else if(linkTemp->changType == 1)
//  {
//    os_memcpy(linkTemp->remoteIp, pespconn->proto.udp->remote_ip, 4);
//    linkTemp->remotePort = pespconn->proto.udp->remote_port;
//    linkTemp->changType = 0;
//  }
  int i=0;
  for(i=0;i<len;i++)
  {
	  os_printf("%c",pdata[i]);
  }

}
/*******************************************************
 * *
 * *
 * *
 * *
 * @brief
 ******************************************************/
void udp_timerfunc(void *arg)
{

  if(wifi_station_get_connect_status() != STATION_GOT_IP)
  {
	os_printf("no ip\r\n");
	//system_set_os_print(0);
	//os_timer_arm(&send_timer, 500,0 );
	return;
  }
  os_printf("creat connection\r\n");
  enum espconn_type linkType = ESPCONN_UDP;
  struct espconn pcon;
  uint32_t ip = 0;
  char ipTemp[128];
  int32_t remotePort=1026,localPort=1365;
  uint8_t linkID=1;
  uint8_t changType;
  //at_linkConType linkTemp;
  linkTemp.pCon = (struct espconn *)os_zalloc(sizeof(struct espconn));
  linkTemp.pCon->type = linkType;
  linkTemp.pCon->state = ESPCONN_NONE;
  linkTemp.pCon->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
  linkTemp.pCon->proto.udp->local_port = espconn_port();
  linkTemp.pCon->proto.udp->remote_port = 5000;
  linkTemp.remotePort = linkTemp.pCon->proto.udp->remote_port;
  linkTemp.linkId = linkID;
  linkTemp.linkEn = TRUE;
  linkTemp.teType = teClient;
  linkTemp.changType = 0;
  linkTemp.pCon->reverse = &linkTemp;
  espconn_regist_recvcb(linkTemp.pCon, udpclient_recv);
  espconn_regist_sentcb(linkTemp.pCon, tcpclient_sent_cb);
  ip = ipaddr_addr("192.168.1.9");
  os_memcpy(linkTemp.pCon->proto.udp->remote_ip, &ip, 4);
  os_memcpy(linkTemp.remoteIp, &ip, 4);
  espconn_create(linkTemp.pCon);
//  uint8_t data[] = "salam\r\n";
  espconn_sent(linkTemp.pCon,buffer , 1024);
  //system_os_post(user_procTaskPrio, UDP_CONNECTION, (os_param_t)linkTemp.pCon );
  os_free(linkTemp.pCon);
}


void tcp_timerfunc(void *arg)
{
	//Do blinky stuff
	if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & GPIO_BLINK_PIN)
	{
		//Set GPIO2 to LOW
		gpio_output_set(0, GPIO_BLINK_PIN, GPIO_BLINK_PIN, 0);
	}
	else
	{
		//Set GPIO2 to HIGH
		gpio_output_set(GPIO_BLINK_PIN, 0, GPIO_BLINK_PIN, 0);
	}
  if(wifi_station_get_connect_status() != STATION_GOT_IP)
  {
	os_printf("no ip\r\n");
	return;
  }
  enum espconn_type linkType = ESPCONN_TCP;
  struct espconn pcon;
  uint32_t ip = 0;
  char ipTemp[128];
  int32_t remotePort=50000,localPort=1365;
  uint8_t linkID=1;
  uint8_t changType;

  linkTemp.pCon = (struct espconn *)os_zalloc(sizeof(struct espconn));
  linkTemp.pCon->type = linkType;
  linkTemp.pCon->state = ESPCONN_NONE;
  linkTemp.pCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
  linkTemp.pCon->proto.tcp->local_port = espconn_port();
  linkTemp.pCon->proto.tcp->remote_port = 5000;
  linkTemp.remotePort = linkTemp.pCon->proto.tcp->remote_port;
  linkTemp.linkId = linkID;
  linkTemp.linkEn = TRUE;
  linkTemp.teType = teClient;
  linkTemp.repeaTime = 0;
  linkTemp.changType = 1;
  linkTemp.pCon->reverse = &linkTemp;
  espconn_regist_connectcb(linkTemp.pCon, tcpclient_connect_cb);
  espconn_regist_reconcb(linkTemp.pCon, tcpclient_recon_cb);
  ip = ipaddr_addr("192.168.1.9");
  os_memcpy(linkTemp.pCon->proto.tcp->remote_ip, &ip, 4);
  os_memcpy(linkTemp.remoteIp, &ip, 4);
  espconn_create(linkTemp.pCon);
  espconn_connect(linkTemp.pCon);
  os_printf("connection creat\r\n");



}
/////////////////////////////////////////////////////


//Main code function
/*******************************************************
 * *
 * *
 * *
 * *
 * @brief
 ******************************************************/
static void
wifi_connect(os_event_t *events)
{
	uint8_t constat;
	struct espconn *pcon;
	pcon = (struct espconn *)events->par;
	os_printf("task send run\r\n");
	//uint8_t data[] = "salam\r\n";
	espconn_sent(pcon,buffer , 1024);
}
///////////////////////////////////////////////////////
/*******************************************************
 * *
 * *
 * *
 * *
 * @brief
 ******************************************************/
void user_init(void)
{
	static uint8_t trycon=0;
	struct station_config stationConf;
   // at_cmd_array_regist(&at_custom_cmd[0], 1);
	gpio_init();
    //Set GPIO2 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    //Set GPIO2 low
    gpio_output_set(0, GPIO_BLINK_PIN, GPIO_BLINK_PIN, 0);

	struct ip_info info;
	wifi_station_dhcpc_stop();
	wifi_softap_dhcps_stop();
	wifi_station_set_auto_connect(FALSE);

	wifi_set_opmode(0x03);
	info.ip.addr = ipaddr_addr("192.168.1.150");
	info.netmask.addr = ipaddr_addr("255.255.255.0");
	info.gw.addr = ipaddr_addr("192.168.1.1");
	wifi_set_ip_info(STATION_IF, &info);


	os_memset(&stationConf, 0, sizeof(struct station_config));
	os_memcpy(&stationConf.ssid, "default", 32);
	os_memcpy(&stationConf.password, "pass@word123", 64);
	//if(trycon == 0)
		wifi_station_set_config(&stationConf);
	trycon++;
	wifi_station_set_auto_connect(TRUE);
//
//    {
//        struct dhcps_lease pdhcp_info ;
//        os_memset(&pdhcp_info,0,sizeof(struct dhcps_lease));
//        pdhcp_info.start_ip = ipaddr_addr("192.168.1.100");
//        pdhcp_info.end_ip = ipaddr_addr("192.168.1.150");
//        wifi_softap_set_dhcps_lease(&pdhcp_info);
//        struct softap_config configAP;
//        os_memcpy(&configAP.ssid, "des8266", 32);
//        os_memcpy(&configAP.password, "123", 64);
//        configAP.max_connection = 2;
//        configAP.ssid_hidden = 0;
//        configAP.ssid_len = 7;
//        configAP.authmode = AUTH_OPEN;
//        wifi_softap_set_config(&configAP);
//        wifi_softap_dhcps_start();
//
//
//    }



     // wifi_station_get_config(&stationConf);
    //os_printf("is bssid set:%s",stationConf.ssid);
      //Start os task
    system_os_task(wifi_connect, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);




      //Disarm timer
     os_timer_disarm(&send_timer);
     //Setup timer
     os_timer_setfn(&send_timer, (os_timer_func_t *)udp_timerfunc, NULL);
     //Arm the timer
     //&some_timer is the pointer
     //1000 is the fire time in ms
     //0 for once and 1 for repeating
     os_timer_arm(&send_timer, 200, 1);

}




//    struct ip_info info;
//    IP4_ADDR(&info.ip, 192, 168, 3, 200);
//    IP4_ADDR(&info.gw, 192, 168, 3, 1);
//    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
//    wifi_set_ip_info(STATION_IF, &info);
//    IP4_ADDR(&info.ip, 10, 10, 10, 1);
//    IP4_ADDR(&info.gw, 10, 10, 10, 1);
//    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
//    wifi_set_ip_info(SOFTAP_IF, &info);

//    char sofap_mac[6] = {0x16, 0x34, 0x56, 0x78, 0x90, 0xab};
//    char sta_mac[6] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};
//    wifi_set_macaddr(SOFTAP_IF, sofap_mac);
//    wifi_set_macaddr(STATION_IF, sta_mac);


    //bool system_rtc_mem_write (uint32 des_addr , void * src_addr , uint32 save_size)

//
//
//#define ESP_PARAM_START_SEC   0x3C
//
//#define ESP_PARAM_SAVE_0    1
//#define ESP_PARAM_SAVE_1    2
//#define ESP_PARAM_FLAG      3
//struct esp_platform_sec_flag_param {
//    uint8 flag;
//    uint8 pad[3];
//};
//
///******************************************************************************
// * FunctionName : user_esp_platform_load_param
// * Description  : load parameter from flash, toggle use two sector by flag value.
// * Parameters   : param--the parame point which write the flash
// * Returns      : none
//*******************************************************************************/
//void ICACHE_FLASH_ATTR
//user_esp_platform_load_param(void *param, uint16 len)
//{
//    struct esp_platform_sec_flag_param flag;
//
//    spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE,
//                   (uint32 *)&flag, sizeof(struct esp_platform_sec_flag_param));
//
//    if (flag.flag == 0) {
//        spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_0) * SPI_FLASH_SEC_SIZE,
//                       (uint32 *)param, len);
//    } else {
//        spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_1) * SPI_FLASH_SEC_SIZE,
//                       (uint32 *)param, len);
//    }
//}
//
///******************************************************************************
// * FunctionName : user_esp_platform_save_param
// * Description  : toggle save param to two sector by flag value,
// *              : protect write and erase data while power off.
// * Parameters   : param -- the parame point which write the flash
// * Returns      : none
//*******************************************************************************/
//void ICACHE_FLASH_ATTR
//user_esp_platform_save_param(void *param, uint16 len)
//{
//    struct esp_platform_sec_flag_param flag;
//
//    spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE,
//                       (uint32 *)&flag, sizeof(struct esp_platform_sec_flag_param));
//
//    if (flag.flag == 0) {
//        spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_SAVE_1);
//        spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_1) * SPI_FLASH_SEC_SIZE,
//                        (uint32 *)param, len);
//        flag.flag = 1;
//        spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_FLAG);
//        spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE,
//                        (uint32 *)&flag, sizeof(struct esp_platform_sec_flag_param));
//    } else {
//        spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_SAVE_0);
//        spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_0) * SPI_FLASH_SEC_SIZE,
//                        (uint32 *)param, len);
//        flag.flag = 0;
//        spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_FLAG);
//        spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE,
//                        (uint32 *)&flag, sizeof(struct esp_platform_sec_flag_param));
//    }
//}
