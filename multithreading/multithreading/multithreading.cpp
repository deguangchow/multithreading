///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    multithreading.cpp : 定义控制台应用程序的入口点。https://blog.csdn.net/phiall/article/details/52385165
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/28
#include "stdafx.h"
#include "LiftOff.h"
#include "thread_guard.h"
#include "mutex_test.h"
#include "promise_test.h"
#include "package_task_test.h"
#include "function_test.h"

int main() {
    main_lift_off();
    //main_thread_guard();
    //main_mutex_test();
    //main_promise_test();
    //main_packaged_task_test();
    //main_function_test();

    system("pause");
    return 0;
}

