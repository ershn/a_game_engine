#pragma once

#include "Definitions.hpp"
#include "Scene.hpp"

namespace Age::Core
{
void exit();

void run_engine(const App::Definitions &definitions, const App::IScene &scene);
} // namespace Age::Core
