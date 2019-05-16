///    Copyright (C) 2018 DG.C, DGCHOW, deguangchow
///        deguangchow@qq.com
///
///    \brief    function
///
///    \author   deguangchow
///    \version  1.0
///    \2019/05/16

#pragma once
#ifndef FUNCTION_TEST_H
#define FUNCTION_TEST_H

namespace function_test {

//std::function
function<int(int)> g_Functional;
//普通函数
int TestFunc(int a) {
    TICK();
    return a;
}
//lambda表达式
auto const& g_lambda_fun = [](int a)->int {
    TICK();
    return a;
};
//仿函数（functor)
class Functor {
public:
    int operator()(int a) {
        TICK();
        return a;
    }
};
//1.类成员函数
//2.类静态函数
class TestClass {
public:
    int ClassMember(int a) {
        TICK();
        return a;
    }
    static int StaticMember(int a) {
        TICK();
        return a;
    }
};

void test_function_1() {
    TICK();
    //普通函数
    g_Functional = TestFunc;
    int result = g_Functional(10);
    INFO("普通函数：%d", result);
}

void test_function_2() {
    TICK();
    //Lambda表达式
    g_Functional = g_lambda_fun;
    int result = g_Functional(20);
    INFO("Lambda表达式：%d", result);
}

void test_function_3() {
    TICK();
    //仿函数
    Functor testFunctor;
    g_Functional = testFunctor;
    int result = g_Functional(30);
    INFO("仿函数：%d", result);
}

void test_function_4() {
    TICK();
    //类成员函数
    TestClass testClass;
    g_Functional = bind(&TestClass::ClassMember, ref(testClass), _1);
    int result = g_Functional(40);
    INFO("类成员函数：%d", result);
}

void test_function_5() {
    TICK();
    //类静态函数
    g_Functional = TestClass::StaticMember;
    int result = g_Functional(50);
    INFO("类静态函数：%d", result);
}

}//namespace function_test

#endif  //FUNCTION_TEST_H

