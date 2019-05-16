///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    std::future
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/28
#pragma once
#ifndef FUTURE_TEST_H
#define FUTURE_TEST_H

namespace future_test {


/*
7.std::future
*/
//是否为质数
bool is_prime(int x) {
    TICK();
    for (int i = 2; i < x; ++i) {
        if (x%i == 0) {
            return false;
        }
    }
    return true;
}

//7.1 std::future::share
int do_get_value() {
    TICK();
    return 10;
}

//7.2 std::future::valid
int simple_task() {
    TICK();
    INFO("do task");
    return 1;
}

//7.3 std::future::wait
// 为了体现效果，该函数故意没有优化
bool do_task(int x) {
    TICK();
    for (int i = 2; i < x; ++i) {
        int a = x / i;
        a = x%i;
    }
    return x % 2 != 0;
}

//7.4 std::launch::async、std::launch::deferred
void task_launch(char c, int ms) {
    TICK();
    for (int i = 0; i < 10; ++i) {
        sleep_for(milliseconds(ms));
        DEBUG("%c", c);
    }
}

void test_future_1() {
    TICK();
    //7.std::future
    //异步调用判断999999是不是质数
    auto const& a = 9999999999;
    future<bool> fut = async(is_prime, a);

    //由于上面代码中判断素数的方法对于比较大的数，将耗费较多时间
    //开始判断共享状态是否变为了ready，等待
    INFO("waiting for ready...");

    //每100ms内阻塞等待，如果超时输出*，并继续等待，否则结束while
    milliseconds span(100);
    while (fut.wait_for(span) == future_status::timeout) {
        DEBUG("*");
    }

    bool ret = fut.get();
    INFO("%lld %s a prime.", a, (ret ? "is" : "is not"));
}

void test_future_2() {
    TICK();
    //7.1 std::future::share
    future<int> fut = async(do_get_value);

    INFO("Call fut.share()");
    shared_future<int> shared_fut = fut.share();

    DEBUG("fut`s state %s valid.", (fut.valid() ? "is" : "is not"));

    //共享的future 对象可以被多次访问，fut的状态到这里便不再是valid的了
    INFO("value: %d", shared_fut.get());
    INFO("its double: %d", shared_fut.get() * 2);
}

void test_future_3() {
    TICK();

    //7.2 std::future::valid
    //由默认构造函数创建的 future 对象
    //初始化时该 future 读写的共享状态处于为 invalid 状态。
    future<int> foo, bar;

    //move 赋值，foo 的共享状态变为 valid
    foo = async(simple_task);

    //move 赋值，bar 的共享状态变为 valid，而 move 赋值以后 foo 的共享状态变为 invalid。
    bar = move(foo);

    if (foo.valid()) {
        INFO("foo`s value: %d", foo.get());
    } else {
        ERR("foo is not valid");
    }

    if (bar.valid()) {
        INFO("bar`s value: %d", bar.get());
    } else {
        ERR("bar is not valid");
    }
}

void test_future_4() {
    TICK();

    //7.3 std::future::wait
    //异步调用do_task()
    future<bool> fut = async(do_task, 99999999);

    INFO("Waiting...");

    fut.wait();

    if (fut.get()) {
        INFO("return true");
    } else {
        ERR("return false");
    }
}

void test_future_5() {
    TICK();

    //7.4 std::future::wait_for
    INFO("wait_for example :");

    //异步调用判断99999999999是不是质数
    auto const& a = 9999999999;
    future<bool> fut = async(is_prime, a);

    milliseconds span(100);
    while (fut.wait_for(span) == future_status::timeout) {
        INFO("*");
    }

    bool ret = fut.get();
    INFO("%lld %s a prime.", a, (ret ? "is" : "is not"));
}


void test_future_6() {
    TICK();

    INFO("wait_until example :");

    //异步调用判断99999999999是不是质数
    auto const& a = 9999999999;
    future<bool> fut = async(is_prime, a);

    INFO("waiting for ready...");

    system_clock::duration dur(200);
    time_point<system_clock, system_clock::duration> t_point(dur);
    //截止到系统时间当前点+dur，如果依然共享状态不是ready，打出timeout
    if (fut.wait_until(t_point) == future_status::timeout) {
        ERR("timeout");
    }

    //get会阻塞等待其他线程设置完共享状态
    bool ret = fut.get();
    INFO("%lld %s a prime.", a, (ret ? "is" : "is not"));
}

void test_future_7() {
    TICK();

    //7.4 std::launch::async、std::launch::deferred
    INFO("with launch::async:");

    future<void> foo = async(launch::async, task_launch, '*', 100);
    future<void> bar = async(launch::async, task_launch, '#', 200);

    //参数1为launch::async时，当调用async时就创建线程。
    // async "get" (wait for foo and bar to be ready)
    sleep_for(seconds(5));
    foo.get();
    bar.get();
    DEBUG("");
    DEBUG("");

    INFO("with launch::deferred:");
    foo = async(launch::deferred, task_launch, '*', 100);
    bar = async(launch::deferred, task_launch, '#', 200);

    // deferred "get"(perform the actual calls)
    //参数1为launch::deferred时，当调用future<T>::get()或wait()时才创建线程。
    sleep_for(seconds(5));
    foo.get();
    bar.get();
    DEBUG("");
}

}//namespace future_test

#endif  //FUTURE_TEST_H

