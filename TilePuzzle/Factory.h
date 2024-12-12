#pragma once
#include <memory>
#include <SDL.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Factory
{
public:
    template <typename SpriteType, typename... Args>
    static std::shared_ptr<SpriteType> create(SDL_Renderer* renderer, const std::string& path, Args&&... args)
    {
        Factory& instance = getInstance();
        const std::string& texturePath = instance.getTexture(path);
        return std::make_shared<SpriteType>(texturePath.c_str(), renderer, std::forward<Args>(args)...);
    }

    Factory(const Factory&) = delete;
    Factory& operator=(const Factory&) = delete;

private:
    Factory() = default;
    ~Factory() = default;

    void registerTexture(const std::string& key, const std::string& path)
    {
        m_registry[key] = path;
    }

    const std::string& getTexture(const std::string& key) const
    {
        auto it = m_registry.find(key);
        if (it == m_registry.end()) {
            throw std::out_of_range("Texture not found: " + key);
        }
        return it->second;
    }

    static Factory& getInstance()
    {
        static Factory instance;
        instance.init();
        return instance;
    }

    void init()
    {
        if (!m_initialized) 
        {
            registerTexture("Grass", "./sprites/grass.bmp");
            registerTexture("Rock", "./sprites/rock.bmp");
            m_initialized = true;
        }
    }

    bool m_initialized = false;
    std::unordered_map<std::string, std::string> m_registry;
};