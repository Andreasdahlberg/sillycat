
void __wrap_libTimer_Init(void);
uint32_t __wrap_libTimer_GetMilliseconds(void);

void test_Timer_TimeDifference_zero(void **state);
void test_Timer_TimeDifference_nonzero(void **state);
void test_Timer_TimeDifference_wraparound(void **state);
void test_Timer_GetSeconds_exact(void **state);
void test_Timer_GetSeconds_ceil(void **state);
void test_Timer_GetSeconds_floor(void **state);