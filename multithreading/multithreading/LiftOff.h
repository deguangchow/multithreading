///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    火箭发射倒计时类
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/28
#pragma once
#ifndef LIFTOFF_H
#define LIFTOFF_H

class Runnable {
public:
    virtual void run() = 0;
    ~Runnable() {
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
            //Sleep(1000);
        }
        cout << "LiftOff!" << endl;
    }
};

int main_lift_off() {
    {
        LiftOff launch(10, 1);
        launch.run();
    }

    {
        try {
            /*
            以类的成员函数作为参数
            为了作为thread的构造参数，类的成员函数名必须唯一。
            在下例中，如果world1()和world2()函数名都是world，则编译出错，这是因为名字解析发生在参数匹配之前。
            */
            LiftOff l = LiftOff(10, 2);
            thread t(&LiftOff::run, &l);
            //thread t0(&LiftOff::run, &l);
            //以lambda对象作为参数
            thread t1([](const string &text) {cout << "hello world, " << text << endl; }, "deguangchow");
            t.join();
            //t0.join();  //未考虑线程安全问题
            t1.join();
            //t1.join();  //runtime error
            if (t1.joinable()) {
                t1.join();
            }
            cout << "Waiting for LiftOff" << endl;
        }
        catch (exception e) {
            cout << e.what() << endl;
        }
    }

    return 0;
}

#endif  //LIFTOFF_H

