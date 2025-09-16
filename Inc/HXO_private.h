#ifndef HXO_PRIVATE_H_
#define HXO_PRIVATE_H_

#include "HXO_config.h"
#include "HXO_interface.h"

/* helpers */
static void HXO_voidDrawGrid(void);
static void HXO_voidClearFrame(void);
static void HXO_voidDrawCell(u8 row, u8 col);
static void HXO_voidDrawX(u16 cx, u16 cy, u16 radius);
static void HXO_voidDrawO(u16 cx, u16 cy, u16 radius);
static void HXO_voidDrawCursor(u8 row, u8 col);
static void HXO_voidFillRect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
static void HXO_voidSetPixel(u16 x, u16 y, u16 color);
static void HXO_voidDrawLine(int x0, int y0, int x1, int y1, u16 color);

/* board to screen mapping helpers */
static void HXO_voidComputeCellGeometry(void);

#endif /* HXO_PRIVATE_H_ */
