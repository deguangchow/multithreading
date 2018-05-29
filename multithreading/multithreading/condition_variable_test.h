///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    线程间通信：条件变量
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/29
#pragma once
#ifndef CONDITION_VARIABLE_TEST
#define CONDITION_VARIABLE_TEST

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
    unique_lock<mutex> lck(g_mtx);
    // 如果标志位不为 true，则等待……
    // 当前线程被阻塞，当全局标志位变为 true 之后，
    // 线程被唤醒，继续往下执行打印线程编号 id.
    while (!g_ready) {
        g_cv.wait(lck);
    }
    cout << "thread " << id << endl;
}

void go() {
    unique_lock<mutex> lck(g_mtx);
    // 设置全局标志位为 true.
    g_ready = true;
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
    for (int i = 0; i < n; ++i) {
        unique_lock<mutex> lck(g_mtx);
        // 只有当 shipment_available() 返回为 false 时调用 wait() 才会阻塞当前线程，
        // 并且在收到其他线程的通知后只有当 shipment_available() 返回 true 时才会被解除阻塞
        g_cv.wait(lck, shipment_available);
        cout << g_cargo << endl;
        g_cargo = 0;
    }
}

//11.2 std::condition_variable::wait_for()
int g_value_input;

void task_input() {
    cin >> g_value_input;
    g_cv.notify_one();
}

int main_condition_variable() {
    {   //11. std::condition_variable
        thread threads[10];
        // 创建10个线程，由于全局标志ready为false，
        // 所以所有线程创建完成后均会从运行状态转为阻塞态
        for (int i = 0; i < 10; ++i) {
            threads[i] = thread(do_print_id, i);
        }

        // 修改 ready 标志，并通知所有线程可以从阻塞态转为运行态
        // 但显然10个线程也是在某一刻只能运行一个
        cout << "wait 1 second……" << endl;
        sleep_for(seconds(1));
        go();

        for (auto &pos : threads) {
            pos.join();
        }
    }
    {   //11.1 std::condition_variable::wait, notify_one
        thread consumer_thread(consume, 10);    //消费者线程

        // 主线程为生产者线程，生产 10 个物品
        for (int i = 0; i < 10; ++i) {
            while (shipment_available()) {
                // yield() 函数可以用来将调用者线程跳出运行状态，重新交给操作系统进行调度
                yield();
            }
            unique_lock<mutex> lck(g_mtx);
            g_cargo = i + 1;
            g_cv.notify_one();
        }

        consumer_thread.join();
    }
    {   //11.2 std::condition_variable::wait_for()
        // 创建一个子线程执行 task_input
        thread th(task_input);
        mutex mtx;
        unique_lock<mutex> lck(mtx);
        while (g_cv.wait_for(lck, seconds(1)) == cv_status::timeout) {
            cout << '.';
            cout.flush();
        }

        cout << "the entered integer is : " << g_value_input << endl;

        th.join();
    }
    return 0;
}


#endif  //CONDITION_VARIABLE_TEST
