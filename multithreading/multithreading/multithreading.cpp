﻿///    Copyright (C) 2018 cpp_prime5 DG.C
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

#if 0//package_task
    package_task_test::test_packaged_task_1();
    package_task_test::test_packaged_task_2();
    package_task_test::test_packaged_task_3();
    package_task_test::test_packaged_task_4();
    package_task_test::test_packaged_task_5();
    package_task_test::test_packaged_task_6();
#endif

#if 0
    main_packaged_task_test();
    main_function_test();
    main_future_test();
    main_condition_variable();
    main_atomic_flag_test();
    main_atomic_test();
    main_application_example();
#endif

    return 0;
}

