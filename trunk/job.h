#ifndef JOB_H
#define JOB_H

#include "image.h"

class Job
{
public:
    Job();

    virtual void run() = 0;
};

#endif // JOB_H
