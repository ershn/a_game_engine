#pragma once

namespace Age::Time
{
float frame_time();
float delta_time();
float current_time();

void init_frame_time();
void update_frame_time();
} // namespace Age::Time
