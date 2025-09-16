
#ifndef MRCC_PRIVATE_H_
#define MRCC_PRIVATE_H_

#define RCC_BASE_ADDRESS (0x40023800)
typedef struct {
	u32 CR;
	u32 PLLCFGR;
	u32 CFGR ;
	u32 CIR;
	u32 AHB1RSTR;
	u32 AHB2RSTR;
	u32 Reserved [2];
	u32 APB1RSTR;
	u32 APB2RSTR;
	u32 Reserved2[2];
	u32 AHB1ENR;
	u32 AHB2ENR;
	u32 Reserved3[2];
	u32 APB1ENR;
	u32 APB2ENR;
	u32 Reserved4[2];
	u32 AHB1LPENR;
	u32 AHB2LPENR;
	u32 Reserved5[2];
	u32 APB1LPENR;
	u32 APB2LPENR;
	u32 Reserved6[2];
	u32 BDCR;
	u32 CSR;
	u32 Reserved7[2];
	u32 SSCGR ;
	u32 PLLI2SCFGR;
	u32 Reserved8[2];
	u32 DCKCFGR;
}RCC_t;

#define RCC ((volatile RCC_t*)RCC_BASE_ADDRESS)

#define HSI_CLOCK   0
#define HSE_CLOCK   1
#define PLL         2

#define HSION       0
#define HSIRDY      1

typedef enum {
	HSI_Oscillator=0,
	HSE_Oscillator,
	PLL_Oscillator
}Clock_OSC;

#define SW_MUSK 3
#define SW0  0
#define SWS  2

#define HSEON 16
#define HSERDY 17
#define HSEBYP 18

#define MECHANICAL_CLOCK 0
#define RC_CLOCK         1

#define PLLSRC 22
#define PLLP   16
#define PLLM   0
#define PLLN   6
#define PLLQ   24
#define PLLON  24
#define PLLRDY 25

#endif /* MRCC_PRIVATE_H_ */
