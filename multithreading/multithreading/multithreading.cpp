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
#include "thread_guard.h"


void hello() {
    cout << "hello" << endl;
}

/*
1. HelloWorld
*/
void thread_task() {
    cout << "Hello world!" << endl;
}

/*
2. Thread Constructor
*/
void exec_proc1(int n) {
    for (int i = 0; i < 5; ++i) {
        cout << "pass value, executing thread " << n << endl;
        //阻止线程运行到10毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void exec_proc2(int &n) {
    for (int i = 0; i < 5; ++i) {
        cout << "pass reference, executing thread " << n << endl;
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/*
3. 赋值操作
*/
void exec_produce(int duration) {
    //阻止线程运行到duration秒
    std::this_thread::sleep_for(std::chrono::seconds(duration));
    //this_thread::get_id（）获取当前线程id
    cout << "exec_produce thread " << std::this_thread::get_id() << " has sleeped " << duration << " seconds" << endl;
}

/*
4. std::mutex
*/
volatile int counter = 0;
const int MAX_TIMES_VALUE = 10;
const int MAX_THREADS_VALUE = 5;
std::mutex my_mutex;

void my_task() {
    for (int i = 0; i < MAX_TIMES_VALUE; ++i) {
        //尝试获取锁，try_lock()失败时返回false
        if (my_mutex.try_lock()) {
            ++counter;
            my_mutex.unlock();
            cout << "----线程id=" << std::this_thread::get_id() << " counter=" << counter << "\t" << endl;
            //this_thread::sleep_for(chrono::microseconds(100));
        } else {
            cout << "++++线程id=" << std::this_thread::get_id() << " counter=" << counter << "\t" << endl;
        }
    }
}

int main() {
    //LiftOff launch(10);
    //launch.run();


//     try {
//         /*
//         以类的成员函数作为参数
//         为了作为std::thread的构造参数，类的成员函数名必须唯一。
//         在下例中，如果world1()和world2()函数名都是world，则编译出错，这是因为名字解析发生在参数匹配之前。
//         */
//         LiftOff l = LiftOff(10);
//         thread t(&LiftOff::run, &l);
//         //thread t0(&LiftOff::run, &l);
//
//         //以lambda对象作为参数
//         thread t1([](const string &text) {cout << "hello world, " << text << endl; }, "deguangchow");
//
//         t.join();
//         //t0.join();  //未考虑线程安全问题
//         t1.join();
//         //t1.join();  //runtime error
//         if (t1.joinable()) {
//             t1.join();
//         }
//
//         cout << "Waiting for LiftOff" << endl;
//     } catch (exception e) {
//         cout << e.what() << endl;
//     }

    //try {
    //    func();
    //} catch (const exception &e) {
    //    cout << e.what() << endl;
    //}

    //auto lambda_fun = [](string text)->void {
    //    cout << "hello world, " << text << endl;
    //};
    //std::thread t(lambda_fun, "lee");
    //if (t.joinable()) {
    //    t.detach();
    //}
    //assert(!t.joinable());


    /*
    1.Hello World
    */
//     thread t(thread_task);
//     t.join();
//     system("pause");

    /*
    2. Thread Constructor
    1）默认构造函数：thread() noexcept，创建一个空的 thread 执行对象。
    2）初始化构造函数： template
    */
//     int n = 0;
//     // t1，使用默认构造函数，什么都没做
//     std::thread t1;
//     // t2，使用有参构造函数，传入函数名称（地址）exec_pro1，并以传值的方式传入args
//     // 将会执行exec_proc1中的代码
//     std::thread t2(exec_proc1, n + 1);
//     // t3，使用有参构造函数，传入函数名称（地址）exec_pro2，并以传引用的方式传入args
//     // 将会执行exec_proc2中的代码
//     std::thread t3(exec_proc2, std::ref(n));
//     // t4，使用移动构造函数，由t4接管t3的任务，t3不再是线程了
//     std::thread t4(std::move(t3));
//     // 可被 joinable 的 thread 对象必须在他们销毁之前被主线程 join 或者将其设置为 detached.
//     t2.join();
//     t4.join();
//     cout << "the result of n is " << n << endl;
//     system("pause");


    /*
    3. 赋值操作
    */
//     std::thread threads[5];
//     cout << "create 5 threads" << endl;
//     for (int i = 0; i < 5; ++i) {
//         threads[i] = std::thread(exec_produce, i + 1);
//     }
//     cout << "finished creating 5 threads, and waiting for joining" << endl;
//     for (auto &pos : threads) {
//         pos.join();
//     }
//     cout << "Finished!!!" << endl;

    /*
    4. std::mutex
    */
    std::thread threads[MAX_THREADS_VALUE];
    for (int i = 0; i < MAX_THREADS_VALUE; ++i) {
        threads[i] = std::thread(my_task);
    }
    for (auto &pos : threads) {
        pos.join();
    }
    cout << "Finished : the result of counter is " << counter << endl;




    system("pause");
    return 0;
}

