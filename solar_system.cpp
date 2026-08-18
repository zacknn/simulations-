#include <SFML/Graphics.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
struct Planet {
  std::string name;
  float mass;
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::CircleShape circle;
  std::deque<sf::Vector2f> trail;
  size_t maxtail = 300;

  Planet(std::string n, float m, float radius, sf::Vector2f pos,
         sf::Vector2f vel, sf::Color color)
      : name(n), mass(m), position(pos), velocity(vel) {
    circle.setRadius(radius);
    circle.setFillColor(color);
    circle.setOrigin({radius, radius});
    circle.setPosition(position);
  }

  void update(float dt, const Planet &other) {
    sf::Vector2f dir = other.position - position;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (dist < 1.f)
      return;
    sf::Vector2f dirNorm = dir / dist;
    float G = 0.1f;
    float accel = G * other.mass / (dist * dist);
    velocity += dirNorm * accel * dt;
    position += velocity * dt;
    circle.setPosition(position);
    trail.push_back(position);
    if (trail.size() > maxtail)
      trail.pop_front();
  }
};

int main() {
  sf::RenderWindow window(sf::VideoMode({1400, 900}), "Solar System",
                          sf::Style::Titlebar | sf::Style::Close);
  sf::Vector2f center = sf::Vector2f(window.getSize()) / 2.f;
  window.setFramerateLimit(60);
  sf::View camera = window.getDefaultView();

  bool panning = false;
  sf::Vector2i lastMousePos;

  sf::Font font;
  if (!font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
    std::cerr << "Font not found!" << std::endl;
  }

  sf::Text uiText(font);
  uiText.setCharacterSize(20);
  uiText.setFillColor(sf::Color::White);
  uiText.setPosition({10, 10});

  Planet sun("Sun", 1.0e6f, 40.0f, center, sf::Vector2f(0, 0),
             sf::Color::Yellow);

  Planet mercury("Mercury", 0.055f, 4.f,
                 sf::Vector2f(center.x + 80.f, center.y),
                 sf::Vector2f(0.f, 35.4f), sf::Color(180, 180, 180));

  Planet venus("Venus", 0.815f, 7.f, sf::Vector2f(center.x + 130.f, center.y),
               sf::Vector2f(0.f, 27.7f), sf::Color(210, 180, 100));

  Planet earth("Earth", 1.0f, 8.f, sf::Vector2f(center.x + 180.f, center.y),
               sf::Vector2f(0.f, 23.6f), sf::Color(70, 130, 180));

  Planet mars("Mars", 0.107f, 5.f, sf::Vector2f(center.x + 240.f, center.y),
              sf::Vector2f(0.f, 20.4f), sf::Color(188, 74, 60));

  Planet jupiter("Jupiter", 317.8f, 22.f,
                 sf::Vector2f(center.x + 340.f, center.y),
                 sf::Vector2f(0.f, 17.1f), sf::Color(210, 150, 100));

  Planet saturn("Saturn", 95.2f, 18.f, sf::Vector2f(center.x + 430.f, center.y),
                sf::Vector2f(0.f, 15.3f), sf::Color(210, 190, 130));

  Planet uranus("Uranus", 14.5f, 13.f, sf::Vector2f(center.x + 510.f, center.y),
                sf::Vector2f(0.f, 14.0f), sf::Color(130, 210, 210));

  Planet neptune("Neptune", 17.1f, 12.f,
                 sf::Vector2f(center.x + 580.f, center.y),
                 sf::Vector2f(0.f, 13.1f), sf::Color(63, 84, 186));

  float moonOrbitRadius = 25.0f;
  float earthOrbitRadius = 180.0f;
  float earthOrbitSpeed = 23.6f;
  float moonOrbitSpeed = std::sqrt(0.1f * 1.0f / moonOrbitRadius); // G * M_earth / r
  
  // Earth is at (center.x + earthOrbitRadius, center.y) moving UP
  // Moon starts to the RIGHT of Earth, so its relative velocity should be UP
  sf::Vector2f moonInitialPos(center.x + earthOrbitRadius + moonOrbitRadius, center.y);
  sf::Vector2f moonInitialVel(0.f, earthOrbitSpeed + moonOrbitSpeed);

  // moon orbiting earth
  Planet moon("Moon", 0.012f, 3.f, moonInitialPos, moonInitialVel, sf::Color(200, 200, 200));

  
  // put all planets in a vector so we can loop over them
  std::vector<Planet *> planets = {&mercury, &venus,  &earth,  &mars,
                                   &jupiter, &saturn, &uranus, &neptune};
  std::vector<Planet*> moons = { &moon };

  sf::Clock clock;

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
      if (const auto *scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
        if (scroll->delta > 0)
          camera.zoom(0.9f); // scroll up = zoom in
        else
          camera.zoom(1.1f); // scroll down = zoom out
      }
      if (const auto *mb = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Middle) {
          panning = true;
          lastMousePos = mb->position;
        }
      }

      if (const auto *mb = event->getIf<sf::Event::MouseButtonReleased>()) {
        if (mb->button == sf::Mouse::Button::Middle)
          panning = false;
      }

      if (const auto *mm = event->getIf<sf::Event::MouseMoved>()) {
        if (panning) {
          sf::Vector2i delta = lastMousePos - mm->position;
          camera.move(sf::Vector2f(delta));
          lastMousePos = mm->position;
        }
      }
    }

    float dt = clock.restart().asSeconds();

    // update all planets against the sun
    for (auto *p : planets)
      p->update(dt, sun);
    
    for (auto* m : moons) {
      m->update(dt, earth);
      m->update(dt, sun);
    }

    window.setView(camera);
    window.clear();
    for (auto *p : planets) {
      for (size_t i = 0; i < p->trail.size(); i++) {
        sf::CircleShape dot(1.f);
        dot.setPosition(p->trail[i]);
        dot.setOrigin({1.f, 1.f});
        int alpha = (int)(255.f * i / p->trail.size());
        dot.setFillColor(sf::Color(255, 255, 255, alpha));
        window.draw(dot);
      }
    }
    for (auto* m : moons) {
    for (size_t i = 0; i < m->trail.size(); i++) {
        sf::CircleShape dot(1.f);
        dot.setPosition(m->trail[i]);
        dot.setOrigin({1.f, 1.f});
        int alpha = (int)(255.f * i / m->trail.size());
        dot.setFillColor(sf::Color(255, 255, 255, alpha));
        window.draw(dot);
    }
    window.draw(m->circle);
}
    window.draw(sun.circle);
    for (auto *p : planets)
      window.draw(p->circle);
    window.draw(moon.circle);
    window.setView(window.getDefaultView());
    float yOffset = 10.f;
    for (auto *p : planets) {
      float speed = std::sqrt(p->velocity.x * p->velocity.x +
                              p->velocity.y * p->velocity.y);
      std::string info = p->name + ": " + std::to_string(speed);
      uiText.setString(info);
      uiText.setPosition({10.f, yOffset});
      window.draw(uiText);
      yOffset += 25.f; // move down for next line
    }
    window.display();
  }

  return 0;
}
