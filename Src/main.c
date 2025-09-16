#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "MRCC_int.h"
#include "MGPIO_int.h"
#include "MSTK_int.h"
#include "MSPI_int.h"
#include "HTFT_int.h"
#include "MEXTI_Interface.h"
#include "MNVIC_Interface.h"

#include "HMENU_interface.h"

volatile u8 flag=0;

void Button1()
{
	flag=1;
}

int main(void)
{
	/* Initialize system clock */
	MRCC_voidInit();

	/* Enable peripheral clocks */
	MRCC_voidEnablePeripheral(APB2_BUS, APB2_SPI1);  /* SPI1 for display */
	MRCC_voidEnablePeripheral(AHB1_BUS, AHB1_GPIOA); /* PORTA for pins */
	MRCC_voidEnablePeripheral(APB2_BUS, APB2_SYSCFG);

	/* Initialize SysTick timer */
	MSTK_voidInit();

	/* Initialize SPI for display communication */
	MSPI_voidMasterInit();

	/* Configure SPI pins (PA4=NSS, PA5=SCK, PA6=MISO, PA7=MOSI) */
	MGPIO_voidSetPinMode(PORTA, PIN4, ALT_FUN);
	MGPIO_voidSetAltFuncConfig(PORTA, PIN4, AF5);
	MGPIO_voidSetPinMode(PORTA, PIN5, ALT_FUN);
	MGPIO_voidSetAltFuncConfig(PORTA, PIN5, AF5);
	MGPIO_voidSetPinMode(PORTA, PIN6, ALT_FUN);
	MGPIO_voidSetAltFuncConfig(PORTA, PIN6, AF5);
	MGPIO_voidSetPinMode(PORTA, PIN7, ALT_FUN);
	MGPIO_voidSetAltFuncConfig(PORTA, PIN7, AF5);

	/* Configure button pins as inputs with pull-up*/
	//UP
	MGPIO_voidSetPinMode(PORTA, PIN8, INPUT);
	MGPIO_voidSetINPUTConfig(PORTA, PIN8, PULL_UP);
	//DOWN
	MGPIO_voidSetPinMode(PORTA, PIN9, INPUT);
	MGPIO_voidSetINPUTConfig(PORTA, PIN9, PULL_UP);
	//Select
	MGPIO_voidSetPinMode(PORTA, PIN10, INPUT);
	MGPIO_voidSetINPUTConfig(PORTA, PIN10, PULL_UP);



	HTFT_voidInit();

	HMenu_voidInit();

	while (1)
	{
		HMenu_voidPoll();
		MSTK_voidDelayus(20);
	}
	return 0;
}
