#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>

const float GRAVITY = 980.0f;
const float CURVE_CENTER = 400.0f;
const float CURVE_BASE = 550.0f;
const float CURVE_FACTOR = 0.003f;

struct PhysicsBall {
    sf::CircleShape shape;
    sf::Vector2f pos;
    sf::Vector2f vel;
    bool isAlive = false;

    PhysicsBall() {
        shape.setRadius(15.0f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin({15.0f, 15.0f});
    }

    void launch(sf::Vector2f startPos, float speed) {
        pos = startPos;
        isAlive = true;
        vel = {-speed, 0.0f};  // Move left
    }

    void update(float dt) {
        if (!isAlive)
            return;

        // 1. Apply Gravity
        vel.y += GRAVITY * dt;

        // 2. Update Position
        pos += vel * dt;

        // 3. Collision with the Curve
        float groundY = CURVE_BASE - std::powf(pos.x - CURVE_CENTER, 2) * CURVE_FACTOR;

        if (pos.y > groundY) {
            pos.y = groundY;

            // Slope of the curve (derivative)
            float slope = (pos.x - CURVE_CENTER) * -2.0f * CURVE_FACTOR;

            // Project velocity onto the tangent (slide along curve)
            float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
            float angle = std::atan(slope);

            vel.x = speed * std::cos(angle);
            vel.y = speed * std::sin(angle);

            // Friction
            vel *= 0.995f;
        }

        // Stop if very slow
        float currentSpeed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        if (currentSpeed < 10.0f && pos.y >= groundY - 5.0f) {
            isAlive = false;
        }

        shape.setPosition(pos);
    }

    float getGroundY() {
        return CURVE_BASE - std::powf(pos.x - CURVE_CENTER, 2) * CURVE_FACTOR;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Energy Conservation",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    PhysicsBall ball;

    float launchSpeed = 300.0f;
    const float MIN_SPEED = 100.0f;
    const float MAX_SPEED = 800.0f;

    // Start position (high on the right)
    sf::Vector2f startPos = {700.0f, 50.0f};

    // Font for UI
    sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
        std::cerr << "Error: System font not found! UI will not render."
                  << std::endl;
    }

    sf::Text uiText(font);
    uiText.setCharacterSize(20);
    uiText.setFillColor(sf::Color::White);
    uiText.setPosition({10, 10});

    sf::Clock clock;

    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                if (keyReleased->code == sf::Keyboard::Key::Space) {
                    ball.launch(startPos, launchSpeed);
                }
            }
        }

        // Keyboard controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            launchSpeed += 10.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            launchSpeed -= 10.0f;

        // Clamp speed
        if (launchSpeed < MIN_SPEED) launchSpeed = MIN_SPEED;
        if (launchSpeed > MAX_SPEED) launchSpeed = MAX_SPEED;

        float dt = clock.restart().asSeconds();
        ball.update(dt);

        // Calculate energy
        float height = 600.0f - ball.pos.y;
        float PE = 0.5f * height;  // mass = 1, gravity = 980
        float speed = std::sqrt(ball.vel.x * ball.vel.x + ball.vel.y * ball.vel.y);
        float KE = 0.5f * speed * speed;

        // UI Text
        std::string info = "Speed: " + std::to_string((int)launchSpeed) +
                          " | PE: " + std::to_string((int)PE) +
                          " | KE: " + std::to_string((int)KE) +
                          "\n[Space to Launch] [Left/Right to Adjust Speed]";
        uiText.setString(info);

        window.clear(sf::Color(30, 30, 30));

        // Draw the Curve
        for (int x = 0; x < 800; x += 5) {
            sf::CircleShape dot(3);
            dot.setFillColor(sf::Color(100, 200, 100));
            dot.setOrigin({1.5f, 1.5f});
            float y = CURVE_BASE - std::powf(x - CURVE_CENTER, 2) * CURVE_FACTOR;
            dot.setPosition({(float)x, y});
            window.draw(dot);
        }

        // Draw Ball
        if (ball.isAlive)
            window.draw(ball.shape);

        // Draw UI
        if (font.getLineSpacing(20) != 0) {
            window.draw(uiText);
        }

        window.display();
    }
    return 0;
}