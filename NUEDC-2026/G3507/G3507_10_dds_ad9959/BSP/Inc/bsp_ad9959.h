#ifndef __BSP_AD9959_H__
#define __BSP_AD9959_H__

#include "bsp.h"

#define DDS_MCLK			500000000U

#define DDS_CHANNEL_0		0x10U
#define DDS_CHANNEL_1		0x20U
#define DDS_CHANNEL_2		0x40U
#define DDS_CHANNEL_3		0x80U

#define DDS_UPD_PORT		DDS_PORT
#define DDS_SCL_PORT		DDS_PORT
#define DDS_SDA_PORT		DDS_PORT
#define DDS_RST_PORT		DDS_PORT
#define DDS_CS_PORT			DDS_PORT

#define DDS_UPD_PIN			DDS_UPD2_PIN
#define DDS_SCL_PIN			DDS_SCL2_PIN
#define DDS_SDA_PIN			DDS_SDA2_PIN
#define DDS_RST_PIN			DDS_RST2_PIN
#define DDS_CS_PIN			DDS_CS2_PIN

void DEV_DDS_Init(void);
void DEV_DDS_Config(uint32_t chan, float freq, float phs, uint32_t amp);

#endif /* __BSP_AD9959_H__ */
