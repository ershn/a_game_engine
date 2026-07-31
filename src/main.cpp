#include "MainLoop.hpp"

#include "game/CheckerboardScene.hpp"
#include "game/CubePointLightScene.hpp"
#include "game/DoubleProjectionScene.hpp"
#include "game/GammaAndTexturesScene.hpp"
#include "game/InfinitySymbolScene.hpp"
#include "game/ProceduralTextureScene.hpp"
#include "game/ProjectedLightScene.hpp"
#include "game/RenderToTextureScene.hpp"
#include "game/ValleyScene.hpp"

int main()
{
    using Scene = Game::RenderToTextureScene;

#ifdef _DEBUG
    Age::Core::run_engine<Scene>();
#else
    try
    {
        Age::Core::run_engine<Scene>();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
#endif
}
