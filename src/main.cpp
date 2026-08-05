#include "main_loop.hpp"

#include "game/checkerboard_scene.hpp"
#include "game/cube_point_light_scene.hpp"
#include "game/double_projection_scene.hpp"
#include "game/gamma_and_textures_scene.hpp"
#include "game/infinity_symbol_scene.hpp"
#include "game/procedural_texture_scene.hpp"
#include "game/projected_light_scene.hpp"
#include "game/render_to_texture_scene.hpp"
#include "game/valley_scene.hpp"

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
