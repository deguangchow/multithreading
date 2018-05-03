/*
copyright:	deguangchow@qq.com
data:		2018/05/03
autor:		deguangchow
brief:		火箭发射倒计时类
*/
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
    LiftOff(int count, int ident = 0) :countDown(count), id(ident) {
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
#endif  //LIFTOFF_H

