#pragma once

#include <string>
#include <list>

class SBStr
{
public:
    SBStr() : m_str() {}
    SBStr(const SBStr& str) : m_str(str.m_str) {}
    SBStr(std::string& str) : m_str(str) {}
    SBStr(const char* str) : m_str(str) {}

    void SetAt(int i, char c) { m_str[i] = c; }
    SBStr& operator=(CString& str) { m_str = str; return *this; }

    bool operator==(SBStr& str) { return m_str == str.m_str; }
    bool operator==(const char* str) { return m_str == str; }

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
    SBList() : mList() { mIt = mList.end(); }

    T& Add(const T& elem)
    {
        mList.push_back(elem);
        return mList.back();
    }

    long GetNumberOfElements()
    {
        return mList.size();
    }

    T& Get(size_t i)
    {
        GetFirst();
        while (--i > 0)
            GetNext();
        return GetLastAccessed();
    }

    T& GetLastAccessed()
    {
        return *mIt;
    }

    void RemoveLastAccessed()
    {
        mList.erase(mIt);
    }

    void Clear()
    {
        if (!mList.empty())
           mList.clear();
    }

    T& GetFirst() { return *(mIt = mList.begin()); }
    T& GetNext() { return *mIt++; }
    bool IsLast() { return mIt == mList.end(); }

private:
    std::list<T> mList;
    typename std::list<T>::iterator mIt;
};
