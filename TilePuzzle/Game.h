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
    void update(double deltaTime);
    void addBackgroundEntity(const std::shared_ptr<Entity>& entity);
    void addForegroundEntity(const std::shared_ptr<Entity>& entity);
    void loadLevel(const std::string& path);

    /**
     * @return false - user quit
     */
    bool handleInputEvents();
    //bool canMoveTo(const Entity& entity, Vector2<double> potentialPosition) const override;

private:
    GameState m_gameState;
    std::unique_ptr<GameBoard> m_gameBoard;
    std::vector<std::shared_ptr<Entity>> m_backgroundEntities;
    std::vector<std::shared_ptr<Entity>> m_foregroundEntities;
    std::shared_ptr<Sprite> m_mouse;
    std::shared_ptr<Player> m_player;
    SDL_Window* m_window{};                                      // SDL window instance
    SDL_Event m_windowEvent{};                                   // SDL event for window handling
    std::unique_ptr<Renderer> m_renderer;
    std::atomic<bool> m_isAlive{ true };
};
