/*
copyright:  deguangchow@qq.com
data:       2018/05/28
autor:      deguangchow
brief:      �̼߳�ͨ�ţ���������
*/
#pragma once
#ifndef CONDITION_VARIABLE_TEST
#define CONDITION_VARIABLE_TEST

/*
11. std::condition_variable
*/
//ȫ�ֱ���
mutex g_mtx;
//ȫ����������
condition_variable g_cv;
//ȫ�ֱ�־λ
bool g_ready = false;

void do_print_id(int id) {
    unique_lock<mutex> lck(g_mtx);
    // �����־λ��Ϊ true����ȴ�����
    // ��ǰ�̱߳���������ȫ�ֱ�־λ��Ϊ true ֮��
    // �̱߳����ѣ���������ִ�д�ӡ�̱߳�� id.
    while (!g_ready) {
        g_cv.wait(lck);
    }
    cout << "thread " << id << endl;
}

void go() {
    unique_lock<mutex> lck(g_mtx);
    // ����ȫ�ֱ�־λΪ true.
    g_ready = true;
    // ���������߳�.
    g_cv.notify_all();
}

//11.1 std::condition_variable::wait, notify_one 
int g_cargo = 0;    //ȫ�ֱ���������
// ��װ��
bool shipment_available() {
    return g_cargo != 0;
}

// �������߳�
void consume(int n) {
    for (int i = 0; i < n; ++i) {
        unique_lock<mutex> lck(g_mtx);
        // ֻ�е� shipment_available() ����Ϊ false ʱ���� wait() �Ż�������ǰ�̣߳�
        // �������յ������̵߳�֪ͨ��ֻ�е� shipment_available() ���� true ʱ�Żᱻ�������
        g_cv.wait(lck, shipment_available);
        cout << g_cargo << endl;
        g_cargo = 0;
    }
}

int main_condition_variable() {
    {   //11. std::condition_variable
        thread threads[10];
        // ����10���̣߳�����ȫ�ֱ�־readyΪfalse��
        // ���������̴߳�����ɺ���������״̬תΪ����̬
        for (int i = 0; i < 10; ++i) {
            threads[i] = thread(do_print_id, i);
        }

        // �޸� ready ��־����֪ͨ�����߳̿��Դ�����̬תΪ����̬
        // ����Ȼ10���߳�Ҳ����ĳһ��ֻ������һ��
        cout << "wait 1 second����" << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        go();

        for (auto &pos : threads) {
            pos.join();
        }
    }
    {   //11.1 std::condition_variable::wait, notify_one
        thread consumer_thread(consume, 10);    //�������߳�

        // ���߳�Ϊ�������̣߳����� 10 ����Ʒ
        for (int i = 0; i < 10; ++i) {
            while (shipment_available()) {
                // yield() ���������������������߳���������״̬�����½�������ϵͳ���е���
                std::this_thread::yield();
            }
            unique_lock<mutex> lck(g_mtx);
            g_cargo = i + 1;
            g_cv.notify_one();
        }

        consumer_thread.join();
    }

    return 0;
}


#endif  //CONDITION_VARIABLE_TEST
