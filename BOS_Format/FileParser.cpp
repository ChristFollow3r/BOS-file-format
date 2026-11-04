#include "FileParser.h"
#include <stdexcept>


FileParser::FileParser(const std::string& filePath) : file(filePath)
{
    if (!file.is_open()) std::runtime_error("Couldn't open the file: " + filePath);
}

std::string FileParser::readField(char delim)
{
    std::string temp;
    std::getline(file >> std::ws, temp, delim);
    if (!file) throw std::runtime_error("Unexpected end of file");
    return temp;
}

int FileParser::readInt()
{
    std::string temp = readField();
    try{ return std::stoi(temp); }
    catch (...) { throw std::runtime_error("Invalid number: " + temp); }
}

void FileParser::expect(const std::string& token)
{
    std::string temp = readField();
    if (temp != token) throw std::runtime_error("Unexpected token: " + token + "." + " Got: " + temp);
}

char** FileParser::readGrid(int width, int height)
{
    char** grid = new char*[height];
    
    for (int i = 0; i < height; i++) {
        grid[i] = new char[width];
        for (int j = 0; j < width; j++) {
            std::string cellValue = readField();
            grid[i][j] = cellValue[0];
        }
        skipLine();
    }
    
    return grid;
}


LevelData FileParser::parse()
{
    LevelData data;
    
    expect("BOS");
    readField(); // Version right here
    skipLine();
    
    expect("TITLE");
    data.title = readField();
    skipLine();
    
    expect("TEXTURES");
    int textureCount = readInt();
    skipLine();
    for (int i = 0; i < textureCount; i++)
    {
        std::string key = readField();
        std::string value = readField();
        data.textures[key[0]] = value;
        skipLine();
    }
    
    expect("SIZE");
    data.width = readInt();
    data.height = readInt();
    skipLine();
    
    expect("FLOOR");
    skipLine();
    data.floor = readGrid(data.width, data.height);
    
    expect("MAP");
    skipLine();
    data.map = readGrid(data.width, data.height);
    
    expect("COLLISIONS");
    skipLine();
    data.collisions = readGrid(data.width, data.height);
    
    expect("OBJECTS");
    skipLine();
    data.objects = readGrid(data.width, data.height);
    
    return data;
}

void FileParser::skipLine()
{
    std::string temp;
    std::getline(file, temp);
}
