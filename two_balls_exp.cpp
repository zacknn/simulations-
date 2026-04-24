#include <SFML/Graphics.hpp>
#include <iostream>

struct Ball {
    sf::CircleShape shape;
    float x, vx;
    float mass = 1.0f;
    bool hasFriction;

    Ball(float startX, sf::Color color, bool friction) {
        shape.setRadius(20.f);
        shape.setFillColor(color);
        shape.setOrigin({20.f, 20.f});
        x = startX;
        vx = 300.0f; // Initial "kick" to the right
        hasFriction = friction;
    }

    void update(float dt) {
        float ax = 0.0f;

        if (hasFriction) {
            // 1. Define constants
            float gravity = 980.0f;
            float mu = 0.2f; // Coefficient of friction
            
            // 2. Calculate Friction Force (F = mu * m * g)
            float frictionForce = mu * mass * gravity;

            // 3. Apply Force in opposite direction of velocity
            if (std::abs(vx) > 1.0f) { // Only apply if moving
                ax = (vx > 0) ? -frictionForce : frictionForce;
            } else {
                vx = 0; // Stop completely if very slow
            }
        }

        // 4. Update Physics
        vx += ax * dt;
        x += vx * dt;
        shape.setPosition({x, 300.0f}); // Moving along a middle "line"
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Friction Proof", sf::Style::Titlebar | sf::Style::Close);
    
    // Red ball has NO friction, Blue ball HAS friction
    Ball ball1(50.0f, sf::Color::Red, false);
    Ball ball2(50.0f, sf::Color::Blue, true);

    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        float dt = clock.restart().asSeconds();

        ball1.update(dt);
        ball2.update(dt);

        window.clear();
        window.draw(ball1.shape);
        window.draw(ball2.shape);
        window.display();
    }
    return 0;
}
