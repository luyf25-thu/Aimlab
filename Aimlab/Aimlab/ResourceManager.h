#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

// 资源管理类：统一加载与缓存贴图、音效、字体
class ResourceManager
{
public:
    // 获取单例实例
    static ResourceManager& getInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    // 加载贴图资源
    void loadTexture(const std::string& name, const std::string& filename)
    {
        sf::Texture texture;
        if (!texture.loadFromFile(filename))
        {
            throw std::runtime_error("贴图加载失败: " + filename);
        }
        textures[name] = texture;
    }

    // 获取贴图资源
    sf::Texture& getTexture(const std::string& name)
    {
        return textures.at(name);
    }

    // 加载音效资源
    void loadSoundBuffer(const std::string& name, const std::string& filename)
    {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename))
        {
            throw std::runtime_error("音效加载失败: " + filename);
        }
        soundBuffers[name] = buffer;
    }

    // 获取音效资源
    sf::SoundBuffer& getSoundBuffer(const std::string& name)
    {
        return soundBuffers.at(name);
    }

    // 加载字体资源
    void loadFont(const std::string& name, const std::string& filename)
    {
        sf::Font font;
        if (!font.loadFromFile(filename))
        {
            throw std::runtime_error("字体加载失败: " + filename);
        }
        fonts[name] = font;
    }

    // 获取字体资源
    sf::Font& getFont(const std::string& name)
    {
        return fonts.at(name);
    }

private:
    ResourceManager() = default;

    // 贴图缓存
    std::unordered_map<std::string, sf::Texture> textures;
    // 音效缓存
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    // 字体缓存
    std::unordered_map<std::string, sf::Font> fonts;
};
