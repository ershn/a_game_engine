#include "MainLoop.hpp"

#include "game/CheckerboardScene.hpp"
#include "game/DoubleProjectionScene.hpp"
#include "game/Game.hpp"
#include "game/GammaAndTexturesScene.hpp"
#include "game/InfinitySymbolScene.hpp"
#include "game/ProjectedLightScene.hpp"
#include "game/ValleyScene.hpp"

int main()
{
    auto scene = Game::ProjectedLightScene{};

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
