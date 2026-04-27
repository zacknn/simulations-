#include <SFML/Graphics.hpp>
#include <SFML/System/Angle.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const float PI = 3.14159265f;
const float GRAVITY = 980.0f;

struct Ball {
  sf::CircleShape shape;
  sf::Vector2f pos, vel;
  bool isAlive;

  Ball() : isAlive(false) {
    shape.setRadius(10.0f);
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin({10.0f, 10.0f});
  }

  void launch(sf::Vector2f startPos, float angleDegrees, float speed) {
    pos = startPos;
    isAlive = true;

    float radians = angleDegrees * (PI / 180.0f);

    vel.x = speed * std::cos(radians);

    vel.y = -speed * std::sin(radians);
  }

  void update(float dt) {
    if (!isAlive)
      return;

    vel.y += GRAVITY * dt;

    pos += vel * dt;

    shape.setPosition(pos);

    if (pos.y > 600 || pos.x > 800) {
      isAlive = false;
    }
  }
};

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "The Cannon Sim",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  sf::RectangleShape barrel({100.0f, 20.0f});
  barrel.setFillColor(sf::Color::Cyan);

  sf::Vector2f pivotPos = {50.0f, 550.0f};
  barrel.setPosition(pivotPos);

  barrel.setOrigin({0.0f, 10.0f});

  float cannonAngle = 45.0f;
  float launchSpeed = 800.0f;

  Ball ball;

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
          ball.launch(pivotPos, cannonAngle, launchSpeed);
        }
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
      cannonAngle += 1.0f; // Aim higher
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
      cannonAngle -= 1.0f; // Aim lower

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
      launchSpeed += 10.0f; // Speed up
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
      launchSpeed -= 10.0f; // Slow down

    if (cannonAngle > 90.0f)
      cannonAngle = 90.0f; // Don't aim backward
    if (cannonAngle < 0.0f)
      cannonAngle = 0.0f; // Don't aim into the ground
    if (launchSpeed < 100.0f)
      launchSpeed = 100.0f;
    if (launchSpeed > 2000.0f)
      launchSpeed = 2000.0f;

    barrel.setRotation(sf::degrees(-cannonAngle));

    float dt = clock.restart().asSeconds();
    ball.update(dt);

    std::string info = "Angle: " + std::to_string((int)cannonAngle) +
                       " | Speed: " + std::to_string((int)launchSpeed) +
                       "\n[Space to Fire] [Arrows to Aim]";
    uiText.setString(info);

    window.clear();

    window.draw(barrel);
    if (ball.isAlive)
      window.draw(ball.shape);

    if (font.getLineSpacing(20) != 0) {
      window.draw(uiText);
    }
    window.display();
  }
  return 0;
}
