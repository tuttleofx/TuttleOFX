#ifndef _WINDOWSMEMORYLEAKS_HPP_
#define _WINDOWSMEMORYLEAKS_HPP_

#ifdef __WINDOWS__

#include <tuttle/common/patterns/StaticSingleton.hpp>
#include <crtdbg.h>

/**
 * @brief Class to detect memory leaks using the _CrtSetDbgFlag function (windows only).
 *
 * It's provided by the windows API, it's easy to use and very safe.
 * You just need to call CrtSetDbgFlag anywhere in your code !
 * Here is a simple:
 *
 * #include <crtdbg.h>
 *
 * #ifdef _DEBUG
 * #define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
 * #undef THIS_FILE
 * static char THIS_FILE[] = __FILE__;
 * #endif
 *
 * void main()
 * {
 * _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
 *
 * char* memory_leak = new char[256];
 * }
 *
 * And that's all !
 *
 * If your program has memory leaks, you'll be informed at the end of the program execution.
 */
class MemoryLeaks : public StaticSingleton<MemoryLeaks>
{
public:
    friend class StaticSingleton<MemoryLeaks>;

private:
    _CrtMemState m_checkpoint;

protected:
    MemoryLeaks() { _CrtMemCheckpoint(&m_checkpoint); }

    ~MemoryLeaks()
    {
        _CrtMemState checkpoint;

        _CrtMemCheckpoint(&checkpoint);

        _CrtMemState diff;
        _CrtMemDifference(&diff, &m_checkpoint, &checkpoint);

        _CrtMemDumpStatistics(&diff);
        _CrtMemDumpAllObjectsSince(&diff);
    }
};

#endif

#endif
