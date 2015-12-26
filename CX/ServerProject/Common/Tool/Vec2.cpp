#include "Vec2.h"

const Vec2 Vec2::ZERO = Vec2(0.0f, 0.0f);
const Vec2 Vec2::ONE = Vec2(1.0f, 1.0f);
const Vec2 Vec2::UNIT_X = Vec2(1.0f, 0.0f);
const Vec2 Vec2::UNIT_Y = Vec2(0.0f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE = Vec2(0.5f, 0.5f);
const Vec2 Vec2::ANCHOR_BOTTOM_LEFT = Vec2(0.0f, 0.0f);
const Vec2 Vec2::ANCHOR_TOP_LEFT = Vec2(0.0f, 1.0f);
const Vec2 Vec2::ANCHOR_BOTTOM_RIGHT = Vec2(1.0f, 0.0f);
const Vec2 Vec2::ANCHOR_TOP_RIGHT = Vec2(1.0f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE_RIGHT = Vec2(1.0f, 0.5f);
const Vec2 Vec2::ANCHOR_MIDDLE_LEFT = Vec2(0.0f, 0.5f);
const Vec2 Vec2::ANCHOR_MIDDLE_TOP = Vec2(0.5f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE_BOTTOM = Vec2(0.5f, 0.0f);


int BaseFunction::gRandomSeed = 1;