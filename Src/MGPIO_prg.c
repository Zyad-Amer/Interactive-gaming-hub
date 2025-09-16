#include"STD_TYPES.h"
#include"BIT_MATH.h"
#include"MGPIO_int.h"
#include"MGPIO_prv.h"

void MGPIO_voidSetPinMode(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,MODE_t Copy_uddtMode)
{
    if(((Copy_uddtPort == PORTA)) && (Copy_uddtPin >= PIN13)){}
    else if((Copy_uddtPort == PORTB) && ((Copy_uddtPin == PIN3)|| (Copy_uddtPin == PIN4) || (Copy_uddtPin == PIN5))){}
    else{
        switch (Copy_uddtPort)
        {
        case PORTA:
            GPIOA->MODER&=~(GPIO_MASK<<(Copy_uddtPin*2));
            GPIOA->MODER|=(Copy_uddtMode<<Copy_uddtPin*2);
            break;
        case PORTB:
            GPIOB->MODER&=~(GPIO_MASK<<(Copy_uddtPin*2));
            GPIOB->MODER|=(Copy_uddtMode<<Copy_uddtPin*2);
            break;
        case PORTC:
            GPIOC->MODER&=~(GPIO_MASK<<(Copy_uddtPin*2));
            GPIOC->MODER|=(Copy_uddtMode<<Copy_uddtPin*2);
            break;
        default:
            break;
        }
    }  
}

//OUTPUT
void MGPIO_voidSetOutputConfig(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,OUT_TYPE_t Copy_uddtType,SPEED_t Copy_uddtSpeed)
{
    if(((Copy_uddtPort == PORTA)) && (Copy_uddtPin >= PIN13)){}
    else if((Copy_uddtPort == PORTB) && ((Copy_uddtPin == PIN3)|| (Copy_uddtPin == PIN4) || (Copy_uddtPin == PIN5))){}
    else{
        switch (Copy_uddtPort)
        {
        case PORTA:
            GPIOA->OTYPER&=~(1<<Copy_uddtPin);                  //clear
            GPIOA->OTYPER|=(Copy_uddtType<<Copy_uddtPin);       //set the sent value
            GPIOA->OSPEEDR&=~(GPIO_MASK<<Copy_uddtPin*2);
            GPIOA->OSPEEDR|=(Copy_uddtSpeed<<Copy_uddtPin*2);
            break;
        case PORTB:
            GPIOB->OTYPER&=~(1<<Copy_uddtPin);
            GPIOB->OTYPER|=(Copy_uddtType<<Copy_uddtPin);
            GPIOB->OSPEEDR&=~(GPIO_MASK<<Copy_uddtPin*2);
            GPIOB->OSPEEDR|=(Copy_uddtSpeed<<Copy_uddtPin*2);
            break;
        case PORTC:
            GPIOC->OTYPER&=~(1<<Copy_uddtPin);
            GPIOC->OTYPER|=(Copy_uddtType<<Copy_uddtPin);
            GPIOC->OSPEEDR&=~(GPIO_MASK<<Copy_uddtPin*2);
            GPIOC->OSPEEDR|=(Copy_uddtSpeed<<Copy_uddtPin*2);
            break;
        default:
            break;
        }
    } 
}
void MGPIO_voidSetPinValue(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,VALUE_t Copy_uddtVal)
{
    if(((Copy_uddtPort == PORTA)) && (Copy_uddtPin >= PIN13)){}
    else if((Copy_uddtPort == PORTB) && ((Copy_uddtPin == PIN3)|| (Copy_uddtPin == PIN4) || (Copy_uddtPin == PIN5))){}
    else{
        switch (Copy_uddtPort)
        {
        case PORTA:
            GPIOA->ODR&=~(1<<Copy_uddtPin);
            GPIOA->ODR|=(Copy_uddtVal<<Copy_uddtPin);
            break;
        case PORTB:
            GPIOB->ODR&=~(1<<Copy_uddtPin);
            GPIOB->ODR|=(Copy_uddtVal<<Copy_uddtPin);
            break;
        case PORTC:
            GPIOC->ODR&=~(1<<Copy_uddtPin);
            GPIOC->ODR|=(Copy_uddtVal<<Copy_uddtPin);
            break;
        default:
            break;
        }
    } 
}
void MGPIO_voidSetAtomicPinValue(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,VALUE_t Copy_uddtVal)
{
    if(((Copy_uddtPort == PORTA)) && (Copy_uddtPin >= PIN13)){}
    else if((Copy_uddtPort == PORTB) && ((Copy_uddtPin == PIN3)|| (Copy_uddtPin == PIN4) || (Copy_uddtPin == PIN5))){}
    else{
        if(Copy_uddtVal == LOW)
            {
                Copy_uddtPin+=16;
            }
        switch (Copy_uddtPort)
        {
        case PORTA:
            GPIOA->BSRR=(1<<Copy_uddtPin);
            break;
        case PORTB:
            GPIOB->BSRR=(1<<Copy_uddtPin);
            break;
        case PORTC:
            GPIOC->BSRR=(1<<Copy_uddtPin);
            break;
        default:
            break;
        }
    } 
}
//INPUT
void MGPIO_voidSetINPUTConfig(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,IN_TYPE_t Copy_uddtType)
{
    if(((Copy_uddtPort == PORTA)) && (Copy_uddtPin >= PIN13)){}
    else if((Copy_uddtPort == PORTB) && ((Copy_uddtPin == PIN3)|| (Copy_uddtPin == PIN4) || (Copy_uddtPin == PIN5))){}
    else{
        switch (Copy_uddtPort)
        {
        case PORTA:
            GPIOA->PUPDR&=~(GPIO_MASK<<(Copy_uddtPin*2));
            GPIOA->PUPDR|=(Copy_uddtType<<Copy_uddtPin*2);
            break;
        case PORTB:
            GPIOB->PUPDR&=~(GPIO_MASK<<(Copy_uddtPin*2));
            GPIOB->PUPDR|=(Copy_uddtType<<Copy_uddtPin*2);
            break;
        case PORTC:
            GPIOC->PUPDR&=~(GPIO_MASK<<(Copy_uddtPin*2));
            GPIOC->PUPDR|=(Copy_uddtType<<Copy_uddtPin*2);
            break;
        default:
            break;
        }
    }  
}
u8 MGPIO_u8GetPinValue(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin)
{
    u8 Local_u8Value;
    if(((Copy_uddtPort == PORTA)) && (Copy_uddtPin >= PIN13)){}
    else if((Copy_uddtPort == PORTB) && ((Copy_uddtPin == PIN3)|| (Copy_uddtPin == PIN4) || (Copy_uddtPin == PIN5))){}
    else{
        switch (Copy_uddtPort)
        {
        case PORTA:
            Local_u8Value=GET_BIT(GPIOA->IDR,Copy_uddtPin);
            break;
        case PORTB:
            Local_u8Value=GET_BIT(GPIOB->IDR,Copy_uddtPin);
            break;
        case PORTC:
            Local_u8Value=GET_BIT(GPIOC->IDR,Copy_uddtPin);
            break;
        default:
            break;
        }
    }  
    return Local_u8Value;
}
//Alternative
void MGPIO_voidSetAltFuncConfig(PORT_t Copy_uddtPort,PIN_t Copy_uddtPin,ALTERNATIVE_FUN_t Copy_uddtAltFunc)
{
    if(((Copy_uddtPort == PORTA)) && (Copy_uddtPin >= PIN13)){}
    else if((Copy_uddtPort == PORTB) && ((Copy_uddtPin == PIN3)|| (Copy_uddtPin == PIN4) || (Copy_uddtPin == PIN5))){}
    else{
        switch (Copy_uddtPort)
        {
        case PORTA:
            if (Copy_uddtPin<8)
            {
                GPIOA->AFRL&=~(ALT_MUSK<<Copy_uddtPin*4);
                GPIOA->AFRL|=(Copy_uddtAltFunc<<Copy_uddtPin*4);
            }
            else if(Copy_uddtPin<16)
            {
                Copy_uddtPin-=8;
                GPIOA->AFRH&=~(ALT_MUSK<<Copy_uddtPin*4);
                GPIOA->AFRH|=(Copy_uddtAltFunc<<Copy_uddtPin*4);
            }
            break;
        case PORTB:
            if (Copy_uddtPin<8)
            {
                GPIOB->AFRL&=~(ALT_MUSK<<Copy_uddtPin*4);
                GPIOB->AFRL|=(Copy_uddtAltFunc<<Copy_uddtPin*4);
            }
            else if(Copy_uddtPin<16)
            {
                Copy_uddtPin-=8;
                GPIOB->AFRH&=~(ALT_MUSK<<Copy_uddtPin*4);
                GPIOB->AFRH|=(Copy_uddtAltFunc<<Copy_uddtPin*4);
            }
            break;
        case PORTC:
            if (Copy_uddtPin<8)
            {
                GPIOC->AFRL&=~(ALT_MUSK<<Copy_uddtPin*4);
                GPIOC->AFRL|=(Copy_uddtAltFunc<<Copy_uddtPin*4);
            }
            else if(Copy_uddtPin<16)
            {
                Copy_uddtPin-=8;
                GPIOC->AFRH&=~(ALT_MUSK<<Copy_uddtPin*4);
                GPIOC->AFRH|=(Copy_uddtAltFunc<<Copy_uddtPin*4);
            }
            break;
        default:
            break;
        }
    } 
}