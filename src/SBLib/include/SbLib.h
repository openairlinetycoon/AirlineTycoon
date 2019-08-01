#pragma once

#include <vector>

typedef CString SBStr;

template <class T>
class SBList
{
public:
    SBList() : m_list(), m_it(m_list.end()) {}

    void Add(T elem)
    {
        m_list.push_back(elem);
        m_it = m_list.end();
    }

    long GetNumberOfElements()
    {
        return m_list.size();
    }

    T& Get(size_t i)
    {
        return m_list[i];
    }

    T& GetLastAccessed()
    {
        return *m_it;
    }

    void GetFirst() { m_it = m_list.begin(); }
    void GetLast() { m_it = m_list.end(); }
    void GetNext() { m_it++; }
    bool IsLast() { return m_it == m_list.end(); }

private:
    std::vector<T> m_list;
    std::vector<T>::iterator m_it;
};
