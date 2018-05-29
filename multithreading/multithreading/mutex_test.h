///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    add some samples for std::mutex. 线程间通信：互斥量的使用例子。
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/25
#pragma once
#ifndef MUTEX_TEST
#define MUTEX_TEST

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
        sleep_for(milliseconds(10));
    }
}

void exec_proc2(int &n) {
    for (int i = 0; i < 5; ++i) {
        cout << "pass reference, executing thread " << n << endl;
        ++n;
        sleep_for(milliseconds(10));
    }
}

/*
3. 赋值操作
*/
void exec_produce(int duration) {
    //阻止线程运行到duration秒
    sleep_for(seconds(duration));
    //get_id（）获取当前线程id
    cout << "exec_produce thread " << get_id() << " has sleeped " << duration << " seconds" << endl;
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
            cout << "++++ thread(" << get_id() << ") counter=" << counter << "\t" << endl;
        } else {
            cout << "0000 thread(" << get_id() << ") counter=" << counter << "\t" << endl;
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
    while (!t_mutex.try_lock_for(milliseconds(200))) {
        cout << val;
    }
    //成功取得锁，然后将线程sleep到1000ms
    sleep_for(milliseconds(1000));
    cout << tag << endl;
    t_mutex.unlock();
}

/*
4.2 lock_gurad
*/
void print(int x) {
    cout << "value is " << x;
    cout << endl;
    sleep_for(milliseconds(200));
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
    sleep_for(milliseconds(1000));

    //使setValue函数只被第一次执行的线程执行
    call_once(value_once_flag, setValue, id);
}


int main_mutex_test() {
    /*
    1.Hello World
    */
    {
        thread t(thread_task);
        t.join();
    }


    /*
    2. Thread Constructor
    1）默认构造函数：thread() noexcept，创建一个空的 thread 执行对象。
    2）初始化构造函数： template
    */
    {
        int n = 0;
        // t1，使用默认构造函数，什么都没做
        thread t1;
        // t2，使用有参构造函数，传入函数名称（地址）exec_pro1，并以传值的方式传入args
        // 将会执行exec_proc1中的代码
        thread t2(exec_proc1, n + 1);
        // t3，使用有参构造函数，传入函数名称（地址）exec_pro2，并以传引用的方式传入args
        // 将会执行exec_proc2中的代码
        thread t3(exec_proc2, ref(n));
        // t4，使用移动构造函数，由t4接管t3的任务，t3不再是线程了
        thread t4(move(t3));
        // 可被 joinable 的 thread 对象必须在他们销毁之前被主线程 join 或者将其设置为 detached.
        t2.join();
        t4.join();
        cout << "the result of n is " << n << endl;
    }


    /*
    3. 赋值操作
    */
    {
        thread threads[5];
        cout << "create 5 threads" << endl;
        for (int i = 0; i < 5; ++i) {
            threads[i] = thread(exec_produce, i + 1);
        }
        cout << "finished creating 5 threads, and waiting for joining" << endl;
        for (auto &pos : threads) {
            pos.join();
        }
        cout << "Finished!!!" << endl;
    }

    /*
    4. mutex
    */
    {
        thread threads[MAX_THREADS_VALUE];
        for (int i = 0; i < MAX_THREADS_VALUE; ++i) {
            threads[i] = thread(my_task);
        }
        for (auto &pos : threads) {
            pos.join();
        }
        cout << "Finished : the result of counter is " << counter << endl;
    }

    {
        thread threads[10];
        char end_tag[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')' };
        for (int i = 0; i < 10; ++i) {
            threads[i] = thread(my_task_time, i, end_tag[i]);
        }
        for (auto &pos : threads) {
            pos.join();
        }
    }


    {
        thread threads[10];
        for (int i = 0; i < 10; ++i) {
            threads[i] = thread(my_task_lock_gurad, i + 1);
        }
        for (auto &pos : threads) {
            pos.join();
        }
    }

    {
        thread threads[10];
        for (int i = 0; i < 10; ++i) {
            threads[i] = thread(my_task_unique_lock, i + 1);
        }
        for (auto &pos : threads) {
            pos.join();
        }
    }

    {
        thread t1(my_task_unique_lock_2, 50, '1');
        thread t2(my_task_unique_lock_2, 50, '2');
        t1.join();
        t2.join();
        thread threads[5];
        for (int i = 0; i < 5; ++i) {
            threads[i] = thread(finish_task, i);
        }
        for (auto &pos : threads) {
            pos.join();
        }
    }


    {
        thread threads[10];
        for (int i = 0; i < 10; ++i) {
            threads[i] = thread(my_task_once_flag, i + 1);
        }
        for (auto &pos : threads) {
            pos.join();
        }
        cout << "Finished!! the result of value is : " << value << endl;
    }

    return 0;
}

#endif  //MUTEX_TEST


