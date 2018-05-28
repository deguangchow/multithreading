///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    线程守护
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/07
#pragma once
#ifndef THREAD_GUARD_H
#define THREAD_GUARD_H

//线程守护
class thread_guard {
    std::thread t;
public:
    explicit thread_guard(std::thread &_t) : t(move(_t)) {
        if (!t.joinable()) {
            throw logic_error("No Thread");
        }
    }
    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

void func() {
    auto lambda_fun = [](string text)->void {
        cout << "hello world, " << text << endl;
    };

    thread_guard guard(std::thread(lambda_fun));

    try {
        throw exception("test");
    }
    catch (...) {
        throw;
    }
}

int main_thread_guard() {
    try {
        func();
    } catch (const exception &e) {
        cout << e.what() << endl;
    }

    auto lambda_fun = [](string text)->void {
        cout << "hello world, " << text << endl;
    };
    thread t(lambda_fun, "lee");
    if (t.joinable()) {
        t.detach();
    }
    assert(!t.joinable());


    return 0;
}

#endif

