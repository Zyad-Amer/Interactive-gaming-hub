#include"BIT_MATH.h"
#include"STD_TYPES.h"
#include"MGPIO_int.h"
#include "MSPI_int.h"
#include "MSPI_prv.h"

void MSPI_voidMasterInit()
{
//Hardware or software SSM/SSI
SPI1->CR1|=(1<<SSM);
SPI1->CR1|=(1<<SSI);
//Master Mode
SPI1->CR1|=(1<<MSTR);
//Direction (Full duplex)
SPI1->CR1&=~(1<<BIDIMODE);
//Data (8 bit - 16 bit)
SPI1->CR1&=~(1<<DFF);
//MSB or LSB
SPI1->CR1&=~(1<<LSBFIRST);
//CPOL
SPI1->CR1|=(1<<CPOL);
//CPHA
SPI1->CR1&=~(1<<CPHA);
//clock
SPI1->CR1&=~(CLK_MUSK<<BR0);
//SPE Enable
SPI1->CR1|=(1<<SPE);
}

void MSPI_voidSlaveInit()
{
//Slave Mode
SPI1->CR1&=~(1<<MSTR);
//Direction (Full duplex)
SPI1->CR1&=~(1<<BIDIMODE);
//Data (8 bit - 16 bit)
SPI1->CR1&=~(1<<DFF);
//MSB or LSB
SPI1->CR1&=~(1<<LSBFIRST);
//CPOL
SPI1->CR1|=(1<<CPOL);
//CPHA
SPI1->CR1&=~(1<<CPHA);
//clock
//SPI1->CR1&=~(CLK_MUSK<<BR0);
//SPE Enable
SPI1->CR1|=(1<<SPE);
}

u16 MSPI_u16Tranceive(u16 Copy_u16Data)
{
while(!GET_BIT(SPI1->SR,TXE));
SPI1->DR=Copy_u16Data;
while(!GET_BIT(SPI1->SR,RXNE));
return (u16)SPI1->DR;
}