#ifndef MSTK_INT_H_
#define MSTK_INT_H_

typedef enum{
	Systick_IntDisable=0,
	Systick_IntEnable
}STK_IntState;

void MSTK_voidInit();
void MSTK_voidStartTimer(u32 Copy_u32ReloadValue);
void MSTK_voidCtrlIntState(STK_IntState Copy_uddtIntState);
u8 MSTK_u8ReadFlag();
/*Function in Single shot*/
u32 MSTK_u32GetElapsedTickSingleShot();
u32 MSTK_u32GetRemainingTickSingleShot();

void MSTK_voidDelayms(u32 Copy_u32Delayms);
void MSTK_voidDelayus(u32 Copy_u32Delayus);

void MSTK_voidCallBack(void(*ptr)(void), u32 Copy_u32TickTime);
void MSTK_voidCallBackSingle(void(*ptr)(void), u32 Copy_u32TickTime);


#endif