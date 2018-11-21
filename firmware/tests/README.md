# Testing

[Cmocka](https://cmocka.org/) is used as the unit testing framework for firmware.

## Build test suites
Use the following command to build all test suites:
```bash
$ scons scons build-tests
```

It's also possible to build a specific test suite, e.g. the timer test suite:
```bash
$ scons tests/common/timer/
```

## Run test suites
Run all test suites:
```bash
$ scons coverage
```

Run a specific test suite, e.g. the timer test suite.
```bash
$ build/unit_tests/timer/TestRunner
```

## Add a new test suite
1. Add a new directory for the test suite in *tests*. It's important that the
directory structure is the same as in the *src* directory. Assume we want to
add a test suite for the *src/main/encoder* module, then we create
*tests/main/encoder*.

1. Create a SConscript file with the following content
    ```python
    import os

    Import(['*'])

    env.Append(CPPPATH=[
        '<PATH_TO_INCLUDE>'
    ])

    env.Append(LINKFLAGS=[
        '-Wl,--wrap=<FUNCTION_TO_MOCK>'
    ])

    SOURCE = Glob('*.c')
    OBJECTS = env.Object(source=SOURCE)

    Return('OBJECTS')
    ```
1. Implement the test suite in *test_MODULE_NAME.c*. A basic template is
included below but the existing test suites is the best place to learn how to write tests.
    ```c
    #include <stdarg.h>
    #include <stddef.h>
    #include <setjmp.h>
    #include <cmocka.h>

    void test_SomeImportantTest(void **state)
    {
        /* Implement test case here. */
    }

    int main(void)
    {
        const struct CMUnitTest tests[] =
        {
            cmocka_unit_test(test_SomeImportantTest)
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
    }
    ```
1. Enable the test by adding the full module path to the *modules* list in the
*SConstruct* file.
    ```python
    modules = [
        os.path.join('utility', 'CRC'),
        os.path.join('utility', 'List'),
        os.path.join('utility', 'FIFO'),
        os.path.join('main', 'encoder'),
        os.path.join('main', 'interface'),
        os.path.join('common', 'timer')
    ]
    ```
1. Thats all! You can now [build](#build-test-suites) and [run](#run-test-suites) your new test suite.


## GDB
If a test fails with segmentation fault [GDB](https://www.gnu.org/software/gdb/)
is a great tool to use to track down the fault.
```bash
$ gdb build/unit_tests/timer/TestRunner
```

This will load the test application into GDB and present the GDB console.
