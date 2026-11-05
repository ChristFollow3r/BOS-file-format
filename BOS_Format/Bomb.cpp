#include "Bomb.h"

bool Bomb::EndOfCountdown(float dt)
{
    bombTimer += dt;
    return bombTimer >= countdown;
}
    
bool Bomb::Explode(float dt)
{
    explosionTimer += dt;
    return explosionTimer >= explosionDuration;
}