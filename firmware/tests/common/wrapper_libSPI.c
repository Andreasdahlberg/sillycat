#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdint.h>
#include "wrapper_libSPI.h"

void __wrap_libSPI_Init(void)
{
	return;
}

uint32_t __wrap_libTimer_GetMilliseconds(void)
{
	return mock_type(uint32_t);
}

void __wrap_libSPI_WriteByte(uint8_t data_byte, libSPI_callback_type pre_write,
                      libSPI_callback_type post_write)
{
    if (pre_write != NULL)
    {
        pre_write();
    }

    if (post_write != NULL)
    {
        post_write();
    }
    
	return;
}

void __wrap_libSPI_ReadByte(uint8_t *data_byte, libSPI_callback_type pre_read,
                      libSPI_callback_type post_read)
{
    if (pre_read != NULL)
    {
        pre_read();
    }

    if (post__read != NULL)
    {
        post__read();
    }
    return;
}

void libSPI_SetMaster(bool master_value)
{
    return;
}

bool libSPI_SetMode(uint8_t mode)
{
    return mock_type(bool);
}