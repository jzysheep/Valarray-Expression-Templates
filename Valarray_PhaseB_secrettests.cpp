//
// Created by Xingyuan Wang on 4/30/16.
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
// Phase B Secret Tests
/*********************************************************************/
#if defined(PHASE_BS1_0) | defined(PHASE_BS)
TEST(PhaseB1Secret, ExpressionIterator) {
    valarray<int> x(10);
    valarray<double> y(10);

    for (int i = 0; i < 10; ++i) {
        x[i] = i;
        y[i] = 10 - i;
    }

    for (auto const& val : x + y) {
        EXPECT_EQ(10, val);
    }
}
#endif


#if defined(PHASE_BS1_1) | defined(PHASE_BS)
template <typename T>
struct AddPlus5 {
    using first_argument_type = T;
    using second_argument_type = T;
    using result_type = T;
    using argument_type = T;

    result_type operator()(T val1, T val2) const {
        return val1 + val2 + 5;
    }
};

TEST(PhaseB1Secret, AccumulateTest) {
    valarray<int> v1{5};
    int sum = v1.accumulate(std::plus<int>());
    int product = v1.accumulate(std::multiplies<int>());
    EXPECT_EQ(5, sum);
    EXPECT_EQ(5, product);

    valarray<int> v2(5);
    for (int i = 0; i < 5; ++i) {
        v2[i] = i;
    }

    sum = v2.accumulate(AddPlus5<int>());
    EXPECT_EQ(30, sum);
}
#endif

#if defined(PHASE_BS1_2) | defined(PHASE_BS)
template <typename T>
struct AddVal {
    T _val;
    using result_type = T;
    using argument_type = T;

    AddVal(T val) : _val(val) {}

    result_type operator()(T val) const {
        return val + _val;
    }
};

TEST(PhaseB1Secret, ApplyWithState) {
    valarray<int> v1(10);
    valarray<int> v2 = 4 + v1;

    int cnt = InstanceCounter::counter;
    valarray<int> v3 = v2.apply(AddVal<int>(5));
    EXPECT_EQ(cnt + 1, InstanceCounter::counter);

    EXPECT_EQ(10, v3.size());

    for (uint64_t i = 0; i < 10; ++i) {
        EXPECT_EQ(9, v3[i]);
    }
}
#endif

#if defined(PHASE_BS1_3) | defined(PHASE_BS)
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

TEST(PhaseB1Secret, ApplyWithLazyEval) {
    valarray<int> v1(10);
    valarray<int> v2 = 4 + v1;

    int cnt = InstanceCounter::counter;
    v2.apply(MultVal<int>(-2));
    EXPECT_EQ(cnt, InstanceCounter::counter);

    valarray<int> v3 = v2.apply(MultVal<int>(-2));
    EXPECT_EQ(cnt + 1, InstanceCounter::counter);

    EXPECT_EQ(10, v3.size());

    for (uint64_t i = 0; i < 10; ++i) {
        EXPECT_EQ(-8, v3[i]);
    }
}
#endif



#if defined(PHASE_BS1_4) | defined(PHASE_BS)
TEST(PhaseB1Secret, LazyEvalSum) {
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
#endif

#if defined(PHASE_BS1_5) | defined(PHASE_BS)
TEST(PhaseB1Secret, LazyEvalSqrt) {
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
#endif

#if defined(PHASE_BS1_6) | defined(PHASE_BS)
TEST(PhaseB1Secret, LazyEvalComplete) {
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
#endif