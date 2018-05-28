///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    promise test. 线程间通信：共享状态（shared state）的例子。
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/25
#pragma once
#ifndef PROMISE_TEST
#define PROMISE_TEST

/*
5. std::promise
*/
//通过std::future获取共享状态的值
void printShareState(future<int> &state) {
    int x = state.get();
    cout << "share state value : " << x << endl;
}

//5.1
promise<int> prom1;
void printShareStateValue() {
    future<int> fut = prom1.get_future();
    int x = fut.get();
    cout << "share state value is " << x << endl;
}

//5.2
void getAnInteger(promise<int> &prom) {
    int x;
    cout << "input an integer : ";
    //产生异常
    cin.exceptions(ios::failbit);
    try {
        cin >> x;
        prom.set_value(x);
    }
    catch (exception &) {
        prom.set_exception(current_exception());
    }
}

void printAnInteger(future<int> &fut) {
    try {
        int x = fut.get();
        cout << "the value of Integer is " << x << endl;
    }
    catch (exception &e) {
        cout << "exception content:{" << e.what() << "}" << endl;
    }
}

int main_promise_test() {
    /*
    5. std::promise
    */
    //创建一个 promise<int> 对象，状态值为int类型
    {
        promise<int> prom;
        //和 future 关联
        future<int> fut = prom.get_future();
        //将 future 交个另外一个线程
        thread t(printShareState, ref(fut));
        // 设置共享状态的值，此处和线程t保持同步
        prom.set_value(10);
        t.join();
    }


    {
        thread t1(printShareStateValue);
        prom1.set_value(10);
        t1.join();
        //promise<int>() 创建一个匿名空的promise对象，使用移动拷贝构造函数给prom1
        prom1 = promise<int>();
        thread t2(printShareStateValue);
        prom1.set_value(20);
        t2.join();
    }

    {
        promise<int> prom;
        future<int> fut = prom.get_future();
        thread t1(getAnInteger, ref(prom));
        thread t2(printAnInteger, ref(fut));
        t1.join();
        t2.join();
    }


    return 0;
}
#endif  //PROMISE_TEST

