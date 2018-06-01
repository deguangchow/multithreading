///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    c++11 并发：应用实例
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/06/01
#pragma once
#ifndef APPLICATION_EXAMPLE_H
#define APPLICATION_EXAMPLE_H

/*
16. 应用实例：生产者-消费者模型
*/
#if 0
//16.1 单生产者-单消费者模型
// 缓冲区的大小
static const int KItemRepositorySize = 3;
// 计划生产商品的数目
static const int KItemsToProduct = 6;

struct ItemRepository {
    int item_buffer[KItemRepositorySize];   //产品缓冲区，配合 pos_consume 和 pos_produce 模型环形队列
    size_t pos_consume;                     //消费者消费产品的位置
    size_t pos_produce;                     //生产者生产产品的位置
    mutex mtx;                              //互斥量，保护产品缓冲区
    condition_variable cv_not_full;         //条件变量，指示产品缓冲区不为满
    condition_variable cv_not_empty;        //条件变量，指示产品缓冲区不为空
}gItemRepository;                           //产品库全局变量，生产者和消费者操作该变量。

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository *ir, int item) {
    unique_lock<mutex> lock(ir->mtx);
    while (((ir->pos_produce + 1) % KItemRepositorySize) == ir->pos_consume) {
        cout << "==== Producer is waiting for an empty slot..." << endl;
        (ir->cv_not_full).wait(lock);   //生产者等待“产品库缓冲区不为满”这一条件发生。
    }

    (ir->item_buffer)[ir->pos_produce] = item;   // 写入产品
    (ir->pos_produce)++;

    if (ir->pos_produce == KItemRepositorySize) {
        ir->pos_produce = 0;
    }

    (ir->cv_not_empty).notify_all(); //通知消费者产品库不为空
                                     //lock.unlock();  //解锁
}

int ConsumeItem(ItemRepository *ir) {
    int data;
    unique_lock<mutex> lock(ir->mtx);
    while (ir->pos_produce == ir->pos_consume) {
        cout << ".... Consumer is waiting for items..." << endl;
        (ir->cv_not_empty).wait(lock);  // 消费者等待“产品库缓冲区不为空”这一条件发生。
    }

    data = (ir->item_buffer)[ir->pos_consume];
    (ir->pos_consume)++;

    if (ir->pos_consume >= KItemRepositorySize) {
        ir->pos_consume = 0;
    }

    (ir->cv_not_full).notify_all(); //通知生产者产品库不为满
                                    //lock.unlock();  //解锁

    return data;
}

// 生产者任务
void ProducerTask() {
    for (int i = 1; i <= KItemsToProduct; ++i) {
        sleep_for(milliseconds(500));
        cout << "++++ Produce the " << i << "^th item..." << endl;
        ProduceItem(&gItemRepository, i);
    }
}

// 消费者任务
void ConsumerTask() {
    static int cnt = 0;
    while (1) {
        sleep_for(milliseconds(1000));
        int item = ConsumeItem(&gItemRepository);
        cout << "---- Consume the " << item << "^th item." << endl;
        if (++cnt == KItemsToProduct) {
            break;
        }
    }
}

void InitItemRepository(ItemRepository *ir) {
    ir->pos_produce = 0;
    ir->pos_consume = 0;
}
#endif
//16.2 单生产者-多消费者模型
static const int KItemRepositorySize = 3;
static const int KItemProduce = 6;

struct ItemRepository {
    int item_buffer[KItemRepositorySize];
    size_t pos_consume;
    size_t pos_produce;
    size_t item_counter;
    mutex mtx;
    mutex item_counter_mtx;
    condition_variable cv_not_full;
    condition_variable cv_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository *ir, int item) {
    unique_lock<mutex> lock(ir->mtx);
    while (((ir->pos_produce+1)%KItemRepositorySize) == ir->pos_consume) {
        cout << "==== Producer is waiting for an empty slot..." << endl;
        (ir->cv_not_full).wait(lock);
    }

    (ir->item_buffer)[ir->pos_produce] = item;
    (ir->pos_produce)++;

    if (ir->pos_produce == KItemRepositorySize) {
        ir->pos_produce = 0;
    }

    (ir->cv_not_empty).notify_all();
}

int ConsumeItem(ItemRepository *ir) {
    int data;
    unique_lock<mutex> lock(ir->mtx);
    while (ir->pos_produce == ir->pos_consume) {
        cout << ".... Consumer is waiting for items..." << endl;
        (ir->cv_not_empty).wait(lock);
    }

    data = (ir->item_buffer)[ir->pos_consume];
    (ir->pos_consume)++;

    if (ir->pos_consume >= KItemRepositorySize) {
        ir->pos_consume = 0;
    }

    (ir->cv_not_full).notify_all();

    return data;
}

void ProduceTask(int id) {
    for (int i = 1; i <= KItemProduce; ++i) {
        sleep_for(milliseconds(500));
        cout << "++++ Producer thread " << id << " is producing the " << i << "^th item..." << endl;
        ProduceItem(&gItemRepository, i);
    }
    cout << "<<<< Producer thread " << id << " is exiting..." << endl;
}

void ConsumeTask(int id) {
    bool ready_to_exit = false;
    while (1) {
        sleep_for(milliseconds(1000));
        unique_lock<mutex> lock(gItemRepository.item_counter_mtx);
        if (gItemRepository.item_counter < KItemProduce) {
            int item = ConsumeItem(&gItemRepository);
            ++(gItemRepository.item_counter);
            cout << "---- Consumer thread " << id << " is consuming the " << item << "^th item..." << endl;
        } else {
            ready_to_exit = true;
        }
        lock.unlock();
        if (ready_to_exit == true) {
            break;
        }
    }

    cout << "<<<< Consumer thread " << id << " is exiting..." << endl;
}

void InitRepository(ItemRepository *ir) {
    ir->pos_produce = 0;
    ir->pos_consume = 0;
    ir->item_counter = 0;
}

int main_application_example() {
#if 0
    {   //16.1 单生产者-单消费者模型
        InitItemRepository(&gItemRepository);
        thread t_producer(ProducerTask);
        thread t_consumer(ConsumerTask);
        t_producer.join();
        t_consumer.join();
    }
#endif
    {   //16.2 单生产者-多消费者
        InitRepository(&gItemRepository);
        thread t_producer(ProduceTask, 0);
        vector<thread> vctTConsumers;
        for (int i = 1; i <= 4; ++i) {
            vctTConsumers.emplace_back(ConsumeTask, i);
        }

        t_producer.join();
        for (auto &pos : vctTConsumers) {
            pos.join();
        }
    }


    return 0;
}


#endif  //APPLICATION_EXAMPLE_H
