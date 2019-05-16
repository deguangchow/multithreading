///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    promise test. 线程间通信：共享状态（shared state）的例子。
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/25
#pragma once
#ifndef PROMISE_TEST_H
#define PROMISE_TEST_H

namespace promise_test {

/*
5. std::promise
*/
//通过std::future获取共享状态的值
void printShareState(future<int> &state) {
    int x = state.get();
    DEBUG("share state value : %d", x);
}

//5.1
promise<int> g_promise1;
void printShareStateValue() {
    future<int> fut = g_promise1.get_future();
    int x = fut.get();//future<T>::get() 方法只能调用一次，对应 promise<T>::set_value() 方法
    DEBUG("share state value is %d", x);
}

//5.2
void InputAnInteger(promise<int> &prom) {
    int x;
    INFO("input an integer : ");
    //设置如试图从不能解析为整数的字符串里想要读一个整数等，顺便说下eof也会造成failbit被置位
    //则产生异常
    cin.exceptions(ios::failbit);
    try {
        cin >> x;
        prom.set_value(x);
    }
    catch (exception &) {
        prom.set_exception(current_exception());
    }
}

void OutputAnInteger(future<int> &fut) {
    try {
        int x = fut.get();
        DEBUG("the value of Integer is %d", x);
    }
    catch (exception const &e) {
        DEBUG("exception content:{%s}", e.what());
    }
}

void test_promise1() {
    TICK();
    /*
    5. std::promise
    */
    //创建一个 promise<int> 对象，状态值为int类型
    promise<int> prom;

    //和 future 关联
    future<int> fut = prom.get_future();

    //将 future 交个另外一个线程
    thread t(printShareState, ref(fut));

    // 设置共享状态的值，此处和线程t保持同步
    prom.set_value(10);

    t.join();
}

void test_promise2() {
    TICK();
    thread t1(printShareStateValue);
    g_promise1.set_value(10);
    t1.join();

    //promise<int>() 创建一个匿名空的 promise 对象，使用移动拷贝构造函数给 g_promise1
    g_promise1 = promise<int>();

    thread t2(printShareStateValue);
    g_promise1.set_value(20);//g_promise1 被 set_value 之后不能重复设置，需要重新分配内存.
    t2.join();
}

void test_promise3() {
    TICK();
    promise<int> prom;
    future<int> fut = prom.get_future();
    thread t1(InputAnInteger, ref(prom));
    thread t2(OutputAnInteger, ref(fut));
    t1.join();
    t2.join();
}


}//namespace promise_test

#endif  //PROMISE_TEST_H


