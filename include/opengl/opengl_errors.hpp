#pragma once

#ifdef DEVELOPMENT_BUILD
# define OGL_CLEAR_ERRORS() clear_errors()
# define OGL_LOG_ERROR() log_error()
#else
# define OGL_CLEAR_ERRORS()
# define OGL_LOG_ERROR()
#endif

namespace Age::Gfx::OGL
{
void clear_errors();
void log_error();
} // namespace Age::Gfx::OGL
