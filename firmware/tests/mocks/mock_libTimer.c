#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdint.h>
#include "mock_libTimer.h"

void __wrap_libTimer_Init(void)
{
	return;
}

uint32_t __wrap_libTimer_GetMilliseconds(void)
{
	return mock_type(uint32_t);
}
