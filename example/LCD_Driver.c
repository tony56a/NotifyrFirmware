#include "LCD_Driver.h"

unsigned char buffer[240][50];

void LCDInit() {
	initGPIO();
	initSPI();
	initTimer();
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	delay(LCD_DELAY);
}

void clearMem(int16_t color) {
	int i, j;
	unsigned char fill_color = color ? 0x00 : 0xFF;
	for (i = 0; i < LCD_HEIGHT; i++) {
		for (j = 0; j < LCD_WIDTH_BYTES; j++) {
			buffer[i][j] = fill_color;
		}
	}
}

void clear()
{
	clearMem(WHITE);
	//now clear the lcd
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	SPISend(CMD_CLEAR); //send command
	SPISend(0); //send trailer
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void LCDStart() {
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
	TIM_Cmd(TIM4, ENABLE);
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
	clear();
}
void LcdStop() {
	TIM_Cmd(TIM4, DISABLE);
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
}

void sendLine(int line){
	int j = 0;
	SPISend(line+1);
	for (j = 0; j < LCD_WIDTH_BYTES; j++) {
		SPISend(buffer[line][j]);
	}
			//send trailer
	SPISend(0);
}

void refresh() {
	char cmdByte = 0;
	int i = 0;

	//set the command byte
	cmdByte = CMD_WRITE_ROW;
	//setup for the SPI write
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	delay(LCD_DELAY);
	SPISend(cmdByte);

	for (i = 0; i < LCD_HEIGHT; i++) {
		sendLine(i);
	}
	//send final trailer
	SPISend(0);
	//cleanup after the SPI write
	delay(LCD_DELAY);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void sendRow(int row)
{
	char cmdByte = 0;
	cmdByte=CMD_WRITE_ROW;
	//set up the SPI transfer
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	SPISend(cmdByte);
	//Send the row count to the LCD (add 1 because lcd counts first row as 1)
	sendLine(row);
	SPISend(0);
	//clean up after SPI transfer
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void SPISend(uint8_t value) {
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI2, value);
}

void drawPixel(int16_t x, int16_t y, uint16_t color) {

	int x_block = x/8;
	int x_offset = x%8;
	unsigned char block = buffer[y][x_block];
	if(!color){
		buffer[y][x_block] = (block |= 1 << x_offset);
	} else{
		buffer[y][x_block] = (block &= ~(1 << x_offset));
	}
}

void initGPIO() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8  | GPIO_Pin_13
			| GPIO_Pin_14 | GPIO_Pin_15; //GREEN AND BLUE LED (CHECK DOCUMENTATION ON AF)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


}

void initTimer() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	// Enable clock for TIM4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Period = 300 - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 32000 - 1;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;

	TIM_OC3Init(TIM4, &TIM_OCInitStruct);
}

void initSPI() {
	SPI_InitTypeDef SPI_InitStructure;
	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	SPI_I2S_DeInit(SPI2);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	/* Initializes the SPI communication */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}

void delay(__IO uint32_t nCount) {
	while (nCount--) {
	}
}
