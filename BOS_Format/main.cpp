#include "raylib.h"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include "rlgl.h"
#include <fstream>

std::fstream file("template.BOS");
std::string header;
std::string version;
std::string title;

int amountOfTextures;
int levelWidth;
int levelHeight;
std::map<char, std::string> textures;

char** floorMatrix = new char* [levelHeight];
char** mapMatrix = new char* [levelHeight];
char** collisionMatrix = new char* [levelHeight];
char** objectMatrix = new char* [levelHeight];

std::string line;
std::map<char, Texture2D> loadedTextures;

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color); // Draw cube textured
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color); // Draw cube with a region of a texture

int raylib()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - cubicmap rendering");

    // Load textures AFTER the window/OpenGL context exists
    for (auto &png : textures)
    {
        std::string pngName = "resources/" + png.second;
        Image tempImage = LoadImage(pngName.c_str());
        Texture2D tempTexture = LoadTextureFromImage(tempImage);
        UnloadImage(tempImage);

        if (tempTexture.id == 0)
            std::cerr << "Failed to load texture: " << pngName << "\n";

        loadedTextures[png.first] = tempTexture;
    }

    Camera camera = { 0 };
    camera.position = { 16.0f, 14.0f, 16.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    bool pause = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_P)) pause = !pause;

        if (!pause) UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        
        
        DrawCubeTexture(loadedTextures['a'], { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, WHITE);
        //DrawGrid(10, 1.0f);

        for (int i = 0; i < levelWidth; i++)
        {
            for (int j = 0; j < levelHeight; j++)
            {
            	DrawCubeTexture(loadedTextures[floorMatrix[i][j]], { (float)j, 0, (float)i }, 1.0f, 1.0f, 1.0f, WHITE);
            }
        }
        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    for (auto &t : loadedTextures) UnloadTexture(t.second);

    CloseWindow();

    return 0;
}

int main() {

	if (!file.is_open()) {
		std::cerr << "Error 1: Couldn't open the file.";
		file.close();
		return 1;
	}

	std::getline(file, header, ';');

	if (header != "BOS") {
		std::cout << "Error 2: Incorrect format.";
		file.close();
		return 2;
	}

	std::getline(file, version, ';');

	std::getline(file, line, ';');
	std::getline(file >> std::ws, line, ';');

	if (line != "TITLE") {
		std::cerr << "Error 3: Missing Title.";
		file.close();
		return 3;
	}

	std::getline(file, title, ';');
	std::getline(file, line, ';');
	std::getline(file, line, ';');
	std::getline(file >> std::ws, line, ';');

	try {
		amountOfTextures = std::stoi(line);
	}
	catch (...) {
		std::cerr << "Error 4: Invalid texture";
		file.close();
		return 4;
	}
		
	std::getline(file, line, ';');

	for (int i = 0; i < amountOfTextures; i++) {

		std::string key;
		std::string name;

		std::getline(file >> std::ws, key, ';');
		std::getline(file >> std::ws, name, ';');
		std::getline(file, line);

		textures.insert({key[0], name});
	}

	std::getline(file, line, ';');

	if (line != "SIZE") {
		std::cerr << "Error 5: Missing size!";
		file.close();
		return 5;
	}

	std::getline(file, line, ';');
	try { levelWidth = std::stoi(line); }
	catch (...) { std::cerr << "Error: Couldn't convert string to int"; }

	std::getline(file, line, ';');
	try { levelHeight = std::stoi(line); }
	catch (...) { std::cerr << "Error: Couldn't convert string to int"; }

	std::getline(file, line);
	std::getline(file, line, ';');
	if (line != "FLOOR") {
		std::cerr << "Error 5: Missing size!";
		file.close();
		return 5;
	}

	std::cout << line;
	std::getline(file, line);

	
	// FLOOR
	for (int i = 0; i < levelHeight; i++) {
		floorMatrix[i] = new char[levelWidth];
	}

	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {

			std::getline(file, line, ';');
			floorMatrix[i][j] = line[0];

		}
		std::getline(file, line);
	}

	std::getline(file, line);
	// MAP
	for (int i = 0; i < levelHeight; i++) {
		mapMatrix[i] = new char[levelWidth];
	}

	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {

			std::getline(file, line, ';');
			mapMatrix[i][j] = line[0];

		}
		std::getline(file, line);
	}
	std::getline(file, line);
	
	// COLLISIONS
	for (int i = 0; i < levelHeight; i++) {
		collisionMatrix[i] = new char[levelWidth];
	}

	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {

			std::getline(file, line, ';');
			collisionMatrix[i][j] = line[0];

		}
		std::getline(file, line);
	}
	std::getline(file, line);
	
	//OBJECTS
	for (int i = 0; i < levelHeight; i++) {
		objectMatrix[i] = new char[levelWidth];
	}

	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {

			std::getline(file, line, ';');
			objectMatrix[i][j] = line[0];

		}
		std::getline(file, line);
	}
	
	std::cout << "\n";
	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {
			std::cout << floorMatrix[i][j];
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {
			std::cout << mapMatrix[i][j];
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {
			std::cout << collisionMatrix[i][j];
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	for (int i = 0; i < levelWidth; i++) {
		for (int j = 0; j < levelHeight; j++) {
			std::cout << objectMatrix[i][j];
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	
	file.close();
	raylib();
	return 0;

}


void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);
    // Front Face
    rlNormal3f(0.0f, 0.0f, 1.0f);       // Normal Pointing Towards Viewer
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
    // Back Face
    rlNormal3f(0.0f, 0.0f, -1.0f);     // Normal Pointing Away From Viewer
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
    // Top Face
    rlNormal3f(0.0f, 1.0f, 0.0f);       // Normal Pointing Up
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    // Bottom Face
    rlNormal3f(0.0f, -1.0f, 0.0f);     // Normal Pointing Down
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    // Right face
    rlNormal3f(1.0f, 0.0f, 0.0f);       // Normal Pointing Right
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
    // Left Face
    rlNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
    rlEnd();

    rlSetTexture(0);
}

    void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;
    float texWidth = (float)texture.width;
    float texHeight = (float)texture.height;

    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(texture.id);

    // We calculate the normalized texture coordinates for the desired texture-source-rectangle
    // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
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