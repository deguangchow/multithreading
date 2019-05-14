///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    thread_test
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/28
#pragma once
#ifndef THREAD_TEST_H
#define THREAD_TEST_H

namespace thread_test {


class Runnable {
public:
    virtual void run() = 0;
    virtual ~Runnable() {
    }
};

class LiftOff : public Runnable {
    int countDown;
    int id;
public:
    explicit LiftOff(int count, int ident = 0) :countDown(count), id(ident) {
    }
    ~LiftOff() {
        cout << id << " completed" << endl;
    }
    void run() {
        while (--countDown) {
            cout << id << ":" << countDown << endl;
            Sleep(1000);
        }
        cout << "LiftOff!" << endl;
    }
};

void test_lift_off() {
    LiftOff launch(10, 1);
    launch.run();
}

void test_lift_off_thread() {
    /*
    以类的成员函数作为参数
    为了作为thread的构造参数，类的成员函数名必须唯一。
    在下例中，如果world1()和world2()函数名都是world，则编译出错，这是因为名字解析发生在参数匹配之前。
    */
    LiftOff l = LiftOff(10, 2);
    cout << "Waiting for LiftOff" << endl;

    thread t(&LiftOff::run, &l);
    thread t0(&LiftOff::run, &l);
    t.join();
    t0.join();  //未考虑线程安全问题
}

void test_thread_join() {
    try {
        //以lambda对象作为参数
        thread t1([](const string &text) {
            cout << "hello world, " << text << endl;
        }, "deguangchow");

        t1.join();
        t1.join();  //runtime error
        if (t1.joinable()) {
            t1.join();
        }
    } catch (exception e) {
        cout << e.what() << endl;
    }
}

void test_thread_detach() {
    thread t([](string const& text)->void {
        cout << "hello world, " << text << endl;
    }, "lee");
    if (t.joinable()) {
        t.detach();
    }
    assert(!t.joinable());
}

}//namespace thread_test

#endif  //THREAD_TEST_H

