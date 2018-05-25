/*
copyright:	deguangchow@qq.com
data:		2018/05/03
autor:		deguangchow
brief:		定义控制台应用程序的入口点
url:        https://blog.csdn.net/phiall/article/details/52385165
*/
// multithreading.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "LiftOff.h"
#include "thread_guard.h"
#include "mutex_test.h"
#include "promise_test.h"
#include "package_task_test.h"
#include "function_test.h"

int main() {
    main_function_test();

    system("pause");
    return 0;
}

