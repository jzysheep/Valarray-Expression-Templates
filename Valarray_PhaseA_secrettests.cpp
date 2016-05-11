//
// Created by Xingyuan Wang on 4/28/16.
//

#include <chrono>
#include <complex>
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>

#include "InstanceCounter.h"
#include "Valarray.h"
#include "gtest/gtest.h"

using std::cout;
using std::endl;
using std::string;
using std::complex;

using namespace epl;

/*********************************************************************/
// Phase A Secret Tests
/*********************************************************************/
#if defined(PHASE_AS0_0) | defined(PHASE_AS)
TEST(PhaseASecret, PushPopBack) {
    valarray<int> x(10);
    valarray<int> y(20);

    for (int i = 0; i < 10; i++) {
        x.push_back(1);
    }
    for (int i = 0; i < 5; i++) {
        x.pop_back();
    }

    x = x + y;

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(0, x[i]);
    }
    for (int i = 10; i < 15; i++) {
        EXPECT_EQ(1, x[i]);
    }
}
#endif

#if defined(PHASE_AS0_1) | defined(PHASE_AS)
TEST(PhaseASecret, Cout) {
    valarray<int> x(10);
    std::cout << x << std::endl;
    std::cout << x + 1 << std::endl;
}
#endif

#if defined(PHASE_AS0_2) | defined(PHASE_AS)
TEST(PhaseASecret, ValarrayToValarrayOp) {
    valarray<int> x(0);
    valarray<int> y(0);

    for (int i = 0; i < 10; i++) {
        x.push_back(1);
    }
    for (int i = 0; i < 10; i++) {
        y.push_back(2);
    }

    valarray<int> z = x + y;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(3, z[i]); }

    z = x - y;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(-1, z[i]); }

    z = x * y;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, z[i]); }

    z = y / x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, z[i]); }
}
#endif

#if defined(PHASE_AS0_3) | defined(PHASE_AS)
TEST(PhaseASecret, ValarrayToScalarOp) {
    valarray<int> x(0);

    for (int i = 0; i < 10; i++) {
        x.push_back(1);
    }

    valarray<int> y = x + 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, y[i]); }

    y = x - 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(0, y[i]); }

    y = x * 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }

    y = x / 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }
}
#endif

#if defined(PHASE_AS0_4) | defined(PHASE_AS)
TEST(PhaseASecret, ScalarToValarrayOp) {
    valarray<int> x(0);

    for (int i = 0; i < 10; i++) {
        x.push_back(1);
    }

    valarray<int> y = 1 + x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, y[i]); }

    y = 1 - x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(0, y[i]); }

    y = 1 * x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }

    y = 1 / x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }
}
#endif

#if defined(PHASE_AS0_5) | defined(PHASE_AS)
TEST(PhaseASecret, VoidConstructor) {
    valarray<int> x;
    SUCCEED();
}
#endif

#if defined(PHASE_AS0_6) | defined(PHASE_AS)
TEST(PhaseASecret, Iterator) {
    valarray<int> x(10);

    valarray<int>::iterator ite = x.begin();
    for (; ite != x.end(); ++ite) {
        EXPECT_EQ(0, *ite);
    }
}
#endif

#if defined(PHASE_AS1_0) | defined(PHASE_AS)
TEST(PhaseA1Secret, AssignmentOperator) {
    valarray<int> v1(10);
    valarray<double> v2(5);

    for (int i = 0; i < 5; ++i) {
        v2[i] = (i + 0.5);
    }

    v1 = v2;

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(i, v1[i]);
    }
}
#endif

#if defined(PHASE_AS2_0) | defined(PHASE_AS)
TEST(PhaseA2Secret, ValarrayToValarrayOp) {
    valarray<int> x(0);
    valarray<int> y(0);

    for (int i = 0; i < 10; i++) {
        x.push_back(1);
    }
    for (int i = 0; i < 10; i++) {
        y.push_back(2);
    }

    int cnt = InstanceCounter::counter;
    x + y;
    x - y;
    x * y;
    y / x;
    x + y - x / x;
    (x + y) * (y / x);
    EXPECT_EQ(cnt, InstanceCounter::counter);

    valarray<int> z = x + y;

    for (int i = 0; i < 10; i++) { EXPECT_EQ(3, z[i]); }

    z = x - y;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(-1, z[i]); }

    z = x * y;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, z[i]); }

    z = y / x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, z[i]); }
}
#endif


#if defined(PHASE_AS2_1) | defined(PHASE_AS)
TEST(PhaseA2Secret, ValarrayToScalarOp) {
    valarray<int> x(0);

    for (int i = 0; i < 10; i++) {
        x.push_back(1);
    }

    int cnt = InstanceCounter::counter;
    x + 1;
    x - 1;
    x * 1;
    x / 1;
    (x + x) + 1;
    (x - 1) - 1;
    (x * 1) * 1;
    (x / 1) / 1;
    EXPECT_EQ(cnt, InstanceCounter::counter);

    valarray<int> y = x + 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, y[i]); }

    y = x - 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(0, y[i]); }

    y = x * 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }

    y = x / 1;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }
}
#endif

#if defined(PHASE_AS2_2) | defined(PHASE_AS)
TEST(PhaseA2Secret, ScalarToValarrayOp) {
    valarray<int> x(0);

    for (int i = 0; i < 10; i++) {
        x.push_back(1);
    }

    int cnt = InstanceCounter::counter;
    1 + x;
    1 - x;
    1 * x;
    1 / x;
    1 + (x + x);
    1 - (x - 1);
    1 * (x * 1);
    1 / (x / 1);
    EXPECT_EQ(cnt, InstanceCounter::counter);

    valarray<int> y = 1 + x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(2, y[i]); }

    y = 1 - x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(0, y[i]); }

    y = 1 * x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }

    y = 1 / x;
    for (int i = 0; i < 10; i++) { EXPECT_EQ(1, y[i]); }
}
#endif

