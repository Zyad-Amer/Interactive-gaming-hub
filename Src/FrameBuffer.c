/*
 * FrameBuffer.c
 *
 *  Created on: Sep 12, 2025
 *      Author: Zyad Montaser
 */

#include "STD_TYPES.h"
#include "HTFT_conf.h"
#include "FrameBuffer.h"

/* Single global framebuffer (placed once in RAM) */
u16 GlobalFrameBuffer[IMAGE_SIZE];

