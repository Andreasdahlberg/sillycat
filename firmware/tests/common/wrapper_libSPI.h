
typedef void (*libSPI_callback_type)(void);

void __wrap_libSPI_Init(void);
uint32_t __wrap_libTimer_GetMilliseconds(void);
void __wrap_libSPI_WriteByte(uint8_t data_byte, libSPI_callback_type pre_write,
                      libSPI_callback_type post_write);
void __wrap_libSPI_ReadByte(uint8_t *data_byte, libSPI_callback_type pre_read,
                      libSPI_callback_type post_read);
bool libSPI_SetMode(uint8_t mode);
