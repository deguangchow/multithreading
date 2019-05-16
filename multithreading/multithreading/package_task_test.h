///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    std::packaged_task. 线程间通信：共享状态（shared state）。
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/25
#pragma once
#ifndef PACKAGE_TASK_TEST_H
#define PACKAGE_TASK_TEST_H

namespace package_task_test {


/*
6. std::packaged_task
*/
int my_task_packaged(int from, int to) {
    TICK();
    for (int i = from; i != to; --i) {
        DEBUG("%d", i);
        sleep_for(seconds(1));
    }
    INFO("Finished!!");
    return from - to;
}

//6.1 std::packaged_task::valid()
//在新线程中启动一个 int(int) packaged_task.
future<int> launcher(packaged_task<int(int)> &&p_task, int arg) {
    TICK();
    if (p_task.valid()) {
        future<int> fut = p_task.get_future();
        //创建匿名线程，线程运行结束后直接退出不需要和主线程会合
        thread(move(p_task), arg).detach();
        return fut;
    } else {
        return future<int>();
    }
}

//6.2 std::packaged_task::reset
int tripleX(int x) {
    TICK();
    return x * 3;
}

void test_packaged_task_1() {
    TICK();
    /*
    6. std::package_task
    */

    //设置 package_task，形式上类似于std::function
    //如 std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
    packaged_task<int(int, int)> p_task(my_task_packaged);

    //获取与 package_task 共享状态相关联的 future 对象
    future<int> ret = p_task.get_future();

    thread th(move(p_task), 10, 0);  //创建一个线程完成计数任务
    int value = ret.get();

    INFO("the result of the future stage value is %d)", value);
    th.join();
}

void test_packaged_task_2() {
    TICK();

    //packaged_task<T(Args...)> 的函数对象只能被执行一次
    packaged_task<int(int, int)> p_task([](int const& a, int const& b)->int {
        return a + b;
    });

    future<int> fut = p_task.get_future();
    int a = 1;
    int b = 2;
    thread t1(move(p_task), a, b);
    t1.join();

    //future<T>::get() 方法对应 packaged_task<T>
    int sum = fut.get();//阻塞等待 packaged_task 的对象被调用

    INFO("sum(%d, %d)=%d", a, b, sum);

    //////////////////////////////////////////////////////////////////////////

    //p_task 的内存对象已经被 move 到线程 t1 的构造参数中，p_task 被清空，需要重新分配内存
    p_task = packaged_task<int(int, int)>([](int const& a, int const& b)->int {
        return a - b;
    });

    future<int> fut1 = p_task.get_future();
    p_task(a, b);
    int sub = fut1.get();

    INFO("sub(%d, %d)=%d", a, b, sub);
}

void test_packaged_task_3() {
    TICK();
    /*
    6.1
    */
    //默认构造函数
    packaged_task<int(int)> p_task_default;

    //使用 lambda 表达式初始化一个 package_task 对象
    packaged_task<int(int)> p_task_lambda([](int x) {
        return x * 3;
    });

    p_task_default = move(p_task_lambda);

    //获取与 package_task 共享状态相关联的 future 对象
    future<int> fut = p_task_default.get_future();

    thread(move(p_task_default), 30).detach(); //产生线程，调用被包装的任务

    int value = fut.get();
    INFO("The final result is %d", value);
}

void test_packaged_task_4() {
    TICK();
    /*
    6.1 packaged_task::valid()
    */
    packaged_task<int(int)> p_task([](int x) {
        return x * 2;
    });

    future<int> fut_ret = launcher(move(p_task), 25);

    if (fut_ret.get() == 50) {
        INFO("task is valid refrence to a shared state");
    } else {
        INFO("do notiong");
    }
}

void test_packaged_task_5() {
    TICK();
#if 0
    packaged_task<int(int)> p_task([](int a) {
        DEBUG("RAII");
        return ++a;
    });
#else
    packaged_task<int(int)> p_task;
#endif


    if (p_task.valid()) {
        INFO("p_task has been initialized, p_task is valid");

        future<int> f_res = p_task.get_future();
        int a = 1;
        p_task(a);

        auto const& res = f_res.get();
        INFO("(++%d)= %d", a, res);
    } else {
        INFO("p_task has not been initialized, p_task is invalid");
        INFO("initializing p_task now: ");

        p_task = packaged_task<int(int)>([](int a) {
            DEBUG("not RAII");
            return ++a;
        });

        future<int> f_res = p_task.get_future();

        int a = 2;
        p_task(a);

        auto const& res = f_res.get();
        INFO("(++%d)= %d", a, res);
    }
}

void test_packaged_task_6() {
    TICK();

    //6.2 std::packaged_task::reset
    packaged_task<int(int)> p_task(tripleX);
    future<int> fut_ret = p_task.get_future();
    p_task(100);
    INFO("The tiple of 100 is %d", fut_ret.get());

    //packaged_task<T> 函数对象只能执行一次，也只能调用成员函数 get_future() 一次;
    //如要再次执行或调用，需要执行成员函数 reset().
    p_task.reset();
    fut_ret = p_task.get_future();
    thread(move(p_task), 200).detach();
    INFO("The tiple of 200 is %d", fut_ret.get());
}


}//namespace package_task_test

#endif  //PACKAGE_TASK_TEST_H

