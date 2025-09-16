
#include"STD_TYPES.h"
#include"BIT_MATH.h"

#include"MRCC_int.h"
#include"MRCC_conf.h"
#include"MRCC_prv.h"

void MRCC_voidInit()
{
   #if (SYSTEM_CLOCK==HSI_CLOCK)
		RCC->CR|=(1<<HSION);
		while(!GET_BIT(RCC->CR,HSIRDY));
		RCC->CFGR&=~(SW_MUSK<<SW0);
		RCC->CFGR&=~(HSI_Oscillator<<SWS);
   #elif(SYSTEM_CLOCK==HSE_CLOCK)
        #if(HSE_OSC==MECHANICAL_CLOCK)
			RCC->CR&=~(1<<HSEBYP);
        #elif(HSE_OSC==RC_CLOCK)
		    RCC->CR|=(1<<HSEBYP);
        #endif
			RCC->CR|=(1<<HSEON);
			while(!GET_BIT(RCC->CR,HSERDY));
			RCC->CFGR&=~(SW_MUSK<<SW0);
			RCC->CFGR|=(HSE_Oscillator<<SWS);
			RCC->CR&=~(1<<HSION);
			while (RCC->CR & (1 << HSIRDY));
  #elif(SYSTEM_CLOCK==PLL)
		#if(PLL_SOURCE==HSI_PLL)
			//OPEN THE HSI CLOCK AND Wit till it ready
			RCC->CR|=(1<<HSION);
			while(!GET_BIT(RCC->CR,HSIRDY));
			RCC->CFGR&=~(SW_MUSK<<SW0);
			RCC->CFGR&=~(HSI_Oscillator<<SWS);
			//CHOOSE THE HSI AS A SOURCE TO PLL
			RCC->PLLCFGR&=~(1<<PLLSRC);
        #elif(PLL_SOURCE==HSE_PLL)
			#if(HSE_OSC==MECHANICAL_CLOCK)
				RCC->CR&=~(1<<HSEBYP)
			#elif(HSE_OSC==RC_CLOCK)
				RCC->CR&|=(1<<HSEBYP)
			#endif
				//CHOOSE THE HSI AS A SOURCE TO PLL
				RCC->PLLCFGR|=(1<<PLLSRC);
				RCC->CR|=(1<<HSEON);
				while(!GET_BIT(RCC->CR,HSERDY));
				RCC->CFGR&=~(SW_MUSK<<SW0);
				RCC->CFGR|=(HSE_Oscillator<<SWS);
				RCC->CR&=~(1<<HSION);
		 #endif
			//PUT THE P
			RCC->PLLCFGR &=~(0x3<<PLLP);
			RCC->PLLCFGR|=(PLLP_MUSK<<PLLP);
			//PUT THE N
			RCC->PLLCFGR &= ~(0x1FF << PLLN);
			RCC->PLLCFGR|=(PLLN_MUSK<<PLLN);
			//PUT THE M
			RCC->PLLCFGR &= ~(0x3F << PLLM);
			RCC->PLLCFGR|=(PLLM_MUSK<<PLLM);
			//PUT THE Q
			RCC->PLLCFGR &= ~(0xF << PLLQ);
			RCC->PLLCFGR|=(PLLQ_MUSK<<PLLQ);
			RCC->CR |= (1<<PLLON);                // Turn on PLL
			while(!GET_BIT(RCC->CR,PLLRDY));     // Wait until PLL is ready
			RCC->CFGR&=~(SW_MUSK<<SW0);
			RCC->CFGR|=(PLL_Oscillator<<SWS);


#endif
}
void MRCC_voidEnablePeripheral(System_Bus_t Copy_uddtBus,u8 Copy_u8Peripehral)
{
	switch(Copy_uddtBus)
	{
		case AHB1_BUS:
		RCC->AHB1ENR|=(1<<Copy_u8Peripehral);
		break;
		case AHB2_BUS:
		RCC->AHB2ENR|=(1<<Copy_u8Peripehral);
		break;
		case APB1_BUS:
		RCC->APB1ENR|=(1<<Copy_u8Peripehral);
		break;
		case APB2_BUS:
		RCC->APB2ENR|=(1<<Copy_u8Peripehral);
		break;

		default:
		break;
	}
}
void MRCC_voidDisablePeripheral(System_Bus_t Copy_uddtBus,u8 Copy_u8Peripehral)
{
	switch(Copy_uddtBus)
	{
		case AHB1_BUS:
		RCC->AHB1ENR&=~(1<<Copy_u8Peripehral);
		break;
		case AHB2_BUS:
		RCC->AHB2ENR&=~(1<<Copy_u8Peripehral);
		break;
		case APB1_BUS:
		RCC->APB1ENR&=~(1<<Copy_u8Peripehral);
		break;
		case APB2_BUS:
		RCC->APB2ENR&=~(1<<Copy_u8Peripehral);
		break;
		default:
		break;
	}
}

