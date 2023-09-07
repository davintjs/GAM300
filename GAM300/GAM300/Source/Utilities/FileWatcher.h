#include "Core/SystemInterface.h"
#include <thread>

#ifndef FILEWATCHER_H
#define FILEWATCHER_H

SINGLETON(FileWatcher)
{
public:
    FileWatcher();

    void ThreadWork();

    ~FileWatcher();

    void* hDir;
};

#endif