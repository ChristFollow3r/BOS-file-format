#pragma once
#include "raylib.h"
#include "FileParser.h"
#include "Bomb.h"
#include <map>
#include <vector>

class Bomberman
{
public:
    explicit Bomberman(const LevelData& levelData);
    ~Bomberman();
    
    void run();

private:
    LevelData m_levelData;
    std::map<char, Texture2D> m_loadedTextures;
    Vector3 playerPosition = { 1.0f, 1.0f, 1.0f };
    float playerSpeed = 4.0f;
    float playerRadius = 0.4f;
    
    std::vector<Bomb> bombs;
    int maxBombs = 1;
    
    void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);
    void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color);
    void ExplodeAt(int x, int z);
    bool IsBlocked(float x, float z);
    bool CollidesAt(float x, float z);
};