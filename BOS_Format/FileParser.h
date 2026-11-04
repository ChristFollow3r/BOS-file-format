#pragma once
#include <map>
#include <string>
#include <fstream>

struct LevelData
{
        std::string title;
        std::map<char, std::string> textures;
        int width = 0, height = 0;
        
        char** floor = nullptr;
        char** map = nullptr;
        char** collisions = nullptr;
        char** objects = nullptr;
};

class FileParser
{
public:
        explicit FileParser(const std::string& filePath);
        LevelData parse();
private:
        std::ifstream file;
        std::string readField(char delim = ';');
        void expect(const std::string& token);
        void skipLine();
        int readInt();
        char** readGrid(int width, int height);
};
