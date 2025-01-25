#include "MainLoop.hpp"

int main()
{
#ifdef _DEBUG
    Age::Core::run_engine();
    return 0;
#else
    try
    {
        Age::Core::run_engine();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
#endif
}
