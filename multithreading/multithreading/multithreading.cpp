/*
copyright:	deguangchow@qq.com
data:		2018/05/03
autor:		deguangchow
brief:		�������̨Ӧ�ó������ڵ�
*/
// multithreading.cpp : �������̨Ӧ�ó������ڵ㡣
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
        ����ĳ�Ա������Ϊ����
        Ϊ����Ϊstd::thread�Ĺ����������ĳ�Ա����������Ψһ��
        �������У����world1()��world2()����������world����������������Ϊ���ֽ��������ڲ���ƥ��֮ǰ��
        */
        LiftOff l = LiftOff(10);
        thread t(&LiftOff::run, &l);
        //thread t0(&LiftOff::run, &l);

        //��lambda������Ϊ����
        thread t1([](const string &text) {cout << "hello world, " << text << endl; }, "deguangchow");

        t.join();
        //t0.join();  //δ�����̰߳�ȫ����
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

