///    Copyright (C) 2018 cpp_prime5 DG.C
///        http://www.futuresoft.com.cn
///
///    \brief    function test.
///
///    \author   zhoudeguang
///    \version  1.0
///    \2018/05/25
#pragma once
#ifndef FUNCTION_TEST_H
#define FUNCTION_TEST_H

//std::function
function<int(int)> Functional;
//普通函数
int TestFunc(int a) {
    return a;
}
//lambda表达式
auto lambda_fun = [](int a)->int {
    return a;
};
//仿函数（functor)
class Functor {
public:
    int operator()(int a) {
        return a;
    }
};
//1.类成员函数
//2.类静态函数
class TestClass {
public:
    int ClassMember(int a) {
        return a;
    }
    static int StaticMember(int a) {
        return a;
    }
};

int main_function_test() {
    //普通函数
    Functional = TestFunc;
    int result = Functional(10);
    cout << "普通函数：" << result << endl;

    //Lambda表达式
    Functional = lambda_fun;
    result = Functional(20);
    cout << "Lambda表达式：" << result << endl;

    //仿函数
    Functor testFunctor;
    Functional = testFunctor;
    result = Functional(30);
    cout << "仿函数：" << result << endl;

    //类成员函数
    TestClass testClass;
    Functional = bind(&TestClass::ClassMember, testClass, _1);
    result = Functional(40);
    cout << "类成员函数：" << result << endl;

    //类静态函数
    Functional = TestClass::StaticMember;
    result = Functional(50);
    cout << "类静态函数：" << result << endl;

    return 0;
}

#endif  //FUNCTION_TEST_H

