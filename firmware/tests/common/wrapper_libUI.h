#include "libUI.h"

void __wrap_libUI_SetFont(FONT_INFO *font);
void __wrap_libUI_Update(void);
void __wrap_libUI_DrawLine(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);