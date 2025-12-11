#include "game.h"
#include <iostream>


int main(int arg_c, char* argv[]){
    std::cout << "Hi!\n";

    Game game(100, 100);
    game.Run();

    return 0;
}
