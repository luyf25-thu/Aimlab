#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    std::cout << "Creating window..." << std::endl;
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Test");

    std::cout << "Loading font..." << std::endl;
    sf::Font font;
    if (!font.openFromFile("font.ttf"))
    {
        std::cerr << "Font load FAILED!" << std::endl;
        return 1;
    }
    std::cout << "Font OK" << std::endl;

    sf::Text text(font);
    text.setString("Hello SFML!");
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
    text.setPosition({ 250.0f, 280.0f });

    std::cout << "Entering main loop..." << std::endl;
    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
        }
        window.clear(sf::Color(30, 30, 50));
        window.draw(text);
        window.display();
    }
    std::cout << "Exited normally." << std::endl;
    return 0;
}
