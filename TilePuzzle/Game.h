#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <vector>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "SDLExceptions.h"
#include "Counter.h"
#include "GameBoard.h"
#include "Vector2.h"
#include "Player.h"
#include "Renderer.h"
#include "GameBoard.h"
#include "GameState.h"

class Game final : public Observer
{
public:
    Game(SDL_Window* window, const std::string& levelPath);
    ~Game() override;
    void run();
    void handleLeftMouseButtonClick(const SDL_MouseButtonEvent& event);
    void handleRightMouseButtonClick(const SDL_MouseButtonEvent& event);
    bool handleInputEvents();
    void update(double deltaTime);
    void addBackgroundEntity(const std::shared_ptr<Entity>& entity);
    void addForegroundEntity(const std::shared_ptr<Entity>& entity);
    void loadLevel(const std::string& path);
    //bool canMoveTo(const Entity& entity, Vector2<double> potentialPosition) const override;

private:
    GameState m_gameState;
    std::unique_ptr<GameBoard> m_gameBoard;
    std::vector<std::shared_ptr<Entity>> m_backgroundEntities;   // Collection of background entities in the game
    std::vector<std::shared_ptr<Entity>> m_foregroundEntities;   // Collection of foreground entities in the game
    std::shared_ptr<Sprite> m_mouse;
    std::shared_ptr<Player> m_player;                            // Player sprite
    SDL_Window* m_window{};                                      // SDL window instance
    SDL_Event m_windowEvent{};                                   // SDL event for window handling
    std::unique_ptr<Renderer> m_renderer;
    std::atomic<bool> m_isAlive{ true };
};
