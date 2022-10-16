#ifndef INTFLOATCONVERSIONS_H
#define INTFLOATCONVERSIONS_H

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

template <typename T>
constexpr T get_zero()
{
    if (std::is_unsigned<T>())
    {
        return static_cast<T>((std::numeric_limits<T>::max() / 2) + 1);
    }
    return 0;
}

template <typename T>
constexpr inline T clip_float(T value)
{
    if (value > 1.0) return 1.0;
    else if (value < -1.0) return -1.0;
    return value;
}

template <typename T>
constexpr inline void clip_float(T* value)
{
    *value = clip_float<T>(*value);
}

template <typename T>
constexpr inline T roundf(T value)
{
    int truncated(static_cast<int>(value));
    T remainder(value - static_cast<T>(truncated));
    int roundValue(
            (remainder > 0.5)
            || ((remainder == 0.5) && !(static_cast<int>(value + 1) % 2))
        );
    return static_cast<T>(truncated + ((value < 0) ? -roundValue : roundValue));
}

template <typename F, typename I>
constexpr I float_to_int(F value)
{
    if (value == 0.0) return get_zero<I>();
    else if (std::is_unsigned<I>())
    {
        if (value < 0)
        {
            return static_cast<I>(roundf<F>(
                    static_cast<F>(get_zero<I>())
                    + (value * static_cast<F>(get_zero<I>()))
                ));
        }
        return static_cast<I>(
                (value * static_cast<F>(get_zero<I>() - 1))
                + static_cast<F>(get_zero<I>())
            );
    }
    else if (value < 0)
    {
        return static_cast<I>(
                roundf<F>(value * static_cast<F>(std::numeric_limits<I>::min()))
            );
    }
    return static_cast<I>(
            roundf<F>(value * static_cast<F>(std::numeric_limits<I>::max()))
        );
}

// template <typename F, typename I>
// constexpr I float_to_int(F value)
// {
//     if (value == 0.0) return get_zero<I>();
//     else if (std::is_unsigned<I>())
//     {
//         if (value < 0)
//         {
//             return static_cast<I>(std::round(
//                     static_cast<F>(get_zero<I>())
//                     + (value * static_cast<F>(get_zero<I>()))
//                 ));
//         }
//         return static_cast<I>(
//                 (value * (get_zero<I>() - 1))
//                 + get_zero<I>()
//             );
//     }
//     else if (value < 0)
//     {
//         return static_cast<I>(
//                 std::round(value * std::numeric_limits<I>::min())
//             );
//     }
//     return static_cast<I>(
//             std::round(value * std::numeric_limits<I>::max())
//         );
// }

template <typename F, typename I>
constexpr void float_to_int(I* converted, F* value)
{
    *converted = float_to_int<F, I>(*value);
}

template <typename F, typename I>
constexpr void float_to_int(I* converted, F* value, int length)
{
    for (int i(0); i < length; ++i)
    {
        float_to_int<F, I>(converted + i, value + i);
    }
}

template <typename F, typename I>
constexpr std::vector<I> float_to_int(std::vector<F> values)
{
    const int length = static_cast<int>(values.size());
    std::vector<I> converted;
    converted.reserve(length);
    for (int i(0); i < length; ++i)
    {
        converted.emplace_back(float_to_int<F, I>(values.at(i)));
    }
    return converted;
}

template <typename F, typename I>
constexpr void float_to_int(std::vector<I>* converted, std::vector<F>* values)
{
    const int length = static_cast<int>(values->size());
    for (int i(0); i < length; ++i)
    {
        float_to_int<F, I>(&((*converted)[i]), &((*values)[i]));
    }
}

template <typename F, typename I>
constexpr void float_to_int(std::vector<I>* converted, std::vector<F>* values, int length)
{
    for (int i(0); i < length; ++i)
    {
        float_to_int<F, I>(&((*converted)[i]), &((*values)[i]));
    }
}

template <typename I, typename F>
constexpr F int_to_float(I value)
{
    if (value == get_zero<I>()) return 0.0;
    else if (std::is_unsigned<I>())
    {
        if (value < get_zero<I>())
        {
            return (
                    static_cast<F>(value)
                    / static_cast<F>(get_zero<I>() - 1)
                );
        }
        return (
                static_cast<F>(value - get_zero<I>())
                / static_cast<F>(get_zero<I>())
            );
    }
    else if (value < 0)
    {
        return static_cast<F>(value) * static_cast<F>(std::numeric_limits<I>::max());
    }
    return static_cast<F>(value) * static_cast<F>(std::numeric_limits<I>::min());
}

template <typename I, typename F>
constexpr void int_to_float(F* converted, I* value)
{
    *converted = int_to_float<I, F>(*value);
}

template <typename I, typename F>
constexpr void int_to_float(F* converted, I* value, int length)
{
    for (int i(0); i < length; ++i)
    {
        int_to_float<I, F>(converted + i, value + i);
    }
}

template <typename I, typename F>
constexpr std::vector<F> int_to_float(std::vector<I> values)
{
    const int length = static_cast<int>(values.size());
    std::vector<F> converted;
    converted.reserve(length);
    for (int i(0); i < length; ++i)
    {
        converted.emplace_back(int_to_float<I, F>(values.at(i)));
    }
    return converted;
}

template <typename I, typename F>
constexpr void int_to_float(std::vector<F>* converted, std::vector<I>* values)
{
    const int length = static_cast<int>(values->size());
    for (int i(0); i < length; ++i)
    {
        int_to_float<I, F>(&((*converted)[i]), &((*values)[i]));
    }
}

template <typename I, typename F>
constexpr void int_to_float(std::vector<F>* converted, std::vector<I>* values, int length)
{
    for (int i(0); i < length; ++i)
    {
        int_to_float<I, F>(&((*converted)[i]), &((*values)[i]));
    }
}

template float get_zero<float>();
template double get_zero<double>();
template long double get_zero<long double>();
template int8_t get_zero<int8_t>();
template uint8_t get_zero<uint8_t>();
template int16_t get_zero<int16_t>();
template uint16_t get_zero<uint16_t>();
template int32_t get_zero<int32_t>();
template uint32_t get_zero<uint32_t>();
template int64_t get_zero<int64_t>();
template uint64_t get_zero<uint64_t>();

template float clip_float<float>(float);
template double clip_float<double>(double);
template long double clip_float<long double>(long double);

template void clip_float<float>(float*);
template void clip_float<double>(double*);
template void clip_float<long double>(long double*);

template float roundf<float>(float);
template double roundf<double>(double);
template long double roundf<long double>(long double);

template int8_t float_to_int<float, int8_t>(float);
template uint8_t float_to_int<float, uint8_t>(float);
template int16_t float_to_int<float, int16_t>(float);
template uint16_t float_to_int<float, uint16_t>(float);
template int32_t float_to_int<float, int32_t>(float);
template uint32_t float_to_int<float, uint32_t>(float);
template int64_t float_to_int<float, int64_t>(float);
template uint64_t float_to_int<float, uint64_t>(float);

template int float_to_int<float, int>(float);

template int8_t float_to_int<double, int8_t>(double);
template uint8_t float_to_int<double, uint8_t>(double);
template int16_t float_to_int<double, int16_t>(double);
template uint16_t float_to_int<double, uint16_t>(double);
template int32_t float_to_int<double, int32_t>(double);
template uint32_t float_to_int<double, uint32_t>(double);
template int64_t float_to_int<double, int64_t>(double);
template uint64_t float_to_int<double, uint64_t>(double);

template int float_to_int<double, int>(double);

template int8_t float_to_int<long double, int8_t>(long double);
template uint8_t float_to_int<long double, uint8_t>(long double);
template int16_t float_to_int<long double, int16_t>(long double);
template uint16_t float_to_int<long double, uint16_t>(long double);
template int32_t float_to_int<long double, int32_t>(long double);
template uint32_t float_to_int<long double, uint32_t>(long double);
template int64_t float_to_int<long double, int64_t>(long double);
template uint64_t float_to_int<long double, uint64_t>(long double);

template int float_to_int<long double, int>(long double);

template void float_to_int<float, int8_t>(int8_t*, float*);
template void float_to_int<float, uint8_t>(uint8_t*, float*);
template void float_to_int<float, int16_t>(int16_t*, float*);
template void float_to_int<float, uint16_t>(uint16_t*, float*);
template void float_to_int<float, int32_t>(int32_t*, float*);
template void float_to_int<float, uint32_t>(uint32_t*, float*);
template void float_to_int<float, int64_t>(int64_t*, float*);
template void float_to_int<float, uint64_t>(uint64_t*, float*);

template void float_to_int<float, int>(int*, float*);

template void float_to_int<double, int8_t>(int8_t*, double*);
template void float_to_int<double, uint8_t>(uint8_t*, double*);
template void float_to_int<double, int16_t>(int16_t*, double*);
template void float_to_int<double, uint16_t>(uint16_t*, double*);
template void float_to_int<double, int32_t>(int32_t*, double*);
template void float_to_int<double, uint32_t>(uint32_t*, double*);
template void float_to_int<double, int64_t>(int64_t*, double*);
template void float_to_int<double, uint64_t>(uint64_t*, double*);

template void float_to_int<double, int>(int*, double*);

template void float_to_int<long double, int8_t>(int8_t*, long double*);
template void float_to_int<long double, uint8_t>(uint8_t*, long double*);
template void float_to_int<long double, int16_t>(int16_t*, long double*);
template void float_to_int<long double, uint16_t>(uint16_t*, long double*);
template void float_to_int<long double, int32_t>(int32_t*, long double*);
template void float_to_int<long double, uint32_t>(uint32_t*, long double*);
template void float_to_int<long double, int64_t>(int64_t*, long double*);
template void float_to_int<long double, uint64_t>(uint64_t*, long double*);

template void float_to_int<long double, int>(int*, long double*);

template void float_to_int<float, int8_t>(int8_t*, float*, int);
template void float_to_int<float, uint8_t>(uint8_t*, float*, int);
template void float_to_int<float, int16_t>(int16_t*, float*, int);
template void float_to_int<float, uint16_t>(uint16_t*, float*, int);
template void float_to_int<float, int32_t>(int32_t*, float*, int);
template void float_to_int<float, uint32_t>(uint32_t*, float*, int);
template void float_to_int<float, int64_t>(int64_t*, float*, int);
template void float_to_int<float, uint64_t>(uint64_t*, float*, int);

template void float_to_int<float, int>(int*, float*, int);

template void float_to_int<double, int8_t>(int8_t*, double*, int);
template void float_to_int<double, uint8_t>(uint8_t*, double*, int);
template void float_to_int<double, int16_t>(int16_t*, double*, int);
template void float_to_int<double, uint16_t>(uint16_t*, double*, int);
template void float_to_int<double, int32_t>(int32_t*, double*, int);
template void float_to_int<double, uint32_t>(uint32_t*, double*, int);
template void float_to_int<double, int64_t>(int64_t*, double*, int);
template void float_to_int<double, uint64_t>(uint64_t*, double*, int);

template void float_to_int<double, int>(int*, double*, int);

template void float_to_int<long double, int8_t>(int8_t*, long double*, int);
template void float_to_int<long double, uint8_t>(uint8_t*, long double*, int);
template void float_to_int<long double, int16_t>(int16_t*, long double*, int);
template void float_to_int<long double, uint16_t>(uint16_t*, long double*, int);
template void float_to_int<long double, int32_t>(int32_t*, long double*, int);
template void float_to_int<long double, uint32_t>(uint32_t*, long double*, int);
template void float_to_int<long double, int64_t>(int64_t*, long double*, int);
template void float_to_int<long double, uint64_t>(uint64_t*, long double*, int);

template void float_to_int<long double, int>(int*, long double*, int);

template std::vector<int8_t> float_to_int<float, int8_t>(std::vector<float>);
template std::vector<uint8_t> float_to_int<float, uint8_t>(std::vector<float>);
template std::vector<int16_t> float_to_int<float, int16_t>(std::vector<float>);
template std::vector<uint16_t> float_to_int<float, uint16_t>(std::vector<float>);
template std::vector<int32_t> float_to_int<float, int32_t>(std::vector<float>);
template std::vector<uint32_t> float_to_int<float, uint32_t>(std::vector<float>);
template std::vector<int64_t> float_to_int<float, int64_t>(std::vector<float>);
template std::vector<uint64_t> float_to_int<float, uint64_t>(std::vector<float>);

template std::vector<int> float_to_int<float, int>(std::vector<float>);

template std::vector<int8_t> float_to_int<double, int8_t>(std::vector<double>);
template std::vector<uint8_t> float_to_int<double, uint8_t>(std::vector<double>);
template std::vector<int16_t> float_to_int<double, int16_t>(std::vector<double>);
template std::vector<uint16_t> float_to_int<double, uint16_t>(std::vector<double>);
template std::vector<int32_t> float_to_int<double, int32_t>(std::vector<double>);
template std::vector<uint32_t> float_to_int<double, uint32_t>(std::vector<double>);
template std::vector<int64_t> float_to_int<double, int64_t>(std::vector<double>);
template std::vector<uint64_t> float_to_int<double, uint64_t>(std::vector<double>);

template std::vector<int> float_to_int<double, int>(std::vector<double>);

template std::vector<int8_t> float_to_int<long double, int8_t>(std::vector<long double>);
template std::vector<uint8_t> float_to_int<long double, uint8_t>(std::vector<long double>);
template std::vector<int16_t> float_to_int<long double, int16_t>(std::vector<long double>);
template std::vector<uint16_t> float_to_int<long double, uint16_t>(std::vector<long double>);
template std::vector<int32_t> float_to_int<long double, int32_t>(std::vector<long double>);
template std::vector<uint32_t> float_to_int<long double, uint32_t>(std::vector<long double>);
template std::vector<int64_t> float_to_int<long double, int64_t>(std::vector<long double>);
template std::vector<uint64_t> float_to_int<long double, uint64_t>(std::vector<long double>);

template std::vector<int> float_to_int<long double, int>(std::vector<long double>);

template void float_to_int<float, int8_t>(std::vector<int8_t>*, std::vector<float>*);
template void float_to_int<float, uint8_t>(std::vector<uint8_t>*, std::vector<float>*);
template void float_to_int<float, int16_t>(std::vector<int16_t>*, std::vector<float>*);
template void float_to_int<float, uint16_t>(std::vector<uint16_t>*, std::vector<float>*);
template void float_to_int<float, int32_t>(std::vector<int32_t>*, std::vector<float>*);
template void float_to_int<float, uint32_t>(std::vector<uint32_t>*, std::vector<float>*);
template void float_to_int<float, int64_t>(std::vector<int64_t>*, std::vector<float>*);
template void float_to_int<float, uint64_t>(std::vector<uint64_t>*, std::vector<float>*);

template void float_to_int<float, int>(std::vector<int>*, std::vector<float>*);

template void float_to_int<double, int8_t>(std::vector<int8_t>*, std::vector<double>*);
template void float_to_int<double, uint8_t>(std::vector<uint8_t>*, std::vector<double>*);
template void float_to_int<double, int16_t>(std::vector<int16_t>*, std::vector<double>*);
template void float_to_int<double, uint16_t>(std::vector<uint16_t>*, std::vector<double>*);
template void float_to_int<double, int32_t>(std::vector<int32_t>*, std::vector<double>*);
template void float_to_int<double, uint32_t>(std::vector<uint32_t>*, std::vector<double>*);
template void float_to_int<double, int64_t>(std::vector<int64_t>*, std::vector<double>*);
template void float_to_int<double, uint64_t>(std::vector<uint64_t>*, std::vector<double>*);

template void float_to_int<double, int>(std::vector<int>*, std::vector<double>*);

template void float_to_int<long double, int8_t>(std::vector<int8_t>*, std::vector<long double>*);
template void float_to_int<long double, uint8_t>(std::vector<uint8_t>*, std::vector<long double>*);
template void float_to_int<long double, int16_t>(std::vector<int16_t>*, std::vector<long double>*);
template void float_to_int<long double, uint16_t>(std::vector<uint16_t>*, std::vector<long double>*);
template void float_to_int<long double, int32_t>(std::vector<int32_t>*, std::vector<long double>*);
template void float_to_int<long double, uint32_t>(std::vector<uint32_t>*, std::vector<long double>*);
template void float_to_int<long double, int64_t>(std::vector<int64_t>*, std::vector<long double>*);
template void float_to_int<long double, uint64_t>(std::vector<uint64_t>*, std::vector<long double>*);

template void float_to_int<long double, int>(std::vector<int>*, std::vector<long double>*);

template void float_to_int<float, int8_t>(std::vector<int8_t>*, std::vector<float>*, int);
template void float_to_int<float, uint8_t>(std::vector<uint8_t>*, std::vector<float>*, int);
template void float_to_int<float, int16_t>(std::vector<int16_t>*, std::vector<float>*, int);
template void float_to_int<float, uint16_t>(std::vector<uint16_t>*, std::vector<float>*, int);
template void float_to_int<float, int32_t>(std::vector<int32_t>*, std::vector<float>*, int);
template void float_to_int<float, uint32_t>(std::vector<uint32_t>*, std::vector<float>*, int);
template void float_to_int<float, int64_t>(std::vector<int64_t>*, std::vector<float>*, int);
template void float_to_int<float, uint64_t>(std::vector<uint64_t>*, std::vector<float>*, int);

template void float_to_int<float, int>(std::vector<int>*, std::vector<float>*, int);

template void float_to_int<double, int8_t>(std::vector<int8_t>*, std::vector<double>*, int);
template void float_to_int<double, uint8_t>(std::vector<uint8_t>*, std::vector<double>*, int);
template void float_to_int<double, int16_t>(std::vector<int16_t>*, std::vector<double>*, int);
template void float_to_int<double, uint16_t>(std::vector<uint16_t>*, std::vector<double>*, int);
template void float_to_int<double, int32_t>(std::vector<int32_t>*, std::vector<double>*, int);
template void float_to_int<double, uint32_t>(std::vector<uint32_t>*, std::vector<double>*, int);
template void float_to_int<double, int64_t>(std::vector<int64_t>*, std::vector<double>*, int);
template void float_to_int<double, uint64_t>(std::vector<uint64_t>*, std::vector<double>*, int);

template void float_to_int<double, int>(std::vector<int>*, std::vector<double>*, int);

template void float_to_int<long double, int8_t>(std::vector<int8_t>*, std::vector<long double>*, int);
template void float_to_int<long double, uint8_t>(std::vector<uint8_t>*, std::vector<long double>*, int);
template void float_to_int<long double, int16_t>(std::vector<int16_t>*, std::vector<long double>*, int);
template void float_to_int<long double, uint16_t>(std::vector<uint16_t>*, std::vector<long double>*, int);
template void float_to_int<long double, int32_t>(std::vector<int32_t>*, std::vector<long double>*, int);
template void float_to_int<long double, uint32_t>(std::vector<uint32_t>*, std::vector<long double>*, int);
template void float_to_int<long double, int64_t>(std::vector<int64_t>*, std::vector<long double>*, int);
template void float_to_int<long double, uint64_t>(std::vector<uint64_t>*, std::vector<long double>*, int);

template void float_to_int<long double, int>(std::vector<int>*, std::vector<long double>*, int);

template float int_to_float<int8_t, float>(int8_t);
template float int_to_float<uint8_t, float>(uint8_t);
template float int_to_float<int16_t, float>(int16_t);
template float int_to_float<uint16_t, float>(uint16_t);
template float int_to_float<int32_t, float>(int32_t);
template float int_to_float<uint32_t, float>(uint32_t);
template float int_to_float<int64_t, float>(int64_t);
template float int_to_float<uint64_t, float>(uint64_t);

template float int_to_float<int, float>(int);

template double int_to_float<int8_t, double>(int8_t);
template double int_to_float<uint8_t, double>(uint8_t);
template double int_to_float<int16_t, double>(int16_t);
template double int_to_float<uint16_t, double>(uint16_t);
template double int_to_float<int32_t, double>(int32_t);
template double int_to_float<uint32_t, double>(uint32_t);
template double int_to_float<int64_t, double>(int64_t);
template double int_to_float<uint64_t, double>(uint64_t);

template double int_to_float<int, double>(int);

template long double int_to_float<int8_t, long double>(int8_t);
template long double int_to_float<uint8_t, long double>(uint8_t);
template long double int_to_float<int16_t, long double>(int16_t);
template long double int_to_float<uint16_t, long double>(uint16_t);
template long double int_to_float<int32_t, long double>(int32_t);
template long double int_to_float<uint32_t, long double>(uint32_t);
template long double int_to_float<int64_t, long double>(int64_t);
template long double int_to_float<uint64_t, long double>(uint64_t);

template long double int_to_float<int, long double>(int);

template void int_to_float<int8_t, float>(float*, int8_t*);
template void int_to_float<uint8_t, float>(float*, uint8_t*);
template void int_to_float<int16_t, float>(float*, int16_t*);
template void int_to_float<uint16_t, float>(float*, uint16_t*);
template void int_to_float<int32_t, float>(float*, int32_t*);
template void int_to_float<uint32_t, float>(float*, uint32_t*);
template void int_to_float<int64_t, float>(float*, int64_t*);
template void int_to_float<uint64_t, float>(float*, uint64_t*);

template void int_to_float<int, float>(float*, int*);

template void int_to_float<int8_t, double>(double*, int8_t*);
template void int_to_float<uint8_t, double>(double*, uint8_t*);
template void int_to_float<int16_t, double>(double*, int16_t*);
template void int_to_float<uint16_t, double>(double*, uint16_t*);
template void int_to_float<int32_t, double>(double*, int32_t*);
template void int_to_float<uint32_t, double>(double*, uint32_t*);
template void int_to_float<int64_t, double>(double*, int64_t*);
template void int_to_float<uint64_t, double>(double*, uint64_t*);

template void int_to_float<int, double>(double*, int*);

template void int_to_float<int8_t, long double>(long double*, int8_t*);
template void int_to_float<uint8_t, long double>(long double*, uint8_t*);
template void int_to_float<int16_t, long double>(long double*, int16_t*);
template void int_to_float<uint16_t, long double>(long double*, uint16_t*);
template void int_to_float<int32_t, long double>(long double*, int32_t*);
template void int_to_float<uint32_t, long double>(long double*, uint32_t*);
template void int_to_float<int64_t, long double>(long double*, int64_t*);
template void int_to_float<uint64_t, long double>(long double*, uint64_t*);

template void int_to_float<int, long double>(long double*, int*);

template void int_to_float<int8_t, float>(float*, int8_t*, int);
template void int_to_float<uint8_t, float>(float*, uint8_t*, int);
template void int_to_float<int16_t, float>(float*, int16_t*, int);
template void int_to_float<uint16_t, float>(float*, uint16_t*, int);
template void int_to_float<int32_t, float>(float*, int32_t*, int);
template void int_to_float<uint32_t, float>(float*, uint32_t*, int);
template void int_to_float<int64_t, float>(float*, int64_t*, int);
template void int_to_float<uint64_t, float>(float*, uint64_t*, int);

template void int_to_float<int, float>(float*, int*, int);

template void int_to_float<int8_t, double>(double*, int8_t*, int);
template void int_to_float<uint8_t, double>(double*, uint8_t*, int);
template void int_to_float<int16_t, double>(double*, int16_t*, int);
template void int_to_float<uint16_t, double>(double*, uint16_t*, int);
template void int_to_float<int32_t, double>(double*, int32_t*, int);
template void int_to_float<uint32_t, double>(double*, uint32_t*, int);
template void int_to_float<int64_t, double>(double*, int64_t*, int);
template void int_to_float<uint64_t, double>(double*, uint64_t*, int);

template void int_to_float<int, double>(double*, int*, int);

template void int_to_float<int8_t, long double>(long double*, int8_t*, int);
template void int_to_float<uint8_t, long double>(long double*, uint8_t*, int);
template void int_to_float<int16_t, long double>(long double*, int16_t*, int);
template void int_to_float<uint16_t, long double>(long double*, uint16_t*, int);
template void int_to_float<int32_t, long double>(long double*, int32_t*, int);
template void int_to_float<uint32_t, long double>(long double*, uint32_t*, int);
template void int_to_float<int64_t, long double>(long double*, int64_t*, int);
template void int_to_float<uint64_t, long double>(long double*, uint64_t*, int);

template void int_to_float<int, long double>(long double*, int*, int);

template std::vector<float> int_to_float<int8_t, float>(std::vector<int8_t>);
template std::vector<float> int_to_float<uint8_t, float>(std::vector<uint8_t>);
template std::vector<float> int_to_float<int16_t, float>(std::vector<int16_t>);
template std::vector<float> int_to_float<uint16_t, float>(std::vector<uint16_t>);
template std::vector<float> int_to_float<int32_t, float>(std::vector<int32_t>);
template std::vector<float> int_to_float<uint32_t, float>(std::vector<uint32_t>);
template std::vector<float> int_to_float<int64_t, float>(std::vector<int64_t>);
template std::vector<float> int_to_float<uint64_t, float>(std::vector<uint64_t>);

template std::vector<float> int_to_float<int, float>(std::vector<int>);

template std::vector<double> int_to_float<int8_t, double>(std::vector<int8_t>);
template std::vector<double> int_to_float<uint8_t, double>(std::vector<uint8_t>);
template std::vector<double> int_to_float<int16_t, double>(std::vector<int16_t>);
template std::vector<double> int_to_float<uint16_t, double>(std::vector<uint16_t>);
template std::vector<double> int_to_float<int32_t, double>(std::vector<int32_t>);
template std::vector<double> int_to_float<uint32_t, double>(std::vector<uint32_t>);
template std::vector<double> int_to_float<int64_t, double>(std::vector<int64_t>);
template std::vector<double> int_to_float<uint64_t, double>(std::vector<uint64_t>);

template std::vector<double> int_to_float<int, double>(std::vector<int>);

template std::vector<long double> int_to_float<int8_t, long double>(std::vector<int8_t>);
template std::vector<long double> int_to_float<uint8_t, long double>(std::vector<uint8_t>);
template std::vector<long double> int_to_float<int16_t, long double>(std::vector<int16_t>);
template std::vector<long double> int_to_float<uint16_t, long double>(std::vector<uint16_t>);
template std::vector<long double> int_to_float<int32_t, long double>(std::vector<int32_t>);
template std::vector<long double> int_to_float<uint32_t, long double>(std::vector<uint32_t>);
template std::vector<long double> int_to_float<int64_t, long double>(std::vector<int64_t>);
template std::vector<long double> int_to_float<uint64_t, long double>(std::vector<uint64_t>);

template std::vector<long double> int_to_float<int, long double>(std::vector<int>);

template void int_to_float<int8_t, float>(std::vector<float>*, std::vector<int8_t>*);
template void int_to_float<uint8_t, float>(std::vector<float>*, std::vector<uint8_t>*);
template void int_to_float<int16_t, float>(std::vector<float>*, std::vector<int16_t>*);
template void int_to_float<uint16_t, float>(std::vector<float>*, std::vector<uint16_t>*);
template void int_to_float<int32_t, float>(std::vector<float>*, std::vector<int32_t>*);
template void int_to_float<uint32_t, float>(std::vector<float>*, std::vector<uint32_t>*);
template void int_to_float<int64_t, float>(std::vector<float>*, std::vector<int64_t>*);
template void int_to_float<uint64_t, float>(std::vector<float>*, std::vector<uint64_t>*);

template void int_to_float<int, float>(std::vector<float>*, std::vector<int>*);

template void int_to_float<int8_t, double>(std::vector<double>*, std::vector<int8_t>*);
template void int_to_float<uint8_t, double>(std::vector<double>*, std::vector<uint8_t>*);
template void int_to_float<int16_t, double>(std::vector<double>*, std::vector<int16_t>*);
template void int_to_float<uint16_t, double>(std::vector<double>*, std::vector<uint16_t>*);
template void int_to_float<int32_t, double>(std::vector<double>*, std::vector<int32_t>*);
template void int_to_float<uint32_t, double>(std::vector<double>*, std::vector<uint32_t>*);
template void int_to_float<int64_t, double>(std::vector<double>*, std::vector<int64_t>*);
template void int_to_float<uint64_t, double>(std::vector<double>*, std::vector<uint64_t>*);

template void int_to_float<int, double>(std::vector<double>*, std::vector<int>*);

template void int_to_float<int8_t, long double>(std::vector<long double>*, std::vector<int8_t>*);
template void int_to_float<uint8_t, long double>(std::vector<long double>*, std::vector<uint8_t>*);
template void int_to_float<int16_t, long double>(std::vector<long double>*, std::vector<int16_t>*);
template void int_to_float<uint16_t, long double>(std::vector<long double>*, std::vector<uint16_t>*);
template void int_to_float<int32_t, long double>(std::vector<long double>*, std::vector<int32_t>*);
template void int_to_float<uint32_t, long double>(std::vector<long double>*, std::vector<uint32_t>*);
template void int_to_float<int64_t, long double>(std::vector<long double>*, std::vector<int64_t>*);
template void int_to_float<uint64_t, long double>(std::vector<long double>*, std::vector<uint64_t>*);

template void int_to_float<int, long double>(std::vector<long double>*, std::vector<int>*);

template void int_to_float<int8_t, float>(std::vector<float>*, std::vector<int8_t>*, int);
template void int_to_float<uint8_t, float>(std::vector<float>*, std::vector<uint8_t>*, int);
template void int_to_float<int16_t, float>(std::vector<float>*, std::vector<int16_t>*, int);
template void int_to_float<uint16_t, float>(std::vector<float>*, std::vector<uint16_t>*, int);
template void int_to_float<int32_t, float>(std::vector<float>*, std::vector<int32_t>*, int);
template void int_to_float<uint32_t, float>(std::vector<float>*, std::vector<uint32_t>*, int);
template void int_to_float<int64_t, float>(std::vector<float>*, std::vector<int64_t>*, int);
template void int_to_float<uint64_t, float>(std::vector<float>*, std::vector<uint64_t>*, int);

template void int_to_float<int, float>(std::vector<float>*, std::vector<int>*, int);

template void int_to_float<int8_t, double>(std::vector<double>*, std::vector<int8_t>*, int);
template void int_to_float<uint8_t, double>(std::vector<double>*, std::vector<uint8_t>*, int);
template void int_to_float<int16_t, double>(std::vector<double>*, std::vector<int16_t>*, int);
template void int_to_float<uint16_t, double>(std::vector<double>*, std::vector<uint16_t>*, int);
template void int_to_float<int32_t, double>(std::vector<double>*, std::vector<int32_t>*, int);
template void int_to_float<uint32_t, double>(std::vector<double>*, std::vector<uint32_t>*, int);
template void int_to_float<int64_t, double>(std::vector<double>*, std::vector<int64_t>*, int);
template void int_to_float<uint64_t, double>(std::vector<double>*, std::vector<uint64_t>*, int);

template void int_to_float<int, double>(std::vector<double>*, std::vector<int>*, int);

template void int_to_float<int8_t, long double>(std::vector<long double>*, std::vector<int8_t>*, int);
template void int_to_float<uint8_t, long double>(std::vector<long double>*, std::vector<uint8_t>*, int);
template void int_to_float<int16_t, long double>(std::vector<long double>*, std::vector<int16_t>*, int);
template void int_to_float<uint16_t, long double>(std::vector<long double>*, std::vector<uint16_t>*, int);
template void int_to_float<int32_t, long double>(std::vector<long double>*, std::vector<int32_t>*, int);
template void int_to_float<uint32_t, long double>(std::vector<long double>*, std::vector<uint32_t>*, int);
template void int_to_float<int64_t, long double>(std::vector<long double>*, std::vector<int64_t>*, int);
template void int_to_float<uint64_t, long double>(std::vector<long double>*, std::vector<uint64_t>*, int);

template void int_to_float<int, long double>(std::vector<long double>*, std::vector<int>*, int);

#endif
