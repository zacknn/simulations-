# Physics Simulation Code - Rules & Patterns

## Overview
This codebase contains multiple physics simulations using SFML graphics library. Each simulation demonstrates core physics principles through interactive visualizations.

---

## 1. BASIC PHYSICS FOUNDATIONS

### 1.1 Coordinate System Rules
- **Screen space**: Origin (0,0) at top-left, Y-axis points downward
- **Position tracking**: Use `Vec2` struct or `sf::Vector2f` for position/velocity
- **Physics frame**: Delta time (dt) is calculated each frame
- **Update sequence**: 
  1. Calculate acceleration
  2. Update velocity: `velocity += acceleration * dt`
  3. Update position: `position += velocity * dt`

### 1.2 Gravity Implementation
```
Acceleration = 980.0f pixels/s² (tuned for screen scale)
Force applied: F = m * g (where g = 980.0f)
Applied every frame to velocity
```

**Files using this**: `main.cpp`, `Newton_law.cpp`, `Cannon_sim.cpp`

---

## 2. COLLISION DETECTION & RESPONSE

### 2.1 Ground Collision (Horizontal Surface)
**Pattern**:
```cpp
if (position.y >= 580) {
    position.y = 580;  // Clamp position to ground
    velocity.y *= -0.7f;  // Reduce velocity by 30% (coefficient of restitution)
}
```

**Rules**:
- Check if position exceeds ground level
- Clamp position to exact ground level (prevents sub-pixel movement)
- Apply damping factor (e.g., 0.7f = 70% velocity retained)
- Energy loss: Each bounce loses 30% of kinetic energy

**⚠️ FLOATING POINT PROBLEM** (documented in `note.md`):
- After many bounces, velocity becomes extremely small (~0.000001)
- Computer doesn't recognize this as zero
- Gravity continues pulling → micro-bouncing occurs indefinitely
- **Solution**: Add velocity threshold check
  ```cpp
  if (std::abs(velocity.y) > 30.0) {
      velocity.y *= -0.7f;
  } else {
      velocity.y = 0;  // Force stop if very small
  }
  ```

**Files using this**: `main.cpp`, `Cannon_sim.cpp`, `Newton_law.cpp`

### 2.2 Wall Collision (Vertical Surface)
**Pattern**:
```cpp
// Right wall
if (position.x > 780) {
    position.x = 780;
    velocity.x *= -0.999f;  // Nearly perfect reflection
}

// Left wall
if (position.x < 20) {
    position.x = 20;
    velocity.x *= -0.999f;
}

// Screen wrapping (wraps around edges)
if (position.x > 800) position.x = 0;
if (position.x < 0) position.x = 800;
```

**Rules**:
- Clamp to wall position before reversing velocity
- Use very high damping (0.999) for near-elastic collisions
- Alternative: Wrap around screen edges for infinite space effect

**Files using this**: `main.cpp`

### 2.3 Slope/Terrain Collision
**Pattern**:
```cpp
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
```

**Rules**:
- Use linear interpolation between terrain points
- Always keep ball sitting on terrain: `pos.y = getTerrainY(pos.x) - radius`
- No traditional bouncing; object slides along slope

**Files using this**: `Energie.cpp`

---

## 3. PROJECTILE MOTION

### 3.1 Polar to Cartesian Conversion
**Pattern** (used in `Cannon_sim.cpp`):
```cpp
float radians = angleDegrees * (PI / 180.0f);
vel.x = speed * std::cos(radians);
vel.y = -speed * std::sin(radians);  // Negative because Y is inverted
```

**Rules**:
- Convert degrees to radians: `radians = degrees * (π / 180)`
- Use cosine for X velocity (horizontal)
- Use negative sine for Y velocity (up is negative in screen space)
- Angle of 45° gives maximum range (for standard gravity)

### 3.2 Projectile Launch Conditions
- Launch point: Barrel pivot position
- Initial velocity: Calculated from angle + speed
- Termination: When position.y > screen_height OR position.x > screen_width

**Files using this**: `Cannon_sim.cpp`

---

## 4. FORCES & ACCELERATIONS

### 4.1 Newton's Second Law: F = m * a (Rearranged: a = F / m)
**Pattern**:
```cpp
void applyForce(sf::Vector2f force) {
    acc += force / mass;  // Divide by mass to get acceleration
}
```

**Key Rules**:
- Same force on different masses produces different accelerations
- Lighter objects accelerate more
- Heavy objects resist acceleration
- Multiple forces are additive on acceleration

**Files using this**: `Newton_law.cpp`

### 4.2 Gravity Force
```cpp
// Standard gravity force
sf::Vector2f gravityForce = {0, mass * 980.0f};
applyForce(gravityForce);
```

**Rule**: Gravity always points downward, proportional to mass

**Files using this**: `Newton_law.cpp`, `Cannon_sim.cpp`, `Energie.cpp`

### 4.3 Air Resistance / Drag Force
**Pattern**:
```cpp
float k = 0.5f;  // Drag coefficient
sf::Vector2f dragForce = vel * -k;
applyForce(dragForce);
```

**Rules**:
- Drag force is proportional to velocity: F = -k * v
- Always opposes motion direction
- Applies to both X and Y velocity components
- Heavier objects with more drag coefficient lose speed faster

**Files using this**: `Newton_law.cpp`

### 4.4 Friction Force
**Pattern** (from `two_balls_exp.cpp`):
```cpp
float gravity = 980.0f;
float mu = 0.2f;  // Coefficient of friction
float frictionForce = mu * mass * gravity;

if (std::abs(vx) > 1.0f) {
    ax = (vx > 0) ? -frictionForce : frictionForce;
} else {
    vx = 0;  // Stop completely if very slow
}
```

**Rules**:
- Friction force: F = μ * m * g (μ = coefficient of friction)
- Always opposes velocity direction
- Only applies if moving: `std::abs(vx) > 1.0f`
- Floors very small velocities to 0 (avoids eternal micro-movement)

**Files using this**: `two_balls_exp.cpp`

---

## 5. ENERGY CALCULATIONS

### 5.1 Kinetic Energy (KE)
**Formula**: KE = 1/2 * m * v²
```cpp
float ke = 0.5f * mass * speed * speed;
```

**Properties**:
- Depends on velocity (or speed)
- Energy of motion
- Maximum at lowest point (highest speed)

### 5.2 Potential Energy (PE)
**Formula**: PE = m * g * h (where h = height above reference)
```cpp
float h = GROUND_Y - (ball.pos.y + radius);
float pe = GRAVITY * h;  // mass = 1.0f
```

**Properties**:
- Depends on height above reference level
- Reference level defined arbitrarily (usually ground)
- Maximum at highest point
- Zero at reference level

### 5.3 Energy Conservation Principle
**Rule**: Total Energy = KE + PE = constant (in ideal systems without friction)

**Visual proof in code**: Energy bars show three values:
- KE bar (Kinetic Energy) — blue
- PE bar (Potential Energy) — red
- TOT bar (Total) — green (should remain stable)

**When it breaks**:
- Friction causes total energy to decrease
- Collisions with damping lose energy
- Air resistance converts energy to heat

**Files using this**: `Energie.cpp`

---

## 6. GRAVITATIONAL N-BODY SIMULATION

### 6.1 Newton's Law of Universal Gravitation
**Formula**: F = G * m1 * m2 / (r²)

**Implementation** (from `solar_system.cpp`):
```cpp
sf::Vector2f dir = other.position - position;
float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
if (dist < 1.f) return;  // Avoid division by zero
sf::Vector2f dirNorm = dir / dist;
float G = 0.1f;  // Gravitational constant (tuned for screen)
float accel = G * other.mass / (dist * dist);
velocity += dirNorm * accel * dt;
```

**Rules**:
- Force is proportional to both masses
- Force is inversely proportional to distance squared
- Direction: From object towards source of gravity
- Always attractive (no repulsion)
- Very large G value = very strong gravity = small orbits

### 6.2 Orbital Mechanics
**Circular Orbit Velocity Formula**: v = √(G * M / r)

**Pattern** (from `solar_systemV2.cpp`):
```cpp
double vEarth = std::sqrt(G * MS / R_EARTH);
```

**Rules**:
- Stable circular orbit requires specific velocity
- Faster than orbital velocity → object escapes
- Slower than orbital velocity → object falls inward
- Moon has velocity = Earth's velocity + Moon's orbital velocity

### 6.3 Gravitational Softening (Numerical Stability)
**Problem**: When two objects get very close, acceleration becomes infinite

**Solution** (from `solar_systemV2.cpp`):
```cpp
double softening = 1e8;
double distSoft = std::sqrt(dist*dist + softening*softening);
double F = G * a.mass * b.mass / (distSoft * distSoft);
```

**Rules**:
- Add small softening distance to prevent singularities
- Prevents numerical instability
- Allows objects to "pass through" without infinite forces

### 6.4 Reference Frame Management
**Pattern** (keep objects relative to sun):
```cpp
sf::Vector2<double> sunPos = bodies[0].pos;
for (auto& b : bodies)
    b.pos -= sunPos;
sf::Vector2<double> sunVel = bodies[0].vel;
for (auto& b : bodies)
    b.vel -= sunVel;
```

**Rules**:
- Keep simulation centered on most massive object (sun)
- Subtract sun's position/velocity from all objects
- Prevents floating-point precision loss from huge coordinate values
- Makes visualization easier (sun always at screen center)

### 6.5 Multiple Physics Steps Per Frame
**Pattern** (from `solar_systemV2.cpp`):
```cpp
constexpr double dt = 3600.0 * 24.0;  // 1 day per step
constexpr int STEPS_PER_FRAME = 24;   // Run 24 steps per render frame

for (int step = 0; step < STEPS_PER_FRAME; ++step) {
    // Apply gravity
    // Integrate positions
}
```

**Rules**:
- Large dt values speed up simulation visually
- Multiple small steps maintain numerical stability
- Total simulated time = dt * STEPS_PER_FRAME
- Trade-off: More steps = more accurate, slower computation

**Files using this**: `solar_system.cpp`, `solar_systemV2.cpp`

---

## 7. NUMERICAL PRECISION & STABILITY

### 7.1 Float vs Double
**Float** (32-bit):
- Used for screen coordinates (positions, velocities)
- Sufficient for visual accuracy
- Faster computation

**Double** (64-bit):
- Used in `solar_systemV2.cpp` for physics calculations
- Better precision for large-scale simulations
- Prevents floating-point accumulation errors

**Pattern** (from `solar_systemV2.cpp`):
```cpp
sf::Vector2<double> pos;   // Double precision for physics
sf::Vector2f screenPos;    // Float for rendering
```

### 7.2 Delta Time Integration
**Simple Euler method** (used in most files):
```cpp
velocity += acceleration * dt;
position += velocity * dt;
```

**Rules**:
- Restart clock after each frame: `float dt = clock.restart().asSeconds();`
- dt is typically 0.016 seconds (60 FPS) or smaller
- Smaller dt = more accurate but slower
- Velocity-Verlet would be more accurate but Euler is sufficient for visualization

### 7.3 Threshold Checks (Floating Point Comparisons)
**Pattern**:
```cpp
// Instead of: if (velocity == 0)
// Use: if (std::abs(velocity) < 1.0f)
if (std::abs(vx) > 1.0f) {
    // Apply friction
} else {
    vx = 0;  // Force to exact zero
}
```

**Rules**:
- Never compare floats with `==` (floating-point precision issues)
- Use absolute value with threshold: `std::abs(value) > THRESHOLD`
- Explicitly set to zero when below threshold
- Prevents "eternal micro-movement" from rounding errors

**Files using this**: `main.cpp`, `two_balls_exp.cpp`

---

## 8. RENDERING & VISUALIZATION

### 8.1 Shape Positioning
**Rule**: Set origin at shape center for accurate physics
```cpp
sf::CircleShape ball(radius);
ball.setOrigin({radius, radius});  // Center origin for proper rotation/collision
ball.setPosition(physicsPos);
```

### 8.2 Trail Visualization (Orbit Paths)
**Pattern**:
```cpp
std::deque<sf::Vector2f> trail;
size_t maxtail = 300;  // Maximum trail length

trail.push_back(position);
if (trail.size() > maxtail)
    trail.pop_front();
```

**Rules**:
- Use `std::deque` for efficient front removal
- Limit trail size to prevent memory bloat
- Draw using `sf::PrimitiveType::LineStrip`
- Optional: Fade alpha values based on age

### 8.3 Camera/View System
**Pattern** (from `solar_system.cpp`):
```cpp
sf::View camera = window.getDefaultView();

// Zoom with mouse wheel
if (scroll->delta > 0)
    camera.zoom(0.9f);  // Zoom in
else
    camera.zoom(1.1f);  // Zoom out

// Pan with middle mouse button
if (panning) {
    sf::Vector2i delta = lastMousePos - mm->position;
    camera.move(sf::Vector2f(delta));
    lastMousePos = mm->position;
}
```

**Rules**:
- Middle mouse button drag for panning
- Mouse wheel scroll for zoom
- zoom() factor < 1.0 = zoom in, > 1.0 = zoom out

### 8.4 Energy Bars Visualization
**Pattern** (from `Energie.cpp`):
```cpp
float maxE = 80000.f;
float barW = 28.f;
float barBaseY = 595.f;
float maxBarH = 140.f;

auto makeBar = [&](float value, float bx, sf::Color col) -> sf::RectangleShape {
    float bh = std::min(value / maxE * maxBarH, maxBarH);
    sf::RectangleShape bar({barW, bh});
    bar.setFillColor(col);
    bar.setPosition({bx, barBaseY - bh});
    return bar;
};
```

**Rules**:
- Normalize value: `normalized = (value / maxValue) * maxHeight`
- Position bars from bottom: `y = baseY - barHeight`
- Use different colors for different quantities (KE=blue, PE=red, Total=green)

---

## 9. CONSTANT VALUES & SCALING

### 9.1 Physics Constants
```
GRAVITY = 980.0f  (pixels/s²) — tuned for screen scale
PI = 3.14159265f
G = 0.1f to 6.674e-11f (depends on scale and mass units)
```

### 9.2 Screen Scale Conversions
**From `solar_systemV2.cpp`**:
```cpp
constexpr double SCALE = 1.0 / 1.5e9;  // 1 pixel = 1.5e9 metres
constexpr double MS = 1e20;  // Sun mass in simulation units
constexpr double ME = 3e17;  // Earth mass in simulation units
```

**Pattern**: Scale real-world values to screen coordinates
- Real orbital radius: ~1.5×10¹¹ m
- Screen distance: ~200 pixels
- Scale factor adjusts real physics to visible motion

---

## 10. COMMON PATTERNS & CODE ORGANIZATION

### 10.1 Struct-Based Physics Objects
**Pattern**:
```cpp
struct Ball {
    sf::CircleShape shape;
    sf::Vector2f pos, vel, acc;
    float mass;

    void applyForce(sf::Vector2f force);
    void update(float dt);
};
```

**Benefits**:
- Encapsulates physics properties
- Easy to manage multiple objects in `std::vector<Ball>`
- Clean update pattern: loop through all objects, call update()

### 10.2 Event Handling Loop
**Pattern**:
```cpp
while (const std::optional event = window.pollEvent()) {
    if (event->is<sf::Event::Closed>())
        window.close();
    
    if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
        // Handle key release
    }
}

// Continuous input (for movement)
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    // Handle held key
```

**Rules**:
- `window.pollEvent()` for one-time events (key press/release, mouse click)
- `sf::Keyboard::isKeyPressed()` for continuous input (key held down)

### 10.3 Clock & Delta Time
**Pattern**:
```cpp
sf::Clock clock;

while (window.isOpen()) {
    float dt = clock.restart().asSeconds();
    
    // Update physics with dt
    ball.update(dt);
}
```

**Rules**:
- Create clock before main loop
- Call `restart()` to get elapsed time and reset timer
- `asSeconds()` gives float value (e.g., 0.016 for 60 FPS)
- Frame rate independent: faster computers = smaller dt = same result

---

## 11. KNOWN ISSUES & SOLUTIONS

### 11.1 Floating Point Micro-Bouncing
**Issue**: Ball never stops bouncing due to accumulating rounding errors
**Root cause**: Velocity becomes tiny (~0.000001) but non-zero
**Solution**: Force velocity to zero below threshold
```cpp
if (std::abs(velocity.y) > THRESHOLD) {
    velocity.y *= -0.7f;
} else {
    velocity.y = 0;
}
```

### 11.2 Division by Zero (Gravity)
**Issue**: Objects too close cause division by zero or huge accelerations
**Solution 1**: Check distance threshold
```cpp
float dist = std::sqrt(dx*dx + dy*dy);
if (dist < 1.f) return;
```

**Solution 2**: Use softening (adds damping near singularity)
```cpp
double distSoft = std::sqrt(dist*dist + softening*softening);
```

### 11.3 Screen Wrapping vs Boundaries
**Wrapping** (infinite space):
```cpp
if (x > 800) x = 0;
if (x < 0) x = 800;
```

**Boundaries** (solid walls):
```cpp
if (x > 780) { x = 780; vx *= -damping; }
```

### 11.4 Font Loading
**Pattern**:
```cpp
if (!font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
    std::cerr << "Font not found!" << std::endl;
}
```

**Check before drawing**:
```cpp
if (font.getLineSpacing(20) != 0) {
    window.draw(uiText);
}
```

---

## 12. SIMULATION TUNING PARAMETERS

| Parameter | File | Default | Effect |
|-----------|------|---------|--------|
| Gravity (g) | main.cpp | 980.0f | Stronger = faster falling |
| Bounce damping | main.cpp | 0.7f | Lower = less bouncing |
| Bounce threshold | main.cpp | 30.0f | Higher = stops bouncing sooner |
| Friction coefficient (μ) | two_balls_exp.cpp | 0.2f | Higher = more friction |
| Drag coefficient (k) | Newton_law.cpp | 0.5f | Higher = more air resistance |
| G constant | solar_system.cpp | 0.1f | Higher = stronger gravity |
| Trail max length | solar_system.cpp | 300 | More = longer trails |
| Softening distance | solar_systemV2.cpp | 1e8 | Higher = smoother collisions |
| Physics dt | solar_systemV2.cpp | 86400.0 | Higher = faster simulation |

---

## 13. COMPILATION & DEPENDENCIES

- **Graphics Library**: SFML (Simple and Fast Multimedia Library)
- **C++ Standard**: C++17 or later
- **Build**: Using Makefile
- **Required includes**:
  ```cpp
  #include <SFML/Graphics.hpp>
  #include <cmath>
  #include <iostream>
  #include <vector>
  ```

---

## Summary of Key Physics Principles

1. **F = ma** — Force equals mass times acceleration
2. **v = v₀ + at** — Velocity update with acceleration
3. **x = x₀ + vt** — Position update with velocity
4. **KE = ½mv²** — Kinetic energy depends on velocity
5. **PE = mgh** — Potential energy depends on height
6. **F_gravity = mg** — Gravitational force
7. **F_friction = μmg** — Friction opposes motion
8. **F_drag = -kv** — Air resistance proportional to velocity
9. **F_gravity = Gm₁m₂/r²** — Universal gravitation
10. **v_orbit = √(GM/r)** — Circular orbit velocity

