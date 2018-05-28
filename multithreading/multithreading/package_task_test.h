///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    std::packaged_task. 线程间通信：共享状态（shared state）。
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/25
#pragma once
#ifndef PACKAGE_TASK_TEST
#define PACKAGE_TASK_TEST

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

//6.1 std::packaged_task::valid()
//在新线程中启动一个 int(int) packaged_task.
future<int> launcher(packaged_task<int(int)> &p_task, int arg) {
    if (p_task.valid()) {
        future<int> fut = p_task.get_future();
        //创建匿名线程，线程运行结束后直接退出不需要和主线程会合
        thread(move(p_task), arg).detach();
        return fut;
    } else {
        return future<int>();
    }
}

//6.2 std::packaged_task::reset
int tripleX(int x) {
    return x * 3;
}

int main_packaged_task_test() {
    /*
    6. std::package_task
    */
    {
        //设置 package_task，形式上类似于std::function
        //如 std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
        packaged_task<int(int, int)> p_task(my_task_packaged);
        //获取与 package_task 共享状态相关联的 future 对象
        future<int> ret = p_task.get_future();
        thread th(move(p_task), 10, 0);  //创建一个线程完成计数任务
        int value = ret.get();
        cout << "the result of the future stage value is " << value << endl;
        th.join();
    }

    /*
    6.1
    */
    {
        //默认构造函数
        packaged_task<int(int)> p_task_default;
        //使用 lambda 表达式初始化一个 package_task 对象
        packaged_task<int(int)> p_task_lambda([](int x) {return x * 3; });
        p_task_default = move(p_task_lambda);
        //获取与 package_task 共享状态相关联的 future 对象
        future<int> fut = p_task_default.get_future();
        thread(move(p_task_default), 30).detach(); //产生线程，调用被包装的任务
        int value = fut.get();
        cout << "The final result is " << value << endl;
    }

    /*
    6.1 packaged_task::valid()
    */
    {
        packaged_task<int(int)> p_task([](int x) {return x * 2; });
        future<int> fut_ret = launcher(p_task, 25);
        if (fut_ret.get() == 50) {
            cout << "task is valid refrence to a shared state" << endl;
        } else {
            cout << "do notiong" << endl;
        }
    }

    //6.2 std::packaged_task::reset
    {
        packaged_task<int(int)> p_task(tripleX);
        future<int> fut_ret = p_task.get_future();
        p_task(100);
        cout << "The tiple of 100 is " << fut_ret.get() << endl;
        p_task.reset();
        fut_ret = p_task.get_future();
        thread(move(p_task), 200).detach();
        cout << "The tiple of 200 is " << fut_ret.get() << endl;
    }

    return 0;
}

#endif  //PACKAGE_TASK_TEST

