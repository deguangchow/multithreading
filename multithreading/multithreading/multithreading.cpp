/*
copyright:	deguangchow@qq.com
data:		2018/05/03
autor:		deguangchow
brief:		定义控制台应用程序的入口点
*/
// multithreading.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LiftOff.h"

void hello(){
    cout << "hello" << endl;
}
int main() {

    //LiftOff launch(10);
    //launch.run();


    try {
        /*
        以类的成员函数作为参数
        为了作为std::thread的构造参数，类的成员函数名必须唯一。
        在下例中，如果world1()和world2()函数名都是world，则编译出错，这是因为名字解析发生在参数匹配之前。
        */
        LiftOff l = LiftOff(10);
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
    } catch (exception e) {
        cout << e.what() << endl;
    }



    return 0;
}

