#pragma once
#include <raylib.h>

class Bomb
{
public:
    
    Vector3 bombPosition = Vector3({0.0f, 0.0f, 0.0f});
    
    bool hasExploded = false;
    
    float countdown = 3.0f;
    float explosionDuration = 0.5f;
    
    float bombTimer = 0.0f;
    float explosionTimer = 0.0f;
    
    bool EndOfCountdown(float dt);
    bool Explode(float dt);
    
};
