#ifndef MGPIO_INT_H_
#define MGPIO_INT_H_

typedef enum{
    PORTA,
    PORTB,
    PORTC
}PORT_t;

typedef enum{
    PIN0,
    PIN1,
    PIN2,
    PIN3,
    PIN4,
    PIN5,
    PIN6,
    PIN7,
    PIN8,
    PIN9,
    PIN10,
    PIN11,
    PIN12,
    PIN13,
    PIN14,
    PIN15
}PIN_t;

typedef enum{
    INPUT,
    OUTPUT,
    ALT_FUN,
    ANALOG
}MODE_t;

typedef enum{
    PUSH_PULL,
    OPEN_DRAIN
}OUT_TYPE_t;

typedef enum{
    LOW_Speed,
    MED_Speed,
    HIGH_Speed,
    VHIGH_Speed
}SPEED_t;

typedef enum{
    LOW,
    HIGH
}VALUE_t;

typedef enum{
    FLOATING,
    PULL_UP,
    PULL_DOWN
}IN_TYPE_t;

typedef enum{
    AF0,
    AF1,
    AF2,
    AF3,
    AF4,
    AF5,
    AF6,
    AF7,
    AF8,
    AF9,
    AF10,
    AF11,
    AF12,
    AF13,
    AF14,
    AF15
}ALTERNATIVE_FUN_t;

void MGPIO_voidSetPinMode(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,MODE_t Copy_uddtMode);
//OUTPUT
void MGPIO_voidSetOutputConfig(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,OUT_TYPE_t Copy_uddtType,SPEED_t Copy_uddtSpeed);
void MGPIO_voidSetPinValue(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,VALUE_t Copy_uddtVal);
void MGPIO_voidSetAtomicPinValue(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,VALUE_t Copy_uddtVal);
//INPUT
void MGPIO_voidSetINPUTConfig(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,IN_TYPE_t Copy_uddtType);
u8 MGPIO_u8GetPinValue(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin);
//Alternative
void MGPIO_voidSetAltFuncConfig(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,ALTERNATIVE_FUN_t Copy_uddtAltFunc);

#endif