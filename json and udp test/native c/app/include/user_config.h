#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__
#include "osapi.h"
#include "at_custom.h"
#include "user_interface.h"
#include "espconn.h"
#include "c_types.h"
#include "mem.h"
#include "gpio.h"
typedef enum
{
  teClient,
  teServer
}teType;

typedef struct
{
	bool linkEn;
	bool teToff;
	uint8_t linkId;
	teType teType;
	uint8_t repeaTime;
	uint8_t changType;
	uint8 remoteIp[4];
	int32_t remotePort;
	struct espconn *pCon;
}at_linkConType;

#endif
