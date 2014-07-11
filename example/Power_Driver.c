#include "Power_Driver.h"

void enterLPMode(){

	  /* Enable the PWR APB1 Clock */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	  /* Select the Voltage Range 2 (1.5V) */
	  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);

	  /* Wait Until the Voltage Regulator is ready */
	  while(PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
	  {
	  }

	  /* HCLK = SYSCLK/2 = ~32KHz */
	  RCC_HCLKConfig(RCC_SYSCLK_Div2);

	  /* PCLK2 = HCLK */
	  RCC_PCLK2Config(RCC_HCLK_Div1);

	  /* PCLK1 = HCLK */
	  RCC_PCLK1Config(RCC_HCLK_Div1);

	  /* Set MSI clock range to 65.536KHz */
	  RCC_MSIRangeConfig(RCC_MSIRange_0);

	  /* Select MSI as system clock source */
	  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

	  /* Wait till PLL is used as system clock source */
	  while (RCC_GetSYSCLKSource() != 0x00);

	  /* Exit the RUN LP Mode */
	   PWR_EnterLowPowerRunMode(DISABLE);

	   /* Wait until the system exits RUN LP and the Regulator is in main mode */
	   while(PWR_GetFlagStatus(PWR_FLAG_REGLP) != RESET);

}
void exitLPMode(){

	PWR_EnterLowPowerRunMode(DISABLE);
	SetSysClock();

}


