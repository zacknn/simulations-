#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

struct Ball
{
    sf::CircleShape shape;
    sf::Vector2f pos, vel, acc;
    float mass;
    sf::Color color;

    Ball(float m, sf::Vector2f startPos, sf::Color c) : mass(m), pos(startPos), color(c) {
        shape.setRadius(mass * 5.0f); // Size depends on mass for visual proof
        shape.setFillColor(color);
        shape.setOrigin({shape.getRadius(), shape.getRadius()});
    }

    void applyForce(sf::Vector2f force) {
        // Law: a = F / m
        acc += force / mass;
    }

    void update(float dt) {
        // Air Resistance (Drag) Calculation: F = -k * v
        float k = 0.5f; 
        sf::Vector2f dragForce = vel * -k;
        applyForce(dragForce);

        // Standard Gravity: F = m * g (9.8m/s^2 * scale)
        applyForce({0, mass * 980.0f});

        vel += acc * dt;
        pos += vel * dt;
        
        // Ground collision
        if (pos.y > 580) {
            pos.y = 580;
            vel.y *= -0.6f;
        }

        acc = {0, 0}; // Reset acceleration for next frame
        shape.setPosition(pos);
    }
};
int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Newton Laws", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Create a Heavy Ball (Mass 10) and a Light Ball (Mass 1)
    std::vector<Ball> balls;
    balls.push_back(Ball(10.0f, {200, 100}, sf::Color::Red));   // Heavy
    balls.push_back(Ball(1.0f, {400, 100}, sf::Color::Yellow)); // Light
    balls.push_back(Ball(0.2f, {600, 100}, sf::Color::Blue));   // "Balloon"

    sf::Clock clock;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        // Inside the event loop, if space is pressed:
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
           for(auto& ball : balls) {
                ball.applyForce({5000.0f, 0}); // Apply the SAME force to all
            }
        }

        float dt = clock.restart().asSeconds();
        window.clear();

        for (auto& ball : balls) {
            ball.update(dt);
            window.draw(ball.shape);
        }

        window.display();
    }
    return 0;
}

