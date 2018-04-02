#include "Parser.h"

namespace
{
    struct string_span
    {
        constexpr string_span() : m_str(nullptr), m_len(0) {}
        constexpr string_span(const char* p, size_t l) : m_str(p), m_len(l) {}

        string_span take(int n) const
        {
            return { m_str, m_len > n ? n : m_len };
        }

        string_span drop(int n) const
        {
            auto dropped = m_len > n ? n : m_len;
            return { m_str + dropped, m_len - dropped };
        }

        template<size_t Sz>
        bool equals(char(&arr)[Sz]) const
        {
            static_assert(Sz > 0, "");
            if (Sz - 1 != m_len) return false;
            return memcmp(arr, m_str, Sz-1) == 0;
        }

    private:
        const char* m_str;
        size_t m_len;
    };

    ruleset parse_ruleset(string_span ss)
    {
        return {};
    }
}

vector<ruleset> parse(const char* str, int len)
{
    vector<ruleset> ret;



    return ret;
}
