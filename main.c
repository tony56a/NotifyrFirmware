#include "LCD_Driver.h"
#include "Graphics.h"
#include "USART_Example.h"
#include "RTC_Driver.h"
#include "Msg_queue.h"
#define START_TIMEOUT TIM_Cmd(TIM2, ENABLE)
#define STOP_TIMEOUT TIM_Cmd(TIM2, DISABLE)

extern int16_t cursor_x;
extern int16_t cursor_y;
extern uint16_t textcolor;
extern uint16_t textbgcolor;

extern uint8_t textsize;
extern uint8_t wrap;

int hour = 0;
int minute = 0;
int i = 0;

char timestring[9] = " ??:??  ";

void initTimeOut(){

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 4000 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 32000 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	//init TIM2 interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void printTime() {
	getTime(&hour, &minute);
	if(hour >=0 || minute >=0){
		timestring[1] = (hour / 10) + 48;
		timestring[2] = (hour % 10) + 48;
		timestring[4] = (minute / 10) + 48;
		timestring[5] = (minute % 10) + 48;
	}
	clearMem (WHITE);
	setTextSize(8);
	setCursor(30, Y_PADDING);
	writeString(timestring);
	setCursor(X_PADDING, textsize * 8);
	setTextSize(3);
	for (i = 0; i < QUEUE_SIZE / 2; i++) {
		write(i + 49);
		write(':');
		writeString((char *) getMsg(i));
		setCursor(X_PADDING, cursor_y + 8 * textsize);
	}
	refresh();
}

void printNewMsg() {
	STOP_TIMEOUT;
	clearMem (WHITE);
	setTextSize(3);
	fillRect(0, 0, 400, (textsize * 8) + Y_PADDING, BLACK);
	setTextColor(WHITE, BLACK);
	setCursor(X_PADDING, Y_PADDING / 2);
	writeString("New Message!");
	setCursor(X_PADDING, (textsize * 8) + Y_PADDING);
	setTextSize(2);
	setTextColor(BLACK, WHITE);
	writeString((char*) (RxBuffer+1));
	refresh();
	setCursor(X_PADDING, 0);
	enqeue ((uint8_t *)(RxBuffer+1));
	memset(RxBuffer,0,201);
}

int main(void)
{

	initTimeOut();
	initQueue();
	UART_init();
	LCDInit();
	clearMem(WHITE);
	LCDStart();

	refresh();
	while (1) {
		if (msg_flag) {
			if(RxBuffer[0] == 1){
				printNewMsg();
				START_TIMEOUT;
			} else if(RxBuffer[0] == 2){
				uint8_t h = RxBuffer[1]-1;
				uint8_t m = RxBuffer[2]-1;
				uint8_t s = RxBuffer[3]-1; //since it's probably a bad idea to have a data value that's the same as the string terminator
				RTCinit(h,m,s);
				rtc_time_flag = 1;
			}
			msg_flag = 0;
			RxBuffer[0] = 0;
		}if (rtc_time_flag) {
			printTime();
			rtc_time_flag = 0;
		}

	}

}

void TIM2_IRQHandler(void)
{
    //Check interrupt was triggered by update event of TIMn
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
    	rtc_time_flag = 1;
        //Clear the interrupt pending flag for timer update
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        TIM_Cmd(TIM2,DISABLE);
    }
}
