/*
 * valarray_PhaseB_unittests.cpp
 * EPL - Spring 2015
 */

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

template <typename X, typename Y>
bool match(X x, Y y) {
    double d = x - y;
    if (d < 0) { d = -d; }
    return d < 1.0e-9; // not really machine epsilon, but close enough
}
/*********************************************************************/
// Phase B Tests
/*********************************************************************/

#if defined(PHASE_B0_0) | defined(PHASE_B)
TEST(PhaseB, BasicOperation) {
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

    //////////////
    std::cout << x << std::endl;

    //////////////
    valarray<int> v1(20), v2(20), v3(20), v4(20),
        v5(20), v6(20), v7(20), v8(20);
    for (int i = 0; i<20; ++i) {
        v1[i] = i;
        v2[i] = i + 1;
        v3[i] = i * 2;
        v4[i] = i * 2 + 1;
    }
    v5 = v1 + v2;
    v6 = v3 - v4;
    v7 = v1 * v3;
    v8 = v4 / v2;

    for (int i = 0; i<20; ++i) {
        EXPECT_EQ(v5[i], v1[i] + v2[i]);
        EXPECT_EQ(v6[i], v3[i] - v4[i]);
        EXPECT_EQ(v7[i], v1[i] * v3[i]);
        EXPECT_EQ(v8[i], v4[i] / v2[i]);
    }

    // Differing sizes
    valarray<int> v9(10), v10(10), v11(10), v12(10);
    for (int i = 0; i<10; ++i) {
        v9[i] = v2[i];
        v10[i] = v4[i];
    }
    v5 = v1 + v9;
    v6 = v3 - v10;
    v7 = v1 * v9;
    v8 = v10 / v4;

    for (int i = 0; i<10; ++i) {
        EXPECT_EQ(v5[i], v1[i] + v9[i]);
        EXPECT_EQ(v6[i], v3[i] - v10[i]);
        EXPECT_EQ(v7[i], v1[i] * v9[i]);
        EXPECT_EQ(v8[i], v10[i] / v4[i]);
    }
}
#endif

#if defined(PHASE_B0_1) | defined(PHASE_B)
TEST(PhaseB, Lazy) {
    {
        valarray <double> v1, v2, v3, v4;
        for (int i = 0; i<10; ++i) {
            v1.push_back(1.0);
            v2.push_back(1.0);
            v3.push_back(1.0);
            v4.push_back(1.0);
        }
        int cnt = InstanceCounter::counter;
        v1 + v2 - (v3 * v4);
        EXPECT_EQ(cnt, InstanceCounter::counter);

        valarray<double> ans(10);
        ans = v1 + v2 - (v3*v4);
        EXPECT_TRUE(match(ans[3], (v1[3] + v2[3] - (v3[3] * v4[3]))));
    }
    {
        valarray<int> v1(5);
        v1 = 4 + v1;

        int cnt = InstanceCounter::counter;

        v1 * ((-((v1 * 4).sqrt())) + v1) / v1;
        EXPECT_EQ(cnt, InstanceCounter::counter);

        valarray<int> v2 = -((v1 * 4).sqrt());
        for (int i = 0; i < v2.size(); ++i) {
            EXPECT_EQ(-4, v2[i]);
        }
        EXPECT_EQ(cnt + 1, InstanceCounter::counter);

        valarray<int> v3 = v1 * ((-((v1 * 4).sqrt())) + v1) / v1;
        for (int i = 0; i < v3.size(); ++i) {
            EXPECT_EQ(0, v3[i]);
        }
        EXPECT_EQ(cnt + 2, InstanceCounter::counter);
    }
    {
        valarray<int> v1(5);
        valarray<int> v2 = 4 + v1;

        for (int i = 0; i < 5; ++i) {
            v1[i] = i;
        }

        int cnt = InstanceCounter::counter;
        int sum = v1.sum();
        EXPECT_EQ(10, sum);
        EXPECT_EQ(cnt, InstanceCounter::counter);

        sum = (v1 + v2).sum();
        EXPECT_EQ(30, sum);
        EXPECT_EQ(cnt, InstanceCounter::counter);
    }
    {
        valarray<int> v1(5);
        v1 = 4 + v1;

        int cnt = InstanceCounter::counter;

        v1.sqrt();
        EXPECT_EQ(cnt, InstanceCounter::counter);

        valarray<int> v2 = v1.sqrt();
        for (int i = 0; i < v2.size(); ++i) {
            EXPECT_EQ(2, v2[i]);
        }
        EXPECT_EQ(cnt + 1, InstanceCounter::counter);
    }
}
#endif

#if defined(PHASE_B0_2) | defined(PHASE_B)
TEST(PhaseB, ValarrayToScalarOp) {
    {
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

    {
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
}
#endif

#if defined(PHASE_B1_0) | defined(PHASE_B)
TEST(PhaseB1, Sqrt) {
    valarray<int> v1(10);
    valarray<int> v4 = 4 + v1;

    valarray<float> v5 = v4.sqrt();
    for (uint64_t i = 0; i<10; i++) {
        EXPECT_EQ(2.0, v5[i]);
    }
}
#endif

#if defined(PHASE_B1_1) | defined(PHASE_B)
TEST(PhaseB1, Apply) {
    valarray<int> v1(10);
    valarray<int> v4 = 4 + v1;

    valarray<int> v7 = v4.apply(std::negate<int>());

    EXPECT_EQ(10, v7.size());

    for (uint64_t i = 0; i<10; i++) {
        EXPECT_EQ(-4, v7[i]);
    }
}
#endif

#if defined(PHASE_B1_2) | defined(PHASE_B)
TEST(PhaseB1, Accumulate) {
    valarray<int> v1{1, 2, 3, 4, 5};
    int sum = v1.accumulate(std::plus<int>());
    int product = v1.accumulate(std::multiplies<int>());
    EXPECT_EQ(15, sum);
    EXPECT_EQ(120, product);
}
#endif

#if defined(PHASE_B1_3) | defined(PHASE_B)
TEST(PhaseB1, Iterator) {
    valarray<int> x(10);

    for (auto yi : x) {
        EXPECT_EQ(0, yi);
    }

    valarray<int>::iterator ite = x.begin();
    for (; ite != x.end(); ++ite) {
        EXPECT_EQ(0, *ite);
    }
}
#endif

#if defined(PHASE_B1_4) | defined(PHASE_B)
template <typename T>
struct MultVal {
    T _val;
    using result_type = T;
    using argument_type = T;

    MultVal(T val) : _val(val) {}

    result_type operator()(T val) const {
        return val * _val;
    }
};
TEST(PhaseB1, ApplyAccumulate) {
    {
        valarray<int> v1(10);
        valarray<int> v4 = 4 + v1;

        valarray<float> v5 = v4.sqrt();
        for (uint64_t i = 0; i<10; i++) {
            cout << v5[i] << endl;
            EXPECT_EQ(2.0, v5[i]);
        }
    }

    {
        valarray<int> v1(10);
        valarray<int> v4 = 4 + v1;

        valarray<int> v7 = v4.apply(std::negate<int>());

        EXPECT_EQ(v7.size(), 10);

        for (uint64_t i = 0; i<10; i++) {
            cout << v7[i] << endl;
            EXPECT_EQ(-4, v7[i]);
        }
    }
    {
        valarray<int> v1{1, 2, 3, 4, 5};
        int sum = v1.accumulate(std::plus<int>());
        int product = v1.accumulate(std::multiplies<int>());
        cout << sum << endl;
        cout << product << endl;
        EXPECT_EQ(15, sum);
        EXPECT_EQ(120, product);

        valarray<int> v2{ };
        sum = v2.accumulate(std::plus<int>());
        product = v2.accumulate(std::multiplies<int>());
        cout << sum << endl;
        cout << product << endl;
        EXPECT_EQ(0, sum);
        EXPECT_EQ(0, product);
    }
    {
        valarray<int> v1(10);
        valarray<int> v2 = 4 + v1;

        int cnt = InstanceCounter::counter;
        cout << cnt << endl;
        v2.apply(MultVal<int>(-2));
        EXPECT_EQ(cnt, InstanceCounter::counter);

        valarray<int> v3 = v2.apply(MultVal<int>(-2));
        EXPECT_EQ(cnt + 1, InstanceCounter::counter);

        EXPECT_EQ(10, v3.size());

        for (uint64_t i = 0; i < 10; ++i) {
            EXPECT_EQ(-8, v3[i]);
        }
    }
}
#endif
