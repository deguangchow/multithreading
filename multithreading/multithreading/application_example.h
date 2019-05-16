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

namespace application_example_test {

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
    TICK();
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
    TICK();
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
    TICK();
    for (int i = 1; i <= KItemsToProduct; ++i) {
        sleep_for(milliseconds(500));
        cout << "++++ Produce the " << i << "^th item..." << endl;
        ProduceItem(&gItemRepository, i);
    }
}

// 消费者任务
void ConsumerTask() {
    TICK();
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
    TICK();
    ir->pos_produce = 0;
    ir->pos_consume = 0;
}

//16.2 单生产者-多消费者模型
static const int KItemRepositorySize = 3;
static const int KItemProduce = 6;

struct ItemRepository {
    int item_buffer[KItemRepositorySize];
    size_t pos_consume;
    size_t pos_produce;
    size_t item_counter;    //消费者取走产品的计数器
    mutex mtx;
    mutex mtx_item_counter; //互斥量，保护消费者取走产品的计数器
    condition_variable cv_not_full;
    condition_variable cv_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository *ir, int item) {
    TICK();
    unique_lock<mutex> lock(ir->mtx);
    while (((ir->pos_produce + 1) % KItemRepositorySize) == ir->pos_consume) {
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
    TICK();
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
    TICK();
    for (int i = 1; i <= KItemProduce; ++i) {
        sleep_for(milliseconds(500));
        cout << "++++ Producer thread " << id << " is producing the " << i << "^th item..." << endl;
        ProduceItem(&gItemRepository, i);
    }
    cout << "<<<< Producer thread " << id << " is exiting..." << endl;
}

void ConsumeTask(int id) {
    TICK();
    bool ready_to_exit = false;
    while (1) {
        sleep_for(milliseconds(1000));
        unique_lock<mutex> lock(gItemRepository.mtx_item_counter);
        if (gItemRepository.item_counter < KItemProduce) {
            int item = ConsumeItem(&gItemRepository);
            ++(gItemRepository.item_counter);
            cout << "---- Consumer thread " << id << " is consuming the " << item << "^th item..." << endl;
        } else {
            ready_to_exit = true;
        }
        //lock.unlock();
        if (ready_to_exit == true) {
            break;
        }
    }
    cout << "<<<< Consumer thread " << id << " is exiting..." << endl;
}

void InitRepository(ItemRepository *ir) {
    TICK();
    ir->pos_produce = 0;
    ir->pos_consume = 0;
    ir->item_counter = 0;
}

//16.3 多生产者-单消费者模型
static const int KItemRepositorySize = 3;
static const int KItemProduce = 6;

struct ItemRepository {
    int item_buffer[KItemRepositorySize];
    size_t pos_consume;
    size_t pos_produce;
    size_t item_counter;
    mutex mtx;
    mutex mtx_item_counter;
    condition_variable cv_not_full;
    condition_variable cv_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(int id, ItemRepository *ir, int item) {
    TICK();
    unique_lock<mutex> lock(ir->mtx);
    while (((ir->pos_produce + 1) % KItemRepositorySize) == ir->pos_consume) {
        cout << "==== Producer thread " << id << " is waiting for an empty slot..." << endl;
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
    TICK();
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
    TICK();
    bool read_to_exit = false;
    while (1) {
        sleep_for(milliseconds(500));
        unique_lock<mutex> lock(gItemRepository.mtx_item_counter);
        if (gItemRepository.item_counter < KItemProduce) {
            ++(gItemRepository.item_counter);
            ProduceItem(id, &gItemRepository, gItemRepository.item_counter);
            int item = gItemRepository.item_counter;
            cout << "++++ Producer thread " << id << " is producing the " << item << "^th item..." << endl;
        } else {
            read_to_exit = true;
        }

        if (read_to_exit) {
            break;
        }
    }
    cout << "<<<< Producer thread " << id << "is exiting..." << endl;
}

void ConsumeTask(int id) {
    TICK();
    static int item_consumed = 0;
    while (1) {
        sleep_for(milliseconds(1000));
        ++item_consumed;
        if (item_consumed <= KItemProduce) {
            int item = ConsumeItem(&gItemRepository);
            cout << "---- Comsumer thread " << id << " is consuming the " << item << "^th item" << endl;
        } else {
            break;
        }
    }
    cout << "<<<< Consumer thread " << id << " is exiting..." << endl;
}

void InitItemRepository(ItemRepository *ir) {
    TICK();
    ir->pos_consume = 0;
    ir->pos_produce = 0;
    ir->item_counter = 0;
}

void test_app_ex_1() {
    TICK();
    //16.1 单生产者-单消费者模型
    InitItemRepository(&gItemRepository);
    thread t_producer(ProducerTask);
    thread t_consumer(ConsumerTask);
    t_producer.join();
    t_consumer.join();
}

void test_app_ex_2() {
    TICK();
    //16.2 单生产者-多消费者
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

void test_app_ex_3() {
    TICK();
    //16.3 多生产者-单消费者模型
    InitItemRepository(&gItemRepository);
    vector<thread> vctTProducers;
    for (int i = 1; i <= 4; ++i) {
        vctTProducers.emplace_back(ProduceTask, i);
    }
    thread t_consumer(ConsumeTask, 0);

    for (auto &pos : vctTProducers) {
        pos.join();
    }
    t_consumer.join();
}
#else

#endif

//16.4 多生产者-多消费者模型
static const int KItemRepositorySize = 3;
static const int KItemsToProduce = 6;

struct ItemRepository {
    int item_buffer[KItemRepositorySize];
    size_t pos_consume;
    size_t pos_produce;
    size_t produced_counter;
    size_t consumed_counter;
    mutex mtx;
    mutex mtx_produced_counter;
    mutex mtx_consumed_counter;
    condition_variable cv_not_full;
    condition_variable cv_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(int id, ItemRepository *ir, int item) {
    TICK();
    unique_lock<mutex> lock(ir->mtx);
    while (((ir->pos_produce + 1) % KItemRepositorySize) == ir->pos_consume) {
        cout << "==== Producer thread " << id << " is waiting for an empty slot..." << endl;
        (ir->cv_not_full).wait(lock);
    }

    (ir->item_buffer)[ir->pos_produce] = item;
    (ir->pos_produce)++;

    if (ir->pos_produce == KItemRepositorySize) {
        ir->pos_produce = 0;
    }

    (ir->cv_not_empty).notify_all();
}

int ConsumeItem(int id, ItemRepository *ir) {
    TICK();
    int data;
    unique_lock<mutex> lock(ir->mtx);
    while (ir->pos_produce == ir->pos_consume) {
        cout << ".... Consumer thread " << id << " is waiting for items..." << endl;
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

void ProducerTask(int idx) {
    TICK();
    bool ready_to_exit = false;
    while (1) {
        sleep_for(milliseconds(500));
        unique_lock<mutex> lock(gItemRepository.mtx_produced_counter);
        if (gItemRepository.produced_counter < KItemsToProduce) {
            ++(gItemRepository.produced_counter);
            ProduceItem(idx, &gItemRepository, gItemRepository.produced_counter);
            int item = gItemRepository.produced_counter;
            DEBUG("++++ Producer thread %d is producing the %dth item...", idx, item);
        } else {
            ready_to_exit = true;
        }

        if (ready_to_exit) {
            break;
        }
    }

    INFO("<<<< Producer thread %d is exiting...", idx);
}

void ConsumeTask(int idx) {
    TICK();
    bool ready_to_exit = false;
    while (1) {
        unique_lock<mutex> lock(gItemRepository.mtx_consumed_counter);
        if (gItemRepository.consumed_counter < KItemsToProduce) {
            int item = ConsumeItem(idx, &gItemRepository);
            ++(gItemRepository.consumed_counter);
            DEBUG("++++ Consumer thread %d is consuming the %dth item...", idx, item);
        } else {
            ready_to_exit = true;
        }

        if (ready_to_exit) {
            break;
        }
    }
    INFO("<<<< Consumer thread %d is exiting...", idx);
}

void InitItemRepository(ItemRepository *ir) {
    TICK();
    ir->pos_consume = 0;
    ir->pos_produce = 0;
    ir->consumed_counter = 0;
    ir->produced_counter = 0;
}


void test_app_ex_4() {
    TICK();
    //16.4 多生产者-多消费者模型
    InitItemRepository(&gItemRepository);
    vector<thread> vctProducers;
    for (int i = 1; i <= 4; ++i) {
        vctProducers.emplace_back(ProducerTask, i);
    }
    vector<thread> vctConsumers;
    for (int i = 5; i <= 8; ++i) {
        vctConsumers.emplace_back(ConsumeTask, i);
    }

    for (auto &pos : vctProducers) {
        pos.join();
    }
    for (auto &pos : vctConsumers) {
        pos.join();
    }
}

}//namespace application_example_test

#endif  //APPLICATION_EXAMPLE_H
