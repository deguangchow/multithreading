/*
copyright:	deguangchow@qq.com
data:		2018/05/03
autor:		deguangchow
brief:		标准系统包含文件的包含文件
*/
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//
#ifndef STDAFX_H
#define STDAFX_H
#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <thread>
#include <iostream>
#include <string>
#include <windows.h>
#include <assert.h>
#include <mutex>

//using namespace std;
using std::thread;
using std::this_thread::sleep_for;
using std::this_thread::get_id;
using std::chrono::milliseconds;
using std::chrono::microseconds;

using std::string;
using std::cout;
using std::endl;
using std::exception;
using std::logic_error;
using std::move;
using std::ref;

using std::mutex;
using std::timed_mutex;
using std::lock_guard;
using std::unique_lock;
using std::defer_lock;
using std::once_flag;
using std::call_once;




#endif  //STDAFX_H

