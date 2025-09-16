#ifndef BIT_MATH_H_
#define BIT_MATH_H_

#define SET_BIT(reg,bit)    reg|=(1<<bit)
#define TOG_BIT(reg,bit)    reg^=(1<<bit)
#define CLR_BIT(reg,bit)    reg&=~(1<<bit)
#define GET_BIT(reg,bit)    (reg>>bit)&1


/*
ROTR(reg)
ROTl(reg)
SWAP_NIP(reg)
ASS_BIT(reg,bit,val)
*/

#endif