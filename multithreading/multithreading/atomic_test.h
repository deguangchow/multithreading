﻿///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    线程间通信：类型为 <T> 的原子变量（std::atomic<T>）
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/30
#pragma once
#ifndef ATOMIC_TEST_H
#define ATOMIC_TEST_H

namespace atomic_test {


//14. std::atomic
// 由 false 初始化一个 std::atomic<bool> 类型的原子变量
atomic<bool> g_atomic_ready_1(false);
atomic_flag g_atomic_flag_winner_1 = ATOMIC_FLAG_INIT;

void task_atomic(int id) {
    TICK();
    // 等待 g_atomic_ready_1 变为 true
    while (!g_atomic_ready_1) {
        yield();
    }

    // 计数
    for (volatile int i = 0; i < 1000000; ++i) {
    }

    if (!g_atomic_flag_winner_1.test_and_set()) {
        DEBUG("thread %d finished firstly!", id);
    }
}

//14.1 std::atomic 赋值运输符 T operator=(T val) volatile noexcept;
atomic<int> g_atomic_val = 0;

void set_atomic_val(int x) {
    TICK();
    g_atomic_val = x;
}

void print_atomic_val() {
    TICK();
    while (g_atomic_val == 0) {
        // wait while g_atomic_val == 0
        // 让当前线程放弃时间片，让其他线程执行
        yield();
    }
    DEBUG("g_atomic_val: %d", g_atomic_val);
}

//14.2 std::atomic::store(), std::atomic::load(), 强制类型转换 opreator T() const
atomic<int> g_atomic_src = 0;
atomic<int> g_atomic_dest = 0;

void store_atomic_val(int x) {
    TICK();
    // 设置（store）原子对象 g_atomic_val 的值。
    sleep_for(seconds(1));
    g_atomic_val.store(x, memory_order_relaxed);
}

void load_atomic_val() {
    TICK();
    int x;
    do {
        // 读取（load）原子对象 g_atomic_val 的值。
        x = g_atomic_val.load(memory_order_relaxed);
    } while (x == 0);
    DEBUG("g_atomic_val: %d", x);
}

void set_src(int x) {
    TICK();
    g_atomic_src = x;
}

void print_dest() {
    TICK();
    while (g_atomic_dest == 0) {
        yield();
    }
    INFO("g_atomic_dest: %d", g_atomic_dest);
}

void copy_src_to_dest() {
    TICK();
    while (g_atomic_src == 0) {
        yield();
    }
    g_atomic_dest = static_cast<int>(g_atomic_src);
}

//14.3 std::atomic::exchange()
atomic<bool> g_atomic_ready_2(false);
atomic<bool> g_atomic_flag_winner_2(false);

void task_atomic_exchange(int id) {
    TICK();
    while (!g_atomic_ready_2) {
        yield();
    }
    for (int i = 0; i < 1000000; ++i) {
    }
    if (!g_atomic_flag_winner_2.exchange(true)) {
        DEBUG("thread %d finished firstly!", id);
    }
}

//14.4 std::atomic::compare_exchange_weak()
// a simple global linked list:
struct Node {
    int value;
    Node *next;
    Node(int v, Node *n) : value(v), next(n) {
    }
};
atomic<Node*> g_list_head(nullptr);

void append(int val) {
    TICK();
    // 头插法插入新节点
    Node *expected_node = g_list_head;
    Node *val_node = new Node(val, expected_node);

    // 比较原子对象中封装的值和 expected_node 是否相等：
    // 相等：用 val_node 替换原子对象中的旧值；
    // 不相等：用原子对象中的旧值替换 expected_node ；

    //bool ret = addr.compare_exchange_weak(old_value, new_value);
    //if ret == true, addr = new_value;
    //if ret == false, old_value = addr;
#if 1
    while (!g_list_head.compare_exchange_weak(expected_node, val_node)) {
        val_node->next = expected_node;
    }
#else
    while (!g_list_head.compare_exchange_strong(expected_node, val_node)) {
        val_node->next = expected_node;
    }
#endif
}


void test_atomic_1() {
    TICK();
    //14. std::atomic
    vector<thread> vctThreads;
    for (int i = 1; i <= 10; ++i) {
        vctThreads.emplace_back(task_atomic, i);
    }

    // 由于 g_atomic_ready_1 是原子的，所以做赋值操作是线程安全的
    g_atomic_ready_1 = true;

    for (auto &pos : vctThreads) {
        pos.join();
    }
}

void test_atomic_2() {
    TICK();
    //14.1 std::atomic 赋值运算符 T operator=(T val)volatile nonexcept;
    thread thread_print(print_atomic_val);
    thread thread_set(set_atomic_val, 10);
    thread_print.join();
    thread_set.join();
}

void test_atomic_3() {
    TICK();
    //14.2 std::atomic::store(), std::atomic::load(), 强制类型转换 opreator T() const
    thread thread_store(store_atomic_val, 10);
    thread thread_load(load_atomic_val);
    thread_store.join();
    thread_load.join();

    INFO("-----------------------------");

    thread thread_dest(print_dest);
    thread thread_src(set_src, 20);
    thread thread_copy(copy_src_to_dest);
    thread_dest.join();
    thread_src.join();
    thread_copy.join();
}

void test_atomic_4() {
    TICK();
    //14.3 std::atomic::exchange()
    vector<thread> vctThreads;
    for (int i = 1; i <= 10; ++i) {
        vctThreads.emplace_back(task_atomic_exchange, i);
    }

    g_atomic_ready_2 = true;

    for (auto &pos : vctThreads) {
        pos.join();
    }
}

void test_atomic_5() {
    TICK();
    //14.4 std::atomic::compare_exchange_weak()
    vector<thread> vctThreads;
    for (int i = 0; i < 10; ++i) {
        vctThreads.emplace_back(append, i);
    }
    for (auto &pos : vctThreads) {
        pos.join();
    }

    // print contents
    for (Node *pos = g_list_head; pos != nullptr; pos = pos->next) {
        INFO("%d", pos->value);
    }
}


}//namespace atomic_test

#endif  //ATOMIC_TEST_H

