// Bomberman.cpp
#include "Bomberman.h"
#include "rlgl.h"
#include <iostream>
#include <cmath>

Bomberman::Bomberman(const LevelData& levelData) 
    : m_levelData(levelData)
{
}

Bomberman::~Bomberman()
{
    // Clean up Raylib resources on destruction
    for (auto& t : m_loadedTextures) {
        UnloadTexture(t.second);
    }
    CloseWindow();
}

void Bomberman::run()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Fallback title if LevelData title is empty
    std::string windowTitle = m_levelData.title.empty() ? "Bomberman" : m_levelData.title;
    InitWindow(screenWidth, screenHeight, windowTitle.c_str());

    // Load textures AFTER the window/OpenGL context exists
    for (const auto& png : m_levelData.textures)
    {
        std::string pngName = "resources/" + png.second;
        Image tempImage = LoadImage(pngName.c_str());
        Texture2D tempTexture = LoadTextureFromImage(tempImage);
        UnloadImage(tempImage);

        if (tempTexture.id == 0)
            std::cerr << "Failed to load texture: " << pngName << "\n";

        m_loadedTextures[png.first] = tempTexture;
    }
    
    float centerX = (m_levelData.height - 1) / 2.0f;
    float centerZ = (m_levelData.width  - 1) / 2.0f;
    Vector3 mapCenter = { centerX, 0.5f, centerZ };

    Camera camera = { 0 };
    camera.target     = mapCenter;
    camera.position   = { mapCenter.x, 20.0f, mapCenter.z + 0.1f }; 
    camera.up         = { 0.0f, 1.0f, 0.0f };
    camera.fovy       = 13.0f;                  
    camera.projection = CAMERA_ORTHOGRAPHIC;

    bool pause = false;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_P)) pause = !pause;

        if (!pause) UpdateCamera(&camera, CAMERA_CUSTOM);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        
        float dt = GetFrameTime();
        
        auto nextPosition = playerPosition;
        
        if (IsKeyDown(KEY_W)) nextPosition.z -= playerSpeed * dt;
        if (IsKeyDown(KEY_S)) nextPosition.z += playerSpeed * dt;
        if (IsKeyDown(KEY_D)) nextPosition.x += playerSpeed * dt;
        if (IsKeyDown(KEY_A)) nextPosition.x -= playerSpeed * dt;
        if (IsKeyPressed(KEY_Q) && bombs.size() <= maxBombs)
        {
            Bomb bomb;
            bomb.bombPosition = Vector3({playerPosition.x, playerPosition.y, playerPosition.z});
            bombs.push_back(bomb);
        }
        
        if (!CollidesAt(nextPosition.x, playerPosition.z)) playerPosition.x = nextPosition.x;
        if (!CollidesAt(playerPosition.x, nextPosition.z)) playerPosition.z = nextPosition.z;
        
        if (m_levelData.floor != nullptr) 
        {
            for (int i = 0; i < m_levelData.width; i++)
            {
                for (int j = 0; j < m_levelData.height; j++)
                {
                    char texKey = m_levelData.floor[i][j];
                    if (m_loadedTextures.count(texKey)) {
                        DrawCubeTexture(m_loadedTextures[texKey], { (float)j, 0, (float)i }, 1.0f, 1.0f, 1.0f, WHITE);
                    }
                }
            }
        }
        
        if (m_levelData.map != nullptr)
        {
            for (int i = 0; i < m_levelData.width; i++)
            {
                for (int j = 0; j < m_levelData.height; j++)
                {
                    char texKey = m_levelData.map[i][j];
                    if (m_loadedTextures.count(texKey))
                    {
                        DrawCubeTexture(m_loadedTextures[texKey], { (float)j, 1, (float)i }, 1.0f, 1.0f, 1.0f, WHITE);
                    }
                }
            }
        }
        
        DrawSphere({playerPosition.x, (float)1, playerPosition.z}, playerRadius, RED);
        
        for (auto& bomb : bombs)
        {
            if (!bomb.hasExploded)
            {
                if (bomb.EndOfCountdown(dt)) bomb.hasExploded = true;
                DrawSphere(bomb.bombPosition, 0.35f, BLACK);
            }
            
            else
            {
                bomb.Explode(dt);
                DrawSphere(bomb.bombPosition, 0.6f, ORANGE);
                
                int pX = (int)std::round(bomb.bombPosition.x);
                int pZ = (int)std::round(bomb.bombPosition.z);
                
                for (int i = -1; i <= 2; i++)
                {
                    if (m_levelData.collisions[pX + i][pZ] != '1') ExplodeAt(pX + i, pZ);
                    if (m_levelData.collisions[pX][pZ + i] != '1')ExplodeAt(pX, pZ + i);
                }
                
            }
        }
        
        for (int i = bombs.size() - 1; i >= 0; i--)
        {
            if (bombs[i].hasExploded && bombs[i].explosionTimer >= bombs[i].explosionDuration) bombs.erase(bombs.begin() + i);
        }
        
        EndMode3D();
        DrawText(m_levelData.title.c_str(), camera.position.x, camera.position.y, 20, BLACK);
        DrawFPS(10, 10);
        EndDrawing();
    }
}

void Bomberman::DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);
    
    // Front Face
    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    // Back Face
    rlNormal3f(0.0f, 0.0f, -1.0f);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    // Top Face
    rlNormal3f(0.0f, 1.0f, 0.0f);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    // Bottom Face
    rlNormal3f(0.0f, -1.0f, 0.0f);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    // Right face
    rlNormal3f(1.0f, 0.0f, 0.0f);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    // Left Face
    rlNormal3f(-1.0f, 0.0f, 0.0f);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    
    rlEnd();
    rlSetTexture(0);
}

void Bomberman::DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;
    float texWidth = (float)texture.width;
    float texHeight = (float)texture.height;

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);

    // Front face
    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

    // Back face
    rlNormal3f(0.0f, 0.0f, -1.0f);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

    // Top face
    rlNormal3f(0.0f, 1.0f, 0.0f);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

    // Bottom face
    rlNormal3f(0.0f, -1.0f, 0.0f);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

    // Right face
    rlNormal3f(1.0f, 0.0f, 0.0f);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

    // Left face
    rlNormal3f(-1.0f, 0.0f, 0.0f);
    rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(source.x / texWidth, source.y / texHeight);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

    rlEnd();
    rlSetTexture(0);
}

bool Bomberman::IsBlocked(float x, float z)
{
    int col = (int)std::round(x);
    int row = (int)std::round(z);
    
    if (col < 0 || row < 0 || row >= m_levelData.width || col >= m_levelData.height) return true;
    return m_levelData.collisions[row][col] == '1' || m_levelData.collisions[row][col] == 'D';
}

bool Bomberman::CollidesAt(float x, float z)
{
    float r = playerRadius;
    return IsBlocked(x - r, z - r) || IsBlocked(x + r, z - r) ||
           IsBlocked(x - r, z + r) || IsBlocked(x + r, z + r);
}

void Bomberman::ExplodeAt(int x, int z)
{
    if (x < 0 || z < 0 || z >= m_levelData.width || x >= m_levelData.height) return;
    if (m_levelData.collisions[z][x] == 'D')
    {
        m_levelData.collisions[z][x] = '0';
        m_levelData.map[z][x] = '0';
    }
    DrawCube({ (float)x, 1.0f, (float)z }, 1.0f, 1.0f, 1.0f, ORANGE);
}
