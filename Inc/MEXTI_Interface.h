
#ifndef MEXTI_INTERFCAE_H
#define MEXTI_INTERFCAE_H

typedef enum{
	EXTI_Line0,
	EXTI_Line1,
	EXTI_Line2,
	EXTI_Line3,
	EXTI_Line4,
	EXTI_Line5,
	EXTI_Line6,
	EXTI_Line7,
	EXTI_Line8,
	EXTI_Line9,
	EXTI_Line10,
	EXTI_Line11,
	EXTI_Line12,
	EXTI_Line13,
	EXTI_Line14,
	EXTI_Line15
}EXTI_Line_t;

typedef enum{
	EXTI_PORTA,
	EXTI_PORTB,
	EXTI_PORTC
}EXTI_Port_t;
typedef enum{
	EXTI_Falling,
	EXTI_Rising,
	EXTI_OnChange
}EXTI_TriggerMode_t;

void MEXTI_voidSetInterruptPort(EXTI_Line_t Copy_uddtLine,EXTI_Port_t Copy_uddtPort);
void MEXTI_voidEnable(EXTI_Line_t Copy_uddtLine);
void MEXTI_voidDisable(EXTI_Line_t Copy_uddtLine);
void MEXTI_voidSetTrigger(EXTI_Line_t Copy_uddtLine,EXTI_TriggerMode_t Copy_uddtMode);
void MEXTI_voidCallBack(void(*ptr)(void),EXTI_Line_t Copy_uddtLine);

#endif
