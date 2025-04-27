#include "MainLoop.hpp"

#include "game/CheckerboardScene.hpp"
#include "game/Game.hpp"
#include "game/InfinitySymbolScene.hpp"
#include "game/ValleyScene.hpp"

int main()
{
    auto scene = Game::CheckerBoardScene{};

#ifdef _DEBUG
    Age::Core::run_engine(Game::g_definitions, scene);
#else
    try
    {
        Age::Core::run_engine(Game::g_definitions, scene);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
#endif
}
