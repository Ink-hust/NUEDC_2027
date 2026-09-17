#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

#define PI					3.14159265358979f

#define DAC_BUF_SIZE		100U

char     guart0_TxMsg[UART_BUF_SIZE];
char     guart0_RxMsg[UART_BUF_SIZE];
uint16_t gdac_ConvData[DAC_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

uint32_t sigFreq = 10000;

BSP_UART_HandleTypeDef buart = {
	.Instance = UART_0_INST,
	.dma_tx   = DMA_CH0_CHAN_ID,
	.pTxMsg   = guart0_TxMsg,
	.pRxMsg   = guart0_RxMsg,
};

LCD_HandleTypeDef blcd = {
	.Instance = &LCD_1_80_inch,
	.dir      = LCD_DIR_RIGHT,
	.TxBuf    = glcd_TxBuf,
};

static void PeriodicProcess(void);

int main(void)
{
	SYSCFG_DL_init();
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMG_0_INST_INT_IRQN);
	
	LCD_Init(&blcd, &LCD_Font_1608, NULL, WHITE, BLACK);
	LCD_Print(&blcd, 0, 16 * 0, "DAC Waveform");
	
	for (uint32_t i = 0; i < DAC_BUF_SIZE; ++i)
		gdac_ConvData[i] = 2048 + 2047 * sinf(2.0f * PI * i / DAC_BUF_SIZE);
	
	DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)gdac_ConvData);
	DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)&DAC0->DATA0);
	DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, DAC_BUF_SIZE);
	DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
	
	DL_Timer_startCounter(TIMG_8_INST);
	
	DL_Timer_startCounter(TIMG_0_INST);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (keyStatus)
	{
		if (sigFreq == 10000) sigFreq =  5000; else
		if (sigFreq ==  5000) sigFreq =  2000; else
		if (sigFreq ==  2000) sigFreq =  1000; else
		if (sigFreq ==  1000) sigFreq =   500; else
		if (sigFreq ==   500) sigFreq =   200; else
		if (sigFreq ==   200) sigFreq =   100; else
		if (sigFreq ==   100) sigFreq = 10000;
		
		DL_Timer_setLoadValue(TIMG_8_INST, 800000 / sigFreq - 1);
	}
	
	static uint32_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LED_TG(R);
}

void BSP_UART_RxCpltCallback(BSP_UART_HandleTypeDef *buart, uint16_t Size)
{
	BSP_UART_Transmit_DMA(buart, "RxLen: %u, RxMsg: %s", Size, buart->pRxMsg);
}

void UART_0_INST_IRQHandler(void)
{
	BSP_UART_IRQHandler(&buart);
}

void TIMG_0_INST_IRQHandler(void)
{
	switch (DL_Timer_getPendingInterrupt(TIMG_0_INST))
	{
		case DL_TIMER_IIDX_ZERO:
			PeriodicProcess();
			break;
		default:
			break;
	}
}
