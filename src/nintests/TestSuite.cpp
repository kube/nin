#include <cstdio>
#include <thread>
#include <nin/nin.h>
#include "TestSuite.h"

TestSuite::TestSuite()
{

}

void TestSuite::add(const char* name, const char* path, std::size_t cycles, Pred pred)
{
    _tests.push_back({ name, path, cycles, pred, TestState::Pending });
}

int TestSuite::run()
{
    unsigned int                maxThreads;
    std::vector<std::thread>    threads;

    int passed = 0;
    bool done = false;
    unsigned currentTestIdx = 0;

    maxThreads = std::thread::hardware_concurrency();
    if (maxThreads == 0)
        maxThreads = 1;

    for (unsigned i = 0; i < maxThreads; ++i)
        threads.emplace_back(&TestSuite::run_thread, this);

    {
        std::unique_lock<std::mutex> lock{_mutex};
        for (;;)
        {
            for (;;)
            {
                if (currentTestIdx == _tests.size())
                {
                    done = true;
                    break;
                }
                else if (_tests[currentTestIdx].state == TestState::Ok)
                {
                    std::putchar('.');
                    std::fflush(stdout);
                    passed++;
                    currentTestIdx++;
                }
                else if (_tests[currentTestIdx].state == TestState::Fail)
                {
                    std::putchar('F');
                    std::fflush(stdout);
                    currentTestIdx++;
                }
                else if (_tests[currentTestIdx].state == TestState::Error)
                {
                    std::putchar('E');
                    std::fflush(stdout);
                    currentTestIdx++;
                }
                else
                    break;
            }
            if (done)
                break;
            _cv.wait(lock);
        }
    }

    for (auto& t : threads)
        t.join();

    std::putchar('\n');
    std::putchar('\n');
    std::printf("Passed: %d/%d\n", passed, (int)_tests.size());

    if (passed == (int)_tests.size())
        return 0;

    std::putchar('\n');
    for (auto& t : _tests)
    {
        switch (t.state)
        {
        case TestState::Fail:
            std::printf("FAIL: %s\n", t.name);
            break;
        case TestState::Error:
            std::printf("ERROR: %s\n", t.name);
            break;
        default:
            break;
        }
    }
    return 1;
}

void TestSuite::run_thread()
{
    Test* t;

    for (;;)
    {
        t = nullptr;
        {
            std::unique_lock<std::mutex> lock{_mutex};
            for (unsigned i = 0; i < _tests.size(); ++i)
            {
                if (_tests[i].state == TestState::Pending)
                {
                    t = _tests.data() + i;
                    t->state = TestState::Running;
                    break;
                }
            }
        }
        if (!t)
            return;
        run_test(*t);
    }
}

void TestSuite::run_test(Test& t)
{
    NinState* s;

    if (ninCreateState(&s, t.path) != NIN_OK)
    {
        t.state = TestState::Error;
        return;
    }

    ninRunCycles(s, t.cycles, nullptr);

    if (t.pred(s))
    {
        t.state = TestState::Ok;
    }
    else
    {
        t.state = TestState::Fail;
    }

    ninDestroyState(s);
    _cv.notify_one();
}
