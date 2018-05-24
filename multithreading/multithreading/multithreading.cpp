/*
copyright:	deguangchow@qq.com
data:		2018/05/03
autor:		deguangchow
brief:		定义控制台应用程序的入口点
url:        https://blog.csdn.net/phiall/article/details/52385165
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
    //std::this_thread::get_id（）获取当前线程id
    cout << "exec_produce thread " << std::this_thread::get_id() << " has sleeped " << duration << " seconds" << endl;
}

/*
4. mutex
*/
volatile int counter = 0;
const int MAX_TIMES_VALUE = 10;
const int MAX_THREADS_VALUE = 5;
mutex mutex1;
void my_task() {
    for (int i = 0; i < MAX_TIMES_VALUE; ++i) {
        //尝试获取锁，try_lock()失败时返回false
        if (mutex1.try_lock()) {
            ++counter;
            mutex1.unlock();
            cout << "++++ thread(" << std::this_thread::get_id() << ") counter=" << counter << "\t" << endl;
        } else {
            cout << "0000 thread(" << std::this_thread::get_id() << ") counter=" << counter << "\t" << endl;
        }
    }
}

/*
4.1 timed_mutex
*/
timed_mutex t_mutex;
void my_task_time(int val, char tag) {
    //每200ms尝试获取锁，如果获取到跳出while循环，否则输出一次线程编号
    //比如0-200ms，在200ms之前如果获取不到锁，则线程阻塞，时间到了200ms如果取得了锁，
    //则加锁，否则返回false
    while (!t_mutex.try_lock_for(std::chrono::milliseconds(200))) {
        cout << val;
    }
    //成功取得锁，然后将线程sleep到1000ms
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    cout << tag << endl;
    t_mutex.unlock();
}

/*
4.2 lock_gurad
*/
void print(int x) {
    cout << "value is " << x;
    cout << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

mutex mutex_lock_gurad;
void my_task_lock_gurad(int id) {
    //lock_guard创建局部变量my_lock，会在lock_guard的构造方法中对my_mutex加锁
    lock_guard<mutex> l_guard(mutex_lock_gurad);
    //由于自解锁的作用，下面的代码相当于临界区，执行过程不会被打断
    print(id);
    //运行结束时会析构my_lock，然后在析构函数中对my_mutex解锁
}

/*
4.3 unique_lock
*/
mutex mutex_unique_lock;
void my_task_unique_lock(int id) {
    unique_lock<mutex> u_lock(mutex_unique_lock);
    print(id);
}

mutex mutex_unique_lock1;
//使用含两个参数的构造函数
void my_task_unique_lock_2(int n, char c) {
    unique_lock<mutex> u_lock(mutex_unique_lock1, defer_lock);
    u_lock.lock();
    for (int i = 0; i < n; ++i) {
        cout << c;
    }
    cout << endl;
    //会自动unlock
}

mutex mutex_unique_lock2;
unique_lock<mutex> prepare_task() {
    unique_lock<mutex> u_lock(mutex_unique_lock2);
    cout << "print prepare data" << endl;
    //返回对mutex_unique_lock2的所有权，尚未解锁
    return u_lock;
}

void finish_task(int v) {
    //取得prepare_task创建的锁所有权
    unique_lock<mutex> u_lock(prepare_task());
    cout << "finished :" << v << endl;
    //析构，解锁
}

/*
4.4 once_flag
*/
int value;
once_flag value_once_flag;
void setValue(int x) {
    value = x;
}

void my_task_once_flag(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //使setValue函数只被第一次执行的线程执行
    call_once(value_once_flag, setValue, id);
}


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
    } catch (exception &) {
        prom.set_exception(current_exception());
    }
}

void printAnInteger(future<int> &fut) {
    try {
        int x = fut.get();
        cout << "the value of Integer is " << x << endl;
    } catch (exception &e) {
        cout << "exception content:{" << e.what() << "}" << endl;
    }
}

/*
6. std::packaged_task
*/
int my_task_packaged(int from, int to) {
    for (int i = from; i != to; --i) {
        cout << i << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    cout << "Finished!!" << endl;
    return from - to;
}



int main() {
    //LiftOff launch(10);
    //launch.run();


    //try {
    //    /*
    //    以类的成员函数作为参数
    //    为了作为thread的构造参数，类的成员函数名必须唯一。
    //    在下例中，如果world1()和world2()函数名都是world，则编译出错，这是因为名字解析发生在参数匹配之前。
    //    */
    //    LiftOff l = LiftOff(10);
    //    thread t(&LiftOff::run, &l);
    //    //thread t0(&LiftOff::run, &l);
    //    //以lambda对象作为参数
    //    thread t1([](const string &text) {cout << "hello world, " << text << endl; }, "deguangchow");
    //    t.join();
    //    //t0.join();  //未考虑线程安全问题
    //    t1.join();
    //    //t1.join();  //runtime error
    //    if (t1.joinable()) {
    //        t1.join();
    //    }
    //    cout << "Waiting for LiftOff" << endl;
    //}
    //catch (exception e) {
    //    cout << e.what() << endl;
    //}
    //try {
    //    func();
    //}
    //catch (const exception &e) {
    //    cout << e.what() << endl;
    //}
    //auto lambda_fun = [](string text)->void {
    //    cout << "hello world, " << text << endl;
    //};
    //thread t(lambda_fun, "lee");
    //if (t.joinable()) {
    //    t.detach();
    //}
    //assert(!t.joinable());


    /*
    1.Hello World
    */
    //thread t(thread_task);
    //t.join();
    //system("pause");

    /*
    2. Thread Constructor
    1）默认构造函数：thread() noexcept，创建一个空的 thread 执行对象。
    2）初始化构造函数： template
    */
    //int n = 0;
    //// t1，使用默认构造函数，什么都没做
    //thread t1;
    //// t2，使用有参构造函数，传入函数名称（地址）exec_pro1，并以传值的方式传入args
    //// 将会执行exec_proc1中的代码
    //thread t2(exec_proc1, n + 1);
    //// t3，使用有参构造函数，传入函数名称（地址）exec_pro2，并以传引用的方式传入args
    //// 将会执行exec_proc2中的代码
    //thread t3(exec_proc2, ref(n));
    //// t4，使用移动构造函数，由t4接管t3的任务，t3不再是线程了
    //thread t4(move(t3));
    //// 可被 joinable 的 thread 对象必须在他们销毁之前被主线程 join 或者将其设置为 detached.
    //t2.join();
    //t4.join();
    //cout << "the result of n is " << n << endl;
    //system("pause");


    /*
    3. 赋值操作
    */
    //thread threads[5];
    //cout << "create 5 threads" << endl;
    //for (int i = 0; i < 5; ++i) {
    //    threads[i] = thread(exec_produce, i + 1);
    //}
    //cout << "finished creating 5 threads, and waiting for joining" << endl;
    //for (auto &pos : threads) {
    //    pos.join();
    //}
    //cout << "Finished!!!" << endl;

    /*
    4. mutex
    */
    //thread threads[MAX_THREADS_VALUE];
    //for (int i = 0; i < MAX_THREADS_VALUE; ++i) {
    //    threads[i] = thread(my_task);
    //}
    //for (auto &pos : threads) {
    //    pos.join();
    //}
    //cout << "Finished : the result of counter is " << counter << endl;

    //thread threads[10];
    //char end_tag[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')' };
    //for (int i = 0; i < 10; ++i) {
    //    threads[i] = thread(my_task_time, i, end_tag[i]);
    //}
    //for (auto &pos : threads) {
    //    pos.join();
    //}


    //thread threads[10];
    //for (int i = 0; i < 10; ++i) {
    //    threads[i] = thread(my_task_lock_gurad, i+1);
    //}
    //for (auto &pos : threads) {
    //    pos.join();
    //}

    //thread threads[10];
    //for (int i = 0; i < 10; ++i) {
    //    threads[i] = thread(my_task_unique_lock, i + 1);
    //}
    //for (auto &pos : threads) {
    //    pos.join();
    //}

    //thread t1(my_task_unique_lock_2, 50, '1');
    //thread t2(my_task_unique_lock_2, 50, '2');
    //t1.join();
    //t2.join();
    //thread threads[5];
    //for (int i = 0; i < 5; ++i) {
    //    threads[i] = thread(finish_task, i);
    //}
    //for (auto &pos : threads) {
    //    pos.join();
    //}


    //thread threads[10];
    //for (int i = 0; i < 10; ++i) {
    //    threads[i] = thread(my_task_once_flag, i+1);
    //}
    //for (auto &pos : threads) {
    //    pos.join();
    //}
    //cout << "Finished!! the result of value is : " << value << endl;


    ///*
    //5. std::promise
    //*/
    ////创建一个 promise<int> 对象，状态值为int类型
    //promise<int> prom;
    ////和 future 关联
    //future<int> fut = prom.get_future();
    ////将 future 交个另外一个线程
    //thread t(printShareState, ref(fut));
    //// 设置共享状态的值，此处和线程t保持同步
    //prom.set_value(10);
    //t.join();


    //thread t1(printShareStateValue);
    //prom1.set_value(10);
    //t1.join();
    ////promise<int>() 创建一个匿名空的promise对象，使用移动拷贝构造函数给prom1
    //prom1 = promise<int>();
    //thread t2(printShareStateValue);
    //prom1.set_value(20);
    //t2.join();

    //promise<int> prom;
    //future<int> fut = prom.get_future();
    //thread t1(getAnInteger, ref(prom));
    //thread t2(printAnInteger, ref(fut));
    //t1.join();
    //t2.join();

    /*
    6. std::package_task
    */
    ////设置 package_task，形式上类似于std::function
    ////如 std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
    //packaged_task<int(int, int)> p_task(my_task_packaged);
    ////获取与 package_task 共享状态相关联的 future 对象
    //future<int> ret = p_task.get_future();
    //thread th(move(p_task), 10, 0);  //创建一个线程完成计数任务
    //int value = ret.get();
    //cout << "the result of the future stage value is " << value << endl;
    //th.join();

    /*
    6.1
    */
    //默认构造函数
    packaged_task<int(int)> p_task_default;
    //使用 lambda 表达式初始化一个 package_task 对象
    packaged_task<int(int)> p_task_lambda([](int x) {return x * 3; });
    p_task_default = move(p_task_lambda);
    //获取与 package_task 共享状态相关联的 future 对象
    future<int> ret = p_task_default.get_future();
    thread(move(p_task_default), 30).detach(); //产生线程，调用被包装的任务
    int value = ret.get();
    cout << "The final result is " << value << endl;






    system("pause");
    return 0;
}

