#pragma once
#include <cstdint>

namespace gl3 {
    class PhysicsCategories{
        public:
        // Defines the type used for Box2D filter bits
        typedef uint64_t FilterCategory;

        // Base Categories for Physics Bodies (What an entity IS)
        static const FilterCategory CATEGORY_PLAYER    = 0x0000000000000001ULL;
        static const FilterCategory CATEGORY_WALL      = 0x0000000000000002ULL;
        static const FilterCategory CATEGORY_BOX   = 0x0000000000000004ULL;
        static const FilterCategory CATEGORY_DOOR   = 0x0000000000000400ULL;
        static const FilterCategory CATEGORY_SEC_BOT  = 0x000000000000010ULL;
        static const FilterCategory CATEGORY_PATROL_SENSORS = 0x000000000000020ULL;
        static const FilterCategory CATEGORY_FLOOR = 0x000000000000040ULL;
        static const FilterCategory CATEGORY_STICK = 0x000000000000080ULL;
        static const FilterCategory CATEGORY_SWITCH = 0x000000000000100ULL;
        static const FilterCategory CATEGORY_DOG = 0x000000000002000ULL;
        static const FilterCategory CATEGORY_LADDER = 0x000000000004000ULL;
        static const FilterCategory CATEGORY_SPEAKER = 0x000000000008000ULL;
        static const FilterCategory CATEGORY_SPEAKER_SWITCH = 0x0000000000010000ULL;
        static const FilterCategory CATEGORY_VENT = 0x0000000000200000ULL;
        static const FilterCategory CATEGORY_RAY_SENSOR = 0x0000000000400000ULL;
        static const FilterCategory CATEGORY_LASER_EYES = 0x0000000008000000ULL;
        // ... all other CATEGORY constants ...

        // A convenient mask that includes ALL physical bodies (for general collision checks)
        static const FilterCategory MASK_ALL_PHYSICAL =
            CATEGORY_PLAYER | CATEGORY_WALL | CATEGORY_BOX | CATEGORY_SEC_BOT | CATEGORY_DOOR | CATEGORY_FLOOR;


    };
}