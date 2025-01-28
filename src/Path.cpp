#include "Path.hpp"
#include "Time.hpp"

namespace Age::Core
{
void move_along_path(Transform &transform, PathFollower &path_follower)
{
    Math::Vector3 offset_to_target{path_follower.target_position - transform.position};

    if (Math::length(offset_to_target) <= path_follower.target_min_distance)
    {
        if (path_follower.target_index == path_follower.path.size() - 1)
        {
            if (path_follower.repeat_path)
            {
                path_follower.target_index = 0;
                path_follower.target_position = path_follower.path[0];
            }
            else
                return;
        }
        else
        {
            ++path_follower.target_index;
            path_follower.target_position = path_follower.path[path_follower.target_index];
        }
    }

    transform.position += Math::normalize(offset_to_target) * (path_follower.move_speed * Time::delta_time());
}
} // namespace Age::Core
