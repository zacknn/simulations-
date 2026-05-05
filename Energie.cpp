#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const float PI       = 3.14159265f;
const float GRAVITY  = 500.0f;   // pixels/s² (tuned for screen scale)
const float GROUND_Y = 580.0f;   // reference level for PE = 0

// --- Terrain as a polyline ---
std::vector<sf::Vector2f> TERRAIN = {
    {0,   400},
    {150, 400},   // flat start
    {300, 480},   // slope down
    {450, 560},   // valley bottom
    {600, 460},   // slope up
    {750, 340},   // top of hill  <-- high PE here
    {800, 340}
};

float getTerrainY(float x) {
    for (int i = 0; i < (int)TERRAIN.size() - 1; i++) {
        float x0 = TERRAIN[i].x, x1 = TERRAIN[i+1].x;
        if (x >= x0 && x <= x1) {
            float t = (x - x0) / (x1 - x0);
            return TERRAIN[i].y + t * (TERRAIN[i+1].y - TERRAIN[i].y);
        }
    }
    return TERRAIN.back().y;
}

float getSlopeAngle(float x) {
    for (int i = 0; i < (int)TERRAIN.size() - 1; i++) {
        float x0 = TERRAIN[i].x, x1 = TERRAIN[i+1].x;
        if (x >= x0 && x <= x1) {
            float dx = x1 - x0;
            float dy = TERRAIN[i+1].y - TERRAIN[i].y;
            return std::atan2(dy, dx);   // radians
        }
    }
    return 0.f;
}

// ---------------------------------------------------------------
struct Ball {
    sf::CircleShape shape;
    sf::Vector2f    pos;
    float           speed;   // scalar speed along the slope

    Ball() {
        shape.setRadius(12.f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin({12.f, 12.f});
        pos   = {10.f, 0.f};
        speed = 80.f;
    }

    void update(float dt) {
        float angle = getSlopeAngle(pos.x);

        // a = g * sin(θ)  –– positive when going downhill
        float accel = GRAVITY * std::sin(angle);
        speed += accel * dt;
        if (speed < 10.f) speed = 10.f;   // keep rolling forward

        // move along slope direction
        pos.x += speed * std::cos(angle) * dt;
        pos.y  = getTerrainY(pos.x) - 12.f;  // sit on terrain

        // loop back to start
        if (pos.x >= TERRAIN.back().x - 5.f)
            pos = {TERRAIN[0].x + 5.f, 0.f};

        shape.setPosition(pos);
    }
};

// ---------------------------------------------------------------
int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}),
                            "Potential & Kinetic Energy",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
        std::cerr << "Font not found!" << std::endl;
    }

    sf::Text uiText(font);
    uiText.setCharacterSize(16);
    uiText.setFillColor(sf::Color::White);
    uiText.setPosition({10.f, 10.f});

    Ball ball;
    sf::Clock clock;

    while (window.isOpen()) {
        // --- Events ---
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        float dt = clock.restart().asSeconds();
        ball.update(dt);

        // --- Energy calculations ---
        float h  = GROUND_Y - (ball.pos.y + 12.f);   // height above ground
        float ke = 0.5f * ball.speed * ball.speed;    // ½mv² (mass=1)
        float pe = GRAVITY * h;                        // mgh  (mass=1)
        float total = ke + pe;

        // --- Build terrain drawable ---
        // Fill below the terrain
        sf::VertexArray fill(sf::PrimitiveType::TriangleStrip);
        for (auto& p : TERRAIN) {
            fill.append({p,               sf::Color(40, 80, 130, 200)});
            fill.append({{p.x, 600.f},    sf::Color(20, 40,  80, 200)});
        }

        // Terrain outline
        sf::VertexArray outline(sf::PrimitiveType::LineStrip, TERRAIN.size());
        for (int i = 0; i < (int)TERRAIN.size(); i++)
            outline[i] = {TERRAIN[i], sf::Color::White};

        // --- Energy bars (bottom-left) ---
        float maxE   = 80000.f;
        float barW   = 28.f;
        float barBaseY = 595.f;
        float maxBarH  = 140.f;

        auto makeBar = [&](float value, float bx, sf::Color col)
            -> sf::RectangleShape
        {
            float bh = std::min(value / maxE * maxBarH, maxBarH);
            sf::RectangleShape bar({barW, bh});
            bar.setFillColor(col);
            bar.setPosition({bx, barBaseY - bh});
            return bar;
        };

        sf::RectangleShape keBar  = makeBar(ke,    20.f,  sf::Color(80, 160, 255));
        sf::RectangleShape peBar  = makeBar(pe,    60.f,  sf::Color(255, 90,  70));
        sf::RectangleShape totBar = makeBar(total, 100.f, sf::Color(80, 220, 120));

        // Bar labels
        sf::Text keLabel(font),  peLabel(font), totLabel(font);
        keLabel .setCharacterSize(13); keLabel .setFillColor(sf::Color::White);
        peLabel .setCharacterSize(13); peLabel .setFillColor(sf::Color::White);
        totLabel.setCharacterSize(13); totLabel.setFillColor(sf::Color::White);
        keLabel .setString("KE");  keLabel .setPosition({20.f,  barBaseY + 2});
        peLabel .setString("PE");  peLabel .setPosition({60.f,  barBaseY + 2});
        totLabel.setString("TOT"); totLabel.setPosition({98.f,  barBaseY + 2});

        // --- HUD text ---
        std::string info =
            "KE = 1/2 * v^2 = " + std::to_string((int)ke)   + "\n"
            "PE = g * h     = " + std::to_string((int)pe)   + "\n"
            "Total Energy   = " + std::to_string((int)total) + "\n"
            "Height (h)     = " + std::to_string((int)h)     + " px";
        uiText.setString(info);

        // --- Render ---
        window.clear(sf::Color(15, 15, 25));
        window.draw(fill);
        window.draw(outline);
        window.draw(ball.shape);
        window.draw(keBar);
        window.draw(peBar);
        window.draw(totBar);
        window.draw(keLabel);
        window.draw(peLabel);
        window.draw(totLabel);
        if (font.getLineSpacing(16) != 0)
            window.draw(uiText);
        window.display();
    }

    return 0;
}