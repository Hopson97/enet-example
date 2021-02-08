#pragma once

#include <vector>

struct Entity {
    float x;
    float y;
};

struct World {
    std::vector<Entity> entities;
};
