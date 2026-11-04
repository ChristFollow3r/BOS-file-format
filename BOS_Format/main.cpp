#include <string>
#include "Bomberman.h"
#include "FileParser.h"

int main() {

    FileParser parser("template.BOS");
    LevelData levelData = parser.parse();
    
    Bomberman game(levelData);
    game.run();
    
    return 0;

}