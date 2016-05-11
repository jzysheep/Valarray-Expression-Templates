#ifndef _Valarray_h
#define _Valarray_h
#include "Vector.h"
#include <vector>
#include <complex>

using epl::vector;

template <typename V>
struct VectorWrapper;

template <typename T>
using valarray = VectorWrapper<vector<T>>;

template <typename T>
struct choose_ref {
    using type = T;
};

template <typename T>
struct choose_ref<vector<T>> {
    using type = const vector<T>&;
};

template <typename T>
using ChooseRef = typename choose_ref<T>::type;

template <typename T>
struct Rank {
    static constexpr int value = 0;
};

template <>
struct Rank<int> {
    static constexpr int value = 1;
};

template <>
struct Rank<float> {
    static constexpr int value = 2;
};

template <>
struct Rank<double> {
    static constexpr int value = 3;
};

template <typename T>
struct Rank<std::complex<T>> {
    static constexpr int value = Rank<T>::value;
};

template <int T>
struct BasicType;

template <>
struct BasicType<1> {
    using type = int;
};

template <>
struct BasicType<2> {
    using type = float;
};

template <>
struct BasicType<3> {
    using type = double;
};



template <typename T>
struct is_complex : public std::false_type {

};


template <typename T>
struct is_complex<std::complex<T>> : public std::true_type {

};

template <bool p, typename T>
struct ComplexType;

template <typename T>
struct ComplexType<true, T> {
    using type = std::complex<T>;
};

template <typename T>
struct ComplexType<false, T> {
    using type = T;
};

template <typename T1, typename T2>
struct ChooseType {
    static constexpr int t1_rank = Rank<T1>::value;
    static constexpr int t2_rank = Rank<T2>::value;
    static constexpr int max_rank = (t1_rank > t2_rank) ? t1_rank : t2_rank;

    using max_basic_type = typename BasicType<max_rank>::type;

    static constexpr bool t1_complex = is_complex<T1>::value;
    static constexpr bool t2_complex = is_complex<T2>::value;
    static constexpr bool is_complex = t1_complex || t2_complex;

    using return_type = typename ComplexType<is_complex, max_basic_type>::type;
};


// ReturnType can choose type between the combinations of VectorWrapper or Scalar
template <typename T1, typename T2>
struct ReturnType;

// value type is recursive
template <typename V1, typename V2>
struct ReturnType<VectorWrapper<V1>, VectorWrapper<V2>> {
    using type = typename ChooseType<typename V1::value_type, typename V2::value_type>::return_type;
};

template <typename V, typename T>
struct ReturnType<VectorWrapper<V>, T> {
    using type = typename ChooseType<typename V::value_type, T>::return_type;
};

template <typename T, typename V>
struct ReturnType<T, VectorWrapper<V>> {
    using type = typename ChooseType<T, typename V::value_type>::return_type;
};

template <bool p, typename T>
using EnableIf = typename std::enable_if<p, T>::type;

template <typename T>
struct root {
    using result_type = typename ComplexType<is_complex<T>::value, double>::type;
    result_type operator() (T x) const { return sqrt(x); }
};


template <typename T>
class const_iterator {
private:
    const T parent; // might be better to use a pointer, but copying proxy is cheap
    uint64_t index;

public:
    const_iterator(const T _p, const uint64_t& _i) : parent(_p), index(_i) {}

    typename T::value_type operator*() const { return parent[index]; }

    const_iterator& operator++() {
        index++;
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator t{*this};
        this->operator++();
        return t;
    }

    const_iterator& operator--() {
        index--;
        return *this;
    }

    const_iterator operator--(int) {
        const_iterator t{*this};
        this->operator--();
        return t;
    }

    bool operator==(const const_iterator& that) const {
        return this->index == that.index;
    }

    bool operator!=(const const_iterator& that) const {
        return !(*this == that);
    }
};


template <typename T>
class Scalar {
private:
    T value;

public:
    using value_type = T;

    uint64_t size() const {
        return std::numeric_limits<uint64_t>::max();
    }

    T operator[] (uint64_t idx) const {
        return value;
    }

    Scalar(const T& val): value(val) {}

    Scalar(const Scalar& that): value(that.value) {}

    Scalar() = default;

    ~Scalar() = default;

    const_iterator<Scalar> begin() const {
        return const_iterator<Scalar>(*this, 0);
    }

    const_iterator<Scalar> end() const {
        return const_iterator<Scalar>(*this, size());
    }

};


template <typename T, typename Operator>
class UnaryProxy {
private:
    ChooseRef<T> parent; // has-a relationship
    Operator op;

public:
    using value_type = typename T::value_type;

    uint64_t size() const {
        return parent.size();
    }

    typename Operator::result_type operator[](uint64_t index) const {
        return op(parent[index]);
    }

    UnaryProxy(ChooseRef<T> _p, Operator _op): parent(_p), op(_op) {}

    UnaryProxy(const UnaryProxy& that): parent(that.parent), op(that.op) {}

    UnaryProxy() = default;

    ~UnaryProxy() = default;

    const_iterator<UnaryProxy> begin() const {
        return const_iterator<UnaryProxy>(*this, 0);
    }

    const_iterator<UnaryProxy> end() const {
        return const_iterator<UnaryProxy>(*this, size());
    }
};

template <typename Left, typename Right, typename Operator>
class BinaryProxy {
private:
    ChooseRef<Left> l;
    ChooseRef<Right> r;
    Operator op;

public:
    using value_type = typename ChooseType<typename Left::value_type, typename Right::value_type>::return_type;

    typename Operator::result_type operator[](uint64_t idx) const {
        return op(l[idx], r[idx]);
    } // This is recursive

    uint64_t size() const {
        return l.size() < r.size() ? l.size() : r.size();
    }

    BinaryProxy(ChooseRef<Left> _l, ChooseRef<Right> _r, Operator _op): l(_l), r(_r), op(_op) {}

    BinaryProxy(const BinaryProxy& that) : l(that.l), r(that.r), op(that.op) {}

    BinaryProxy() = default;

    ~BinaryProxy() = default;

    const_iterator<BinaryProxy> begin() const {
        return const_iterator<BinaryProxy>(*this, 0);
    }

    const_iterator<BinaryProxy> end() const {
        return const_iterator<BinaryProxy>(*this, size());
    }
};


template <typename V>
struct VectorWrapper : public V {
    VectorWrapper() : V() {}

    VectorWrapper(const V& that) : V(that) {}

    explicit VectorWrapper(uint64_t size) : V(size) {}
    

    template <typename T>
    VectorWrapper(const VectorWrapper<T>& that ) { // different types
        for (auto val : that) {
            this->push_back( static_cast<typename V::value_type>(val) );
        }
    }

    VectorWrapper& operator=(const VectorWrapper<V>& that) {  // left and right are the same type
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = that[i];
            }
        }
        return *this;
    }

    template <typename T>
    EnableIf<Rank<T>::value, VectorWrapper&> operator=(const T& that) {
        this->operator=(VectorWrapper<Scalar<T>>(that));
    }

    template <typename T>
    VectorWrapper& operator=(const VectorWrapper<T>& that) {    // left and right are different types
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = static_cast<typename V::value_type>(that[i]);
            }
        }
        return *this;
    }

    template <typename Operator>
    VectorWrapper<UnaryProxy<V, Operator>> apply(Operator op) {
        return VectorWrapper<UnaryProxy<V, Operator>>( UnaryProxy<V, Operator>(*this, op) );
    }


    VectorWrapper<UnaryProxy<V, root<typename V::value_type>>> sqrt(void) {
        return apply(root<typename V::value_type>{});
    }

    VectorWrapper<UnaryProxy<V, std::negate<typename V::value_type>>> operator-(void) {
        return apply(std::negate<typename V::value_type>{});
    }

    template <typename T>
    VectorWrapper(std::initializer_list<T> il) : V(il) {}

    ~VectorWrapper() = default;
    

    template <typename Operator>
    typename Operator::result_type accumulate(Operator op) {
        if (this->size() > 0) {
            typename Operator::result_type res = this->operator[](0);
            for (auto it = ++(this->begin()); it != this->end(); ++it)
                res = op(res, *it);
            return res;
        } else {
            return 0;
        }
        
    }
    
    typename std::plus<typename V::value_type>::result_type sum() {
        return accumulate(std::plus<typename V::value_type>{});
    }

};


template <typename Operator>
struct ZJType {
    template <typename T1, typename T2>
    static VectorWrapper<BinaryProxy<T1, T2, Operator>> calculate(const VectorWrapper<T1>& l, const VectorWrapper<T2>& r) {
        return VectorWrapper<BinaryProxy<T1, T2, Operator>>(BinaryProxy<T1, T2, Operator>(l, r, Operator{}));
    }

    template <typename V, typename T>
    static EnableIf<Rank<T>::value != 0, VectorWrapper<BinaryProxy<V, Scalar<T>, Operator>>> calculate(const VectorWrapper<V>& l, const T& r) {
        return VectorWrapper<BinaryProxy<V, Scalar<T>, Operator>>(BinaryProxy<V, Scalar<T>, Operator>(l, VectorWrapper<Scalar<T>>(r), Operator{}));
    }

    template <typename T, typename V>
    static EnableIf<Rank<T>::value != 0, VectorWrapper<BinaryProxy<Scalar<T>, V, Operator>>> calculate(const T& l, const VectorWrapper<V>& r) {
        return VectorWrapper<BinaryProxy<Scalar<T>, V, Operator>>(BinaryProxy<Scalar<T>, V, Operator>(VectorWrapper<Scalar<T>>(l), r, Operator{}));
    }


};

template <typename T1, typename T2, typename Operator = std::plus<typename ReturnType<T1, T2>::type>>
auto operator+(const T1& l, const T2& r) -> decltype(ZJType<Operator>::calculate(l, r)) {
    return ZJType<Operator>::calculate(l, r);
}

template <typename T1, typename T2, typename Operator = std::minus<typename ReturnType<T1, T2>::type>>
auto operator-(const T1& l, const T2& r) -> decltype(ZJType<Operator>::calculate(l, r)) {
    return ZJType<Operator>::calculate(l, r);
}

template <typename T1, typename T2, typename Operator = std::multiplies<typename ReturnType<T1, T2>::type>>
auto operator*(const T1& l, const T2& r) -> decltype(ZJType<Operator>::calculate(l, r)) {
    return ZJType<Operator>::calculate(l, r);
}

template <typename T1, typename T2, typename Operator = std::divides<typename ReturnType<T1, T2>::type>>
auto operator/(const T1& l, const T2& r) -> decltype(ZJType<Operator>::calculate(l, r)) {
    return ZJType<Operator>::calculate(l, r);
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const VectorWrapper<T>& v) {
    for (uint64_t i = 0; i < v.size(); ++i) {
        out << v[i] << " ";
    }
    out << "\n";
    return out;
}
#endif /* _Valarray_h */