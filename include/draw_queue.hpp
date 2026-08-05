#pragma once

namespace Age::Gfx
{
struct DrawQueue
{
    enum BitWidth
    {
        QUEUE = 11,
        TOTAL = 12,
    };

    unsigned short transparent : 1 {false};
    unsigned short queue : BitWidth::QUEUE{1000};

    static constexpr DrawQueue from(unsigned int draw_queue)
    {
        return {
            .transparent = static_cast<unsigned short>(draw_queue >> BitWidth::QUEUE & 0b1),
            .queue = static_cast<unsigned short>(draw_queue & (1 << BitWidth::QUEUE) - 1)
        };
    }

    constexpr operator unsigned int() const
    {
        return transparent << BitWidth::QUEUE | queue;
    }

    static const DrawQueue min_opaque;
    static const DrawQueue max_opaque;
    static const DrawQueue min_transparent;
    static const DrawQueue max_transparent;
    static const DrawQueue min;
    static const DrawQueue max;
};

inline constexpr DrawQueue DrawQueue::min_opaque{.transparent = false, .queue = 0};
inline constexpr DrawQueue DrawQueue::max_opaque{.transparent = false, .queue = (1 << BitWidth::QUEUE) - 1};
inline constexpr DrawQueue DrawQueue::min_transparent{.transparent = true, .queue = 0};
inline constexpr DrawQueue DrawQueue::max_transparent{.transparent = true, .queue = (1 << BitWidth::QUEUE) - 1};
inline constexpr DrawQueue DrawQueue::min{min_opaque};
inline constexpr DrawQueue DrawQueue::max{max_transparent};
} // namespace Age::Gfx
