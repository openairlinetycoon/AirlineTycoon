#pragma once

typedef CString SBStr;

template <class T>
class SBList
{
public:
    long GetNumberOfElements()
    {
        DebugBreak();
        return 0;
    }

    T Get(size_t i)
    {
        DebugBreak();
        return T();
    }

    T GetLastAccessed()
    {
        DebugBreak();
        return T();
    }

    void GetFirst() { DebugBreak(); }
    void GetLast() { DebugBreak(); }
    void GetNext() { DebugBreak(); }
    bool IsLast() { DebugBreak(); return false; }
};
