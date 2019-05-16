///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    线程间通信：条件变量
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/29
#pragma once
#ifndef CONDITION_VARIABLE_TEST_H
#define CONDITION_VARIABLE_TEST_H

namespace condition_variable_test {


/*
11. std::condition_variable
*/
//全局变量
mutex g_mtx;
//全局条件变量
condition_variable g_cv;
//全局标志位
bool g_ready = false;

void do_print_id(int id) {
    TICK();
    unique_lock<mutex> lck(g_mtx);

    // 如果标志位不为 true，则等待……
    // 当前线程被阻塞，当全局标志位变为 true 之后，
    while (!g_ready) {
        g_cv.wait(lck);
    }

    // 线程被唤醒，继续往下执行打印线程编号 id.
    DEBUG("thread %5d", id);
}

void go() {
    TICK();
    {
        unique_lock<mutex> lck(g_mtx);

        // 设置全局标志位为 true.
        g_ready = true;
    }

    // 唤醒所有线程.
    g_cv.notify_all();
}

//11.1 std::condition_variable::wait, notify_one
int g_cargo = 0;    //全局变量：货物
// 可装运
bool shipment_available() {
    return g_cargo != 0;
}

// 消费者线程
void consume(int n) {
    TICK();
    for (int i = 0; i < n; ++i) {
        unique_lock<mutex> lck(g_mtx);
        // 只有当 shipment_available() 返回为 false 时调用 wait() 才会阻塞当前线程，
        // 并且在收到其他线程的通知后只有当 shipment_available() 返回 true 时才会被解除阻塞
        g_cv.wait(lck, shipment_available);
        DEBUG("consuming no.%d cargo", g_cargo);
        g_cargo = 0;
    }
}

//11.2 std::condition_variable::wait_for()
int g_value_input;

void task_input() {
    TICK();
    cin >> g_value_input;
    g_cv.notify_one();
}

//11.3 std::condition_variable::wait_until()
int cargo = 0;
void consumer() {
    TICK();
    unique_lock<mutex> lck(g_mtx);
#if 0
    while (cargo == 0) {
        system_clock::duration dur(200);
        time_point<system_clock, system_clock::duration> t_point(dur);
        g_cv.wait_until(lck, t_point);
    }
#else
    g_cv.wait(lck);
#endif
    DEBUG("consuming no.%d cargo.", cargo);
    cargo = 0;
}

void producer(int id) {
    TICK();
    unique_lock<mutex> lck(g_mtx);
    cargo = id;
    INFO("producing no.%d cargo.", cargo);
    g_cv.notify_one();
}

//11.4 std::notify_all_at_thread_exit(condition_variable &cv, unique_lock<T> lck)
void task_output(int id) {
    TICK();
    unique_lock<mutex> lck(g_mtx);
    while (!g_ready) {
        g_cv.wait(lck);
    }
    DEBUG("thread id: %d", id);
}

void go_exit() {
    TICK();
    unique_lock<mutex> lck(g_mtx);
    //g_cv.notify_all();
    notify_all_at_thread_exit(g_cv, move(lck));
    g_ready = true;
}


void test_condition_variable_1() {
    TICK();
    //11. std::condition_variable

    // 创建10个线程，由于全局标志ready为false，
    // 所以所有线程创建完成后均会从运行状态转为阻塞态
    thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = thread(do_print_id, i);
    }

    // 修改 ready 标志，并通知所有线程可以从阻塞态转为运行态
    // 但显然10个线程也是在某一刻只能运行一个
    INFO("wait 1 second……");
    sleep_for(seconds(1));
    go();

    for (auto &pos : threads) {
        pos.join();
    }
}

void test_condition_variable_2() {
    TICK();

    //11.1 std::condition_variable::wait, notify_one
    thread consumer_thread(consume, 10);    //消费者线程

                                            // 主线程为生产者线程，生产 10 个物品
    for (int i = 0; i < 10; ++i) {
        while (shipment_available()) {
            // yield() 函数可以用来将调用者线程跳出运行状态，重新交给操作系统进行调度
            yield();
        }
        unique_lock<mutex> lck(g_mtx);
        g_cargo = i + 1;
        INFO("producing no.%d cargo", g_cargo);

        g_cv.notify_one();
    }

    consumer_thread.join();
}

void test_condition_variable_3() {
    TICK();

    //11.2 std::condition_variable::wait_for()
    // 创建一个子线程执行 task_input
    thread th(task_input);

    mutex mtx;
    unique_lock<mutex> lck(mtx);
    while (g_cv.wait_for(lck, seconds(1)) == cv_status::timeout) {
        DEBUG("Looping");
        cout.flush();
    }

    INFO("the entered integer is : %d", g_value_input);

    th.join();
}

void test_condition_variable_4() {
    TICK();

    //11.3 std::condition_variable::wait_until()
    thread consumers[10], producers[10];

    // create 10 consumers and 10 producers
    for (int i = 0; i < 10; ++i) {
        consumers[i] = thread(consumer);
    }
    for (int i = 0; i < 10; ++i) {
        producers[i] = thread(producer, i + 1);
    }

    for (int i = 0; i < 10; ++i) {
        consumers[i].join();
    }
    for (int i = 0; i < 10; ++i) {
        producers[i].join();
    }
}

void test_condition_variable_5() {
    TICK();

    //11.4 std::notify_all_at_thread_exit(condition_variable &cv, unique_lock<T> lck)
    thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = thread(task_output, i);
    }

    // 创建子线程执行 go_exit()，并运行完成之后自己退出
    thread(go_exit).detach();

    for (auto &pos : threads) {
        pos.join();
    }
}

}//namespace condition_variable_test

#endif  //CONDITION_VARIABLE_TEST_H
