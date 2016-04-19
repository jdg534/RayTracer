#ifndef _FIBERS_ASSIST_H_
#define _FIBERS_ASSIST_H_

#include <sce_fiber.h>
#include <libsysmodule.h>

#define MAX_FIBERS 7 // calls to std::thread::hardware_concurrency gave 7 as a result, so using 7 fibers
#define FIBER_CONTEXT_BUFFER_SIZE (2 * 1024) // 2 KB enough?

#endif