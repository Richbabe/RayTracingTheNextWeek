#pragma once

#include <algorithm>
#include <thread>
#include <functional>
#include <vector>

/// @param[in] TaskSize : size of your for loop
/// @param[in] Functor(Start, End) :
/// your function processing a sub chunk of the for loop.
/// "Start" is the first index to process (included) until the index "End"
/// (excluded)
/// @code
///     for(int i = Start; i < End; ++i)
///         computation(i);
/// @endcode
/// @param bEnableAsync : enable / disable async.
///
///
static void ParallelFor(uint32_t TaskSize,
                  std::function<void (int Start, int End)> Functor,
                  bool bEnableAsync = true)
{
    // -------
    uint32_t ThreadsNum = std::thread::hardware_concurrency();
    ThreadsNum = ThreadsNum == 0 ? 8 : (ThreadsNum);

    uint32_t BatchSize = TaskSize / ThreadsNum;
    uint32_t BatchRemainder = TaskSize % ThreadsNum;

    std::vector<std::thread> Threads(ThreadsNum);

    if( bEnableAsync )
    {
        // Multithread execution
        for(uint32_t i = 0; i < ThreadsNum; ++i)
        {
            int Start = i * BatchSize;
            Threads[i] = std::thread(Functor, Start, Start + BatchSize);
        }
    }
    else
    {
        // Single thread execution (for easy debugging)
        for(uint32_t i = 0; i < ThreadsNum; ++i)
        {
            int Start = i * BatchSize;
            Functor(Start, Start + BatchSize);
        }
    }

    // Deform the elements left
    int Start = ThreadsNum * BatchSize;
    Functor(Start, Start + BatchRemainder);

    // Wait for the other thread to finish their task
    if( bEnableAsync )
    {
        std::for_each(Threads.begin(), Threads.end(), std::mem_fn(&std::thread::join));
    }
}