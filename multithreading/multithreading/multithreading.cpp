///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    multithreading.cpp : 定义控制台应用程序的入口点。https://blog.csdn.net/phiall/article/details/52385165
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/28
#include "stdafx.h"
#include "thread_test.h"
#include "thread_guard.h"
#include "mutex_test.h"
#include "promise_test.h"
#include "package_task_test.h"
#include "function_test.h"
#include "future_test.h"
#include "condition_variable_test.h"
#include "atomic_flag_test.h"
#include "atomic_test.h"
#include "application_example.h"

int main() {
    TICK();
#if 0//thread
    thread_test::test_lift_off();
    thread_test::test_lift_off_thread();
    thread_test::test_thread_join();
    thread_test::test_thread_detach();
    thread_test::test_thread_guard();
#endif

#if 0//mutex
    mutex_test::test_thread_hello_world();
    mutex_test::test_thread_constructor();
    mutex_test::test_thread_sleep();
    mutex_test::test_thread_mutex();
    mutex_test::test_thread_timed_mutex();
    mutex_test::test_thread_lock_guard();
    mutex_test::test_unique_lock();
    mutex_test::test_unique_lock2();
    mutex_test::test_once_flag();
#endif

#if 0//promise
    promise_test::test_promise1();
    promise_test::test_promise2();
    promise_test::test_promise3();
#endif

#if 0//future
    future_test::test_future_1();
    future_test::test_future_2();
    future_test::test_future_3();
    future_test::test_future_4();
    future_test::test_future_5();
    future_test::test_future_6();
    future_test::test_future_7();
#endif

#if 0//package_task
    package_task_test::test_packaged_task_1();
    package_task_test::test_packaged_task_2();
    package_task_test::test_packaged_task_3();
    package_task_test::test_packaged_task_4();
    package_task_test::test_packaged_task_5();
    package_task_test::test_packaged_task_6();
#endif

#if 0//function
    function_test::test_function_1();
    function_test::test_function_2();
    function_test::test_function_3();
    function_test::test_function_4();
    function_test::test_function_5();
#endif

#if 0//condition_variable
    condition_variable_test::test_condition_variable_1();
    condition_variable_test::test_condition_variable_2();
    condition_variable_test::test_condition_variable_3();
    condition_variable_test::test_condition_variable_4();
    condition_variable_test::test_condition_variable_5();
#endif

#if 0//atomic_flag
    atomic_flag_test::test_atomic_flag_1();
    atomic_flag_test::test_atomic_flag_2();
    atomic_flag_test::test_atomic_flag_3();
#endif

#if 0//atomic
    atomic_test::test_atomic_1();
    atomic_test::test_atomic_3();
    atomic_test::test_atomic_3();
    atomic_test::test_atomic_4();
    atomic_test::test_atomic_5();
#endif

#if 0//application_example
    application_example_test::test_app_ex_1();
    application_example_test::test_app_ex_2();
    application_example_test::test_app_ex_3();
    application_example_test::test_app_ex_4();
#endif

    return 0;
}

