#include "Game.h"

int main(int argc, char** argv)
{
	Game& game = Game::get();
    game.run();
    return 0;
}
