#ifndef GPIO_CFG_HPP
#define GPIO_CFG_HPP

template<long long N>
constexpr long long factorial() {
    return N * factorial<N - 1>();
}

template<>
constexpr long long factorial<0>() {
    return 1;
}

constexpr long long fact45 = factorial<15>();

#endif // GPIO_CFG_HPP
