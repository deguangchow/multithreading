///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    线程间通信：原子操作（std::atomic_flag）
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/30
#pragma once
#ifndef ATOMIC_FLAG_TEST_H
#define ATOMIC_FLAG_TEST_H

//13. std::atomic_flag
atomic<bool> g_atomic_ready = false;     //can be checked without being set
atomic_flag g_atomic_flag_winner = ATOMIC_FLAG_INIT;  //always set when checked

void count_task(int id) {
    // 等待主线程中设置 g_ready_atomic 为 true
    while (!g_atomic_ready) {
        yield();
    }
    // 计数
    for (int i = 0; i < 1000000; ++i) {
    }

    // 如果某个线程率先执行网上面的计数过程，则输出自己的 ID.
    // 此后其他线程执行 test_and_set 是 if 语句判断为 false，
    // 因此不会输出自身 ID.
    if (!g_atomic_flag_winner.test_and_set()) {
        cout << "first finished thread is " << id << endl;
    }
}


//13.1 std::atomic_flag::test_and_set()
//ATOMIC_FLAG_INIT: 如果某个 atomic_flag 对象使用该宏初始化，那么可以保证该 atomic_flag 对象在创建时处于 clear 状态
atomic_flag g_atomic_flag_lock_stream = ATOMIC_FLAG_INIT;
stringstream g_stream;

void append_number(int x) {
    //test_and_set() 函数检查 std::atomic_flag 标志
    //如果 std::atomic_flag 之前没有被设置过，则设置 std::atomic_flag 的标志
    //如果之前 std::atomic_flag 对象已被设置，则返回 true，否则返回 false。
    // 设位
    while (g_atomic_flag_lock_stream.test_and_set()) {
        cout << static_cast<char>('a' + x);
    }

    cout << "thread #" << x;
    sleep_for(milliseconds(10));

    g_stream << "thread #" << x << '\n';
    // 清除位
    g_atomic_flag_lock_stream.clear();
}

//13.2 std::atomic_flag::clear()
//清除 std::atomic_flag 标志使得下一次调用 std::atomic_flag::test_and_set 返回 false.
atomic_flag g_atomic_flag_lock = ATOMIC_FLAG_INIT;

void task_atomic_flag_clear(int id) {
    for (int i = 0; i < 2; ++i) {
        while (g_atomic_flag_lock.test_and_set(memory_order_acquire)) {
            cout << static_cast<char>('a' + id);
        }

        cout << "Output from thread " << id << endl;
        sleep_for(milliseconds(10));

        g_atomic_flag_lock.clear(memory_order_release);
    }
}


int main_atomic_flag() {
#if 0
    {   //13. std::atomic_flag
        vector<thread> vctThreads;
        for (int i = 1; i <= 10; ++i) {
            vctThreads.push_back(thread(count_task, i));
        }

        g_atomic_ready = true;

        for (auto &pos : vctThreads) {
            pos.join();
        }
    }
    {   //13.1 std::atomic_flag::test_and_set()
        vector<thread> vctThreads;
        for (int i = 1; i <= 10; ++i) {
            vctThreads.push_back(thread(append_number, i));
        }
        for (auto &pos : vctThreads) {
            pos.join();
        }

        cout << g_stream.str() << endl;
    }
#endif
    {   //13.2 std::atomic_flag::clear()
        vector<thread> vctThreads;
        for (int i = 0; i < 5; ++i) {
            //emplace_back 参数转换为<T>的构造函数中的参数
            vctThreads.emplace_back(task_atomic_flag_clear, i);
        }
        for (auto &pos : vctThreads) {
            pos.join();
        }
    }

    return 0;
}

#endif  //ATOMIC_FLAG_TEST_H
