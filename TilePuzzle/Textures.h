#pragma once

class Textures
{
public:
	Textures() = delete;
	Textures operator=(const Textures&) = delete;

	// Game
	static constexpr const char* PLAYER_SPRITE_PATH = "./sprites/sword.bmp";
	static constexpr const char* CURSOR_SPRITE_PATH = "./sprites/Highlight.bmp";
	static constexpr const char* RECTANGLE_SPRITE_PATH = "./sprites/curvy.bmp";

	// Tiles
	static constexpr const char* GRASS_SPRITE_PATH = "./sprites/grass.bmp";
};