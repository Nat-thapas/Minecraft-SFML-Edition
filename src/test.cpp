#include <SFML/Graphics.hpp>
 
#include <iostream>
 
int main()
{
    auto window = sf::RenderWindow{ { 1500U, 800U }, "The Book of Shaders" };
    window.setFramerateLimit(144);
 
    auto clock = sf::Clock{};
 
    auto grass = sf::Sprite();

    sf::Texture texture;
    texture.loadFromFile("resources/textures/blocksx16/053.png");
    sf::Sprite sprite(texture);
    sprite.setScale(sf::Vector2f(32.f, 32.f));

    sf::RectangleShape shape(sf::Vector2f(512.f, 512.f));
    shape.setPosition(sf::Vector2f(512.f, 0.f));
    shape.setFillColor(sf::Color(0, 0, 0, 127));
 
    auto shader = sf::Shader{};
    if (!shader.loadFromFile("../test.frag", sf::Shader::Fragment))
    {
        std::cerr << "Couldn't load fragment shader\n";
        return -1;
    }
 
    auto mouse_position = sf::Vector2f{};
 
    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                mouse_position = window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
            }
        }
 
        shader.setUniform("texture", sf::Shader::CurrentTexture);
        shader.setUniform("sky", sf::Vector3f(135.f / 255.f, 206.f / 255.f, 235.f / 255.f));
        shader.setUniform("time", 0.f);
 
        window.clear(sf::Color(135, 206, 235));
        sprite.setPosition(sf::Vector2f(0.f, 0.f));
        sprite.setColor(sf::Color(127, 127, 127));
        window.draw(sprite, &shader);
        sprite.setPosition(sf::Vector2f(512.f, 0.f));
        sprite.setColor(sf::Color(255, 255, 255));
        window.draw(sprite);
        window.draw(shape);
        window.display();
    }
} 