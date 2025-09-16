#include "BIT_MATH.h"
#include "STD_TYPES.h"

#include "MEXTI_Interface.h"
#include "MEXTI_Private.h"
#include "MEXTI_Config.h"

static void(*GLOBAL_PTR[16])(void)={NULL};
void MEXTI_voidSetInterruptPort(EXTI_Line_t Copy_uddtLine,EXTI_Port_t Copy_uddtPort)
{
	SYSCFG->EXTICR[Copy_uddtLine/SYS_DIV]&=~(SYS_MUSK<<((Copy_uddtLine%SYS_DIV)*4));
	SYSCFG->EXTICR[Copy_uddtLine/SYS_DIV]|=(Copy_uddtPort<<((Copy_uddtLine%SYS_DIV)*4));
}

void MEXTI_voidEnable(EXTI_Line_t Copy_uddtLine)
{
	EXTI->IMR|=(1<<Copy_uddtLine);
}
void MEXTI_voidDisable(EXTI_Line_t Copy_uddtLine)
{
	EXTI->IMR&=~(1<<Copy_uddtLine);
}
void MEXTI_voidSetTrigger(EXTI_Line_t Copy_uddtLine,EXTI_TriggerMode_t Copy_uddtMode)
{
	switch(Copy_uddtMode)
	{
		case EXTI_Falling:
		EXTI->RTSR&=~(1<<Copy_uddtLine);
		EXTI->FTSR|=(1<<Copy_uddtLine);
		break;
		case EXTI_Rising:
		EXTI->RTSR|=(1<<Copy_uddtLine);
		EXTI->FTSR&=~(1<<Copy_uddtLine);
		break;
		case EXTI_OnChange:
		EXTI->RTSR|=(1<<Copy_uddtLine);
		EXTI->FTSR|=(1<<Copy_uddtLine);
		break;
		default:
		break;
	}
}


void MEXTI_voidCallBack(void(*ptr)(void),EXTI_Line_t Copy_uddtLine)
{
	if(Copy_uddtLine<16)
	{
		GLOBAL_PTR[Copy_uddtLine]=ptr;
	}
}

void EXTI0_IRQHandler(void)
{
    // clear flag
    EXTI->PR = (1 << 0);
    // call user callback if registered
    if (GLOBAL_PTR[0] != NULL) {
        GLOBAL_PTR[0]();
    }
}

void EXTI1_IRQHandler(void)
{
    EXTI->PR = (1 << 1);
    if (GLOBAL_PTR[1] != NULL) {
        GLOBAL_PTR[1]();
    }
}

void EXTI2_IRQHandler(void)
{
    EXTI->PR = (1 << 2);
    if (GLOBAL_PTR[2] != NULL) {
        GLOBAL_PTR[2]();
    }
}

void EXTI3_IRQHandler(void)
{
    EXTI->PR = (1 << 3);
    if (GLOBAL_PTR[3] != NULL) {
        GLOBAL_PTR[3]();
    }
}

void EXTI4_IRQHandler(void)
{
    EXTI->PR = (1 << 4);
    if (GLOBAL_PTR[4] != NULL) {
        GLOBAL_PTR[4]();
    }
}

// Shared handler for EXTI5–EXTI9
void EXTI9_5_IRQHandler(void)
{
    for (u8 i = 5; i <= 9; i++) {
        if (EXTI->PR & (1 << i)) {
            EXTI->PR = (1 << i);   // clear flag
            if (GLOBAL_PTR[i] != NULL) {
                GLOBAL_PTR[i]();
            }
        }
    }
}

// Shared handler for EXTI10–EXTI15
void EXTI15_10_IRQHandler(void)
{
    for (u8 i = 10; i <= 15; i++) {
        if (EXTI->PR & (1 << i)) {
            EXTI->PR = (1 << i);   // clear flag
            if (GLOBAL_PTR[i] != NULL) {
                GLOBAL_PTR[i]();
            }
        }
    }
}

