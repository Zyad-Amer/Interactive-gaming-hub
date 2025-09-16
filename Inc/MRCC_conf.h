
#ifndef MRCC_CONFIG_H_
#define MRCC_CONFIG_H_

//choose the clk type
#define SYSTEM_CLOCK   HSI_CLOCK
#define HSE_OSC   RC_CLOCK

/*
 * chhose the PLL clk source
 * 1- HSI_PLL
 * 2- HSE_PLL
 */

#define PLL_SOURCE    HSI_PLL
#define PLL_outCLOCK     84
#define HSE_PLL          16


#define PLLP_MUSK 3
#define PLLM_MUSK 8
#define PLLN_MUSK 336
#define PLLQ_MUSK 7

#endif /* MRCC_CONFIG_H_ */
