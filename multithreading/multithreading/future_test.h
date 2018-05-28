///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    std::future
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/28
#pragma once
#ifndef FUTURE_TEST
#define FUTURE_TEST

/*
7.std::future
*/
//是否为质数
bool is_prime(int x) {
    for (int i = 2; i < x; ++i) {
        if (x%i == 0) {
            return false;
        }
    }
    return true;
}

//7.1 std::future::share
int do_get_value() {
    return 10;
}

//7.2 std::future::valid
int simple_task() {
    cout << "do task" << endl;
    return 1;
}

//7.3 std::future::wait
// 为了体现效果，该函数故意没有优化
bool do_task(int x) {
    for (int i = 2; i < x; ++i) {
        int a = x / i;
        a = x%i;
    }
    return x % 2;
}

int main_future_test() {
    {   //7.std::future
        //异步调用判断999999是不是质数
        future<bool> fut = async(is_prime, 9999999999);

        //由于上面代码中判断素数的方法对于比较大的数，将耗费较多时间
        //开始判断共享状态是否变为了ready，等待
        cout << "waiting for ready..." << endl;
        std::chrono::milliseconds span(100);
        //每100ms内阻塞等待，如果超时输出*，并继续等待，否则结束while
        while (fut.wait_for(span) == future_status::timeout) {
            cout << "*";
        }

        bool ret = fut.get();
        cout << endl;
        cout << "9999999999 " << (ret ? "is" : "is not") << " a prime." << endl;
    }
    {   //7.1 std::future::share
        future<int> fut = async(do_get_value);
        shared_future<int> shared_fut = fut.share();
        //共享的future 对象可以被多次访问，fut的状态到这里便不再是valid的了
        cout << "value: " << shared_fut.get() << endl;
        cout << "its double: " << shared_fut.get() * 2 << endl;
    }
    {   //7.2 std::future::valid
        //由默认构造函数创建的 future 对象
        //初始化时该 future 读写的共享状态处于为 invalid 状态。
        future<int> foo, bar;
        //move 赋值，foo 的共享状态变为 valid
        foo = async(simple_task);
        //move 赋值，bar 的共享状态变为 valid，而 move 赋值以后 foo 的共享状态变为 invalid。
        bar = move(foo);
        if (foo.valid()) {
            cout << "foo`s value: " << foo.get() << endl;
        } else {
            cout << "foo is not valid" << endl;
        }
        if (bar.valid()) {
            cout << "bar`s value: " << bar.get() << endl;
        } else {
            cout << "bar is not valid" << endl;
        }
    }
    {   //7.3 std::future::wait
        //异步调用do_task()
        future<bool> fut = async(do_task, 99999999);
        cout << "Waiting..." << endl;
        fut.wait();

        if (fut.get()) {
            cout << "return true" << endl;
        } else {
            cout << "return false" << endl;
        }
    }
    {   //7.4 std::future::wait_for
        cout << "wait_for example :" << endl;
        //异步调用判断99999999999是不是质数
        future<bool> fut = async(is_prime, 9999999999);
        std::chrono::milliseconds span(100);
        while (fut.wait_for(span) == future_status::timeout) {
            cout << "*";
        }
        bool ret = fut.get();
        cout << endl;
        cout << "9999999999 " << (ret ? "is" : "is not") << " a prime." << endl;

        cout << "wait_until example :" << endl;
        fut = async(is_prime, 9999999999);
        cout << "waiting for ready..." << endl;
        std::chrono::system_clock::duration dur(200);
        std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t_point(dur);
        //截止到系统时间当前点+dur，如果依然共享状态不是ready，打出timeout
        if (fut.wait_until(t_point) == future_status::timeout) {
            cout << "timeout" << endl;
        }
        //get会阻塞等待其他线程设置完共享状态
        ret = fut.get();
        cout << endl;
        cout << "9999999999 " << (ret ? "is" : "is not") << " a prime." << endl;
    }

    return 0;
}

#endif  //FUTURE_TEST

