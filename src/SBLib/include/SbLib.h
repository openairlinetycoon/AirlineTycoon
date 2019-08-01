#pragma once

#include <string>
#include <vector>

class SBStr
{
public:
    SBStr() : m_str() {}
    SBStr(std::string str) : m_str(str) {}
    SBStr(const char* str) : m_str(str) {}

    void SetAt(int i, char c) { m_str[i] = c; }
    SBStr& operator=(CString& str) { m_str = str; return *this; }

    SBStr Left(int nCount) { return m_str.substr(0,nCount); }
    SBStr Mid(int i, int nCount = std::string::npos) { return m_str.substr(i,nCount); }
    SBStr Right(int i) { return m_str.substr(m_str.size() - i, i); }

    friend SBStr operator+(const SBStr& a, const SBStr& b) { return a.m_str + b.m_str; }

    operator const char*() { return m_str.c_str(); }
    operator std::string() { return m_str; }

private:
    std::string m_str;
};

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
