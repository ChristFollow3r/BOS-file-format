#pragma once
#include "raylib.h"
#include "FileParser.h"
#include <map>

class Bomberman
{
public:
    explicit Bomberman(const LevelData& levelData);
    ~Bomberman();
    
    void run();

private:
    LevelData m_levelData;
    std::map<char, Texture2D> m_loadedTextures;
    Vector3 playerPosition = { 5.0f, 1.0f, 5.0f };
    float playerSpeed = 4.0f;

    void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);
    void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color);
};