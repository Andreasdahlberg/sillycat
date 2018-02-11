
#ifndef TESTCRC_H_
#define TESTCRC_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void test_CRC_16_ZeroLength(void **state __attribute__((__unused__)));
void test_CRC_16_NullPointerZeroLength(void **state __attribute__((__unused__)));
void test_CRC_16_NullPointer(void **state __attribute__((__unused__)));
void test_CRC_16_SingleBytes(void **state __attribute__((__unused__)));
void test_CRC_16_SeveralBytes(void **state __attribute__((__unused__)));

#endif
