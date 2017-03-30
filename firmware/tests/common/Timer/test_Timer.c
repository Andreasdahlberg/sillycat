#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdint.h>

#include "test_Timer.h"
#include "Timer.h"

void __wrap_libTimer_Init(void)
{
	return;
}

uint32_t __wrap_libTimer_GetMilliseconds(void)
{
	return mock_type(uint32_t);
}

void test_Timer_TimeDifference_zero(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);   
    will_return(__wrap_libTimer_GetMilliseconds, 500); 

    assert_int_equal(Timer_TimeDifference(0), 0);
    assert_int_equal(Timer_TimeDifference(500), 0);     
    
    return;
}

void test_Timer_TimeDifference_nonzero(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 100);   
    will_return(__wrap_libTimer_GetMilliseconds, 500);  

    assert_int_equal(Timer_TimeDifference(0), 100);
    assert_int_equal(Timer_TimeDifference(100), 400);     
    
    return;    
}

void test_Timer_TimeDifference_wraparound(void **state)
{
    uint32_t timestamp = (0xFFFFFFFF - 100);

    will_return(__wrap_libTimer_GetMilliseconds, 0xFFFFFFFF);   
    will_return(__wrap_libTimer_GetMilliseconds, 100);  

    assert_int_equal(Timer_TimeDifference(timestamp), 100);   
    assert_int_equal(Timer_TimeDifference(timestamp), 200);

    return;
}

void test_Timer_GetSeconds_exact(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 2000);

    assert_int_equal(Timer_GetSeconds(), 0);   
    assert_int_equal(Timer_GetSeconds(), 2);    
    return;
}

void test_Timer_GetSeconds_ceil(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 500);  
    will_return(__wrap_libTimer_GetMilliseconds, 501);                  

    assert_int_equal(Timer_GetSeconds(), 0);
    assert_int_equal(Timer_GetSeconds(), 1);
    assert_int_equal(Timer_GetSeconds(), 1);        
    return;
}

void test_Timer_GetSeconds_floor(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 499);
    will_return(__wrap_libTimer_GetMilliseconds, 1200); 

    assert_int_equal(Timer_GetSeconds(), 0);
    assert_int_equal(Timer_GetSeconds(), 0);
    assert_int_equal(Timer_GetSeconds(), 1);        
    return;
}

#if 0
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_Timer_TimeDifference_zero),
        cmocka_unit_test(test_Timer_TimeDifference_nonzero), 
        cmocka_unit_test(test_Timer_TimeDifference_wraparound),
        cmocka_unit_test(test_Timer_GetSeconds_exact), 
        cmocka_unit_test(test_Timer_GetSeconds_ceil),  
        cmocka_unit_test(test_Timer_GetSeconds_floor), 
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
#endif