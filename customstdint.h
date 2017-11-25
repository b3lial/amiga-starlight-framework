#ifndef __CUSTOMSTDINT_H__
#define __CUSTOMSTDINT_H__

#ifdef __SASC
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
#else
#include <stdint.h>
#endif

#endif
