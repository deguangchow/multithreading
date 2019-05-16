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
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <assert.h>
#include <mutex>
#include <future>
#include <exception>
#include <functional>
#include <ctime>
#include <atomic>
#include <vector>

#include "console_log.h"


//using namespace std;
using std::thread;
using std::this_thread::sleep_for;
using std::this_thread::get_id;
using std::this_thread::yield;

using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::time_point;
using std::chrono::system_clock;

using std::string;
using std::stringstream;
using std::ios;
using std::cin;
using std::cout;
using std::endl;
using std::exception;
using std::current_exception;
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

using std::future;
using std::future_status;
using std::shared_future;
using std::async;
using std::launch;
using std::promise;
using std::packaged_task;
using std::function;
using std::bind;
using std::placeholders::_1;
using std::ref;

using std::condition_variable;
using std::cv_status;

using std::atomic;
using std::atomic_flag;
using std::memory_order_acquire;
using std::memory_order_release;
using std::memory_order_relaxed;

using std::vector;


#endif  //STDAFX_H
