#include "Core/SystemInterface.h"

#ifndef FILEWATCHER_H
#define FILEWATCHER_H

class FileWatcher
{
public:
    FileWatcher();

    void ThreadWork();

    ~FileWatcher();

    void* hDir;
};

#endif