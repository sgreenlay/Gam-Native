#pragma once

#include "Malloc.h"
#include "Imports.h"

// <type_traits>
template<class T> struct remove_reference { using type = T; };
template<class T> struct remove_reference<T&> { using type = T; };
template<class T> struct remove_reference<T&&> { using type = T; };
template<class T> using remove_reference_t = typename remove_reference<T>::type;

template<class T>
constexpr T&& forward(remove_reference_t<T>& t) { return static_cast<T&&>(t); }

template<class T>
constexpr remove_reference_t<T>&& move(T&& t) { return static_cast<remove_reference_t<T>&&>(t); }
// </type_traits>

template<class T>
void swap(T& a, T& b)
{
    T temp(move(a));
    a = move(b);
    b = move(temp);
}

template<class T>
struct vector
{
    constexpr vector() : m_data(nullptr), m_size(nullptr), m_capacity(nullptr) {}
    constexpr vector(vector&& v) : m_data(v.m_data), m_size(v.m_size), m_capacity(v.m_capacity)
    {
        v.m_data = nullptr;
        v.m_size = nullptr;
        v.m_capacity = nullptr;
    }
    vector& operator=(vector&& v)
    {
        auto temp_data = v.m_data;
        auto temp_size = v.m_size;
        auto temp_cap = v.m_capacity;
        v.m_data = m_data;
        v.m_size = m_size;
        v.m_capacity = m_capacity;
        m_data = temp_data;
        m_size = temp_size;
        m_capacity = temp_cap;
        return *this;
    }
    ~vector()
    {
        for (auto& t : *this) t.~T();
        delete[] (char*)m_data;
    }

    vector(const vector&) = delete;
    vector& operator=(const vector&) = delete;

    T* begin() const { return m_data; }
    T* end() const { return m_size; }

    T& operator[](int i) { return m_data[i]; }
    const T& operator[](int i) const { return m_data[i]; }

    T& back() { return *(end() - 1); }

    void clear()
    {
        for (auto& t : *this)
            t.~T();
        m_size = m_data;
    }

    template<class...Xs>
    void resize(int target_size, Xs&...xs)
    {
        if (target_size > size())
        {
            reallocate_larger(target_size);
            for (auto a = m_size; a != m_capacity; ++a)
                new (a) T(xs...);
            m_size = m_capacity;
        }
        else if (target_size < size())
        {
            reallocate_smaller(target_size);
        }
    }

    constexpr int size() const { return m_size - m_data; }

    template<class...Xs>
    void emplace_back(Xs&&...xs)
    {
        if (m_size == m_capacity) reallocate_larger();

        new (m_size) T(forward<Xs>(xs)...);
        ++m_size;
    }

private:
    void reallocate_larger()
    {
        if (size() == 0)
            allocate(8);
        else
            reallocate_larger(size() * 2);
    }

    void reallocate_larger(int sz)
    {
        auto tmp_data = (T*)new char[sz * sizeof(T)];
        if (m_capacity)
        {
            for (auto a = m_data, b = tmp_data; a != m_size; ++a, ++b)
            {
                new (b) T(move(*a));
                a->~T();
            }
            delete[](char*)m_data;
        }

        auto cur_size = size();
        m_data = tmp_data;
        m_size = m_data + cur_size;
        m_capacity = m_data + sz;
    }

    void reallocate_smaller(int sz)
    {
        auto tmp_data = (T*)new char[sz * sizeof(T)];
        if (m_capacity)
        {
            auto a = m_data;
            for (auto b = tmp_data; b != tmp_data + sz; ++a, ++b)
            {
                new (b) T(move(*a));
                a->~T();
            }
            // destroy the rest
            for (; a != m_size; ++a)
            {
                a->~T();
            }
            delete[](char*)m_data;
        }

        m_data = tmp_data;
        m_size = m_data + sz;
        m_capacity = m_data + sz;
    }

    // only call when m_data is nullptr.
    void allocate(int new_size)
    {
        m_data = (T*)new char[new_size * sizeof(T)];
        m_size = m_data;
        m_capacity = m_data + new_size;
    }

    T * m_data;
    T * m_size;
    T * m_capacity;
};

// <span>
template<class T>
struct span
{
    constexpr span() : m_ptr(nullptr), m_size(0) {}
    constexpr span(T* p, int s) : m_ptr(p), m_size(s) {}

    constexpr int size() const { return m_size; }

    T* begin() const { return m_ptr; }
    T* end() const { return m_ptr + m_size; }

private:
    T * m_ptr;
    int m_size;
};
// </span>