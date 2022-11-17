#include "stdafx.h"

#include <stdio.h>
#include <stdexcept>

const char* ExcAssert           = "Assert (called from %s:%li) failed!";
const char* ExcGuardian         = "Con/Des guardian %lx failed!";
const char* ExcStrangeMem       = "Strange new: %li bytes!";
const char* ExcOutOfMem         = "Couldn't allocate %li bytes!";
const char* ExcNotImplemented   = "Function not implemented!";
const char* ExcImpossible       = "Impossible Event %s occured";

HDU Hdu;

HDU::HDU()
    : Log(NULL)
{
    char* base = SDL_GetBasePath();
    const char* file = "debug.txt";
    BUFFER<char> path(strlen(base) + strlen(file) + 1);
    strcpy(path, base);
    strcat(path, file);
    Log = fopen(path, "w");

    SDL_LogOutputFunction defaultOut;
    SDL_LogGetOutputFunction(&defaultOut, nullptr);

#ifdef _DEBUG
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif

    auto func = [](void* userdata, int category, SDL_LogPriority priority, const char* message) {
        char* finalMessage = const_cast<char*>(message);

        bool modified = false;
        if (strstr(message, "||") == nullptr) {
            const unsigned long long size = strlen(message) + strlen("Misc || ") + 1;
            finalMessage = new char[size]{};
            sprintf_s(finalMessage, size, "Misc || %s", message);
            modified = true;
        }

        SDL_LogOutputFunction func = (SDL_LogOutputFunction)(userdata);
        func(userdata, category, priority, finalMessage);

        if(Hdu.Log){
            fprintf(Hdu.Log, "%s\n", finalMessage);
            fflush(Hdu.Log);
        }

        if(modified)
            delete[] finalMessage;
    };

    SDL_LogSetOutputFunction(func, defaultOut);

}

HDU::~HDU()
{
    Close();
}

void HDU::Close()
{
    if (Log)
        fclose (Log);
    Log = NULL;
}

void HDU::HercPrintf(int, const char* format, ...)
{
    if (!Log)
        return;
    va_list args;
    va_start(args, format);
    //vfprintf(Log, format, args);
    V_AT_Log_I("Herc",format, args);
    va_end(args);
}

void HDU::HercPrintf(const char* format, ...)
{
    if (!Log)
        return;
    va_list args;
    va_start(args, format);
    //vfprintf(Log, format, args);
    V_AT_Log_I("Herc", format, args);
    va_end(args);
}

void here(char *file, SLONG line)
{
    Hdu.HercPrintf(0, "Here in %s, line %li", file, line);
}

SLONG TeakLibW_Exception(char* file, SLONG line, const char* format, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, format, args);
    va_end(args);
    //MessageBeep(0);
    //MessageBeep(0x30u);
    Hdu.HercPrintf(1, "====================================================================");
    Hdu.HercPrintf(0, "Exception in File %s, Line %li:", file, line);
    Hdu.HercPrintf(0, buffer);
    Hdu.HercPrintf(1, "====================================================================");
    Hdu.HercPrintf(0, "C++ Exception thrown. Program will probably be terminated.");
    Hdu.Close();
    //DebugBreak();
    throw std::runtime_error(buffer);
}
