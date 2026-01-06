/// @author Manolea Teodor-Stefan
/// @project Snake in C++ using SFML

#include <SFML/Graphics.hpp>
#include <list>
#include <string>
#include <iostream>

bool isOccupied[32][24];

namespace DIRECTION {
    enum {
        STOP  = -1,
        LEFT  = 1,
        UP    = 2,
        RIGHT = 3,
        DOWN  = 4
    };
}
namespace SCREEN {
    const int MIN_X = 25;
    const int MIN_Y = 25;
    const int MAX_X = 775;
    const int MAX_Y = 575;
}

class apple {
    private:
    public:
    static bool exists;
    static sf::CircleShape fruit;
    static int eaten;

    static void init() {
        fruit = sf::CircleShape(10.f);
        fruit.setFillColor(sf::Color(255,0,0));
        fruit.setOutlineThickness(2.5f);
        fruit.setOutlineColor(sf::Color(105,53,61));

        generator();
    }

    static void reset() {
        const auto [x,y] = fruit.getPosition();
        isOccupied[static_cast<int>(x/25)][static_cast<int>(y/25)] = false;
        fruit = sf::CircleShape(0);
        eaten = 0;
    }

    static void generator() {
        int16_t x,y;
        bool ok = false;
        while (!ok) {
            x = rand() % 31;
            y = rand() % 23;
            if (x == 0 or y == 0) {
                continue;
            }
            if (!isOccupied[x][y]) {
                ok = true;
                fruit.setPosition({x*25.f,y*25.f});
            }
        }

    }
};
bool apple::exists = false;
int  apple::eaten  = 0;
sf::CircleShape apple::fruit;

class snake {
    public:
    static sf::CircleShape TemplateBlob;
    static std::list<sf::CircleShape> body;
    static int8_t direction;

    static void init() {
        TemplateBlob = sf::CircleShape(10.f);
        TemplateBlob.setFillColor(sf::Color(0,255,0));
        TemplateBlob.setOutlineThickness(2.5f);
        TemplateBlob.setOutlineColor(sf::Color(53,105,61));
        TemplateBlob.setPosition({375,275});

        direction = DIRECTION::STOP;

        body = std::list<sf::CircleShape>();
        body.push_back(TemplateBlob);
    }

    static void reset() {
        for (const auto& blob : body) {
            const auto [x,y] = blob.getPosition();
            isOccupied[static_cast<int>(x/25)][static_cast<int>(y/25)] = false;
        }
        body.clear();
        direction = DIRECTION::STOP;
    }

    static void changeDirection(const int8_t dir) {
        direction = dir;
    }

};
int8_t snake::direction;
sf::CircleShape snake::TemplateBlob;
std::list<sf::CircleShape> snake::body;


class game {
    private:
    static sf::Clock gameTimer;

    static void gameOver() {
        status = OVER;
        snake::reset();
        apple::reset();
        score = "Score: 0";
        gameTimer.stop();
        gameTimer.reset();
    }

    public:
    enum {
        STOP    = 0,
        RUNNING = 1,
        OVER    = 2
    };

    static int8_t status;
    static bool canUpdate;
    static std::string score;

    static void start() {
        snake::init();
        apple::init();
        apple::generator();

        gameTimer.start();

        canUpdate = true;
        status    = RUNNING;
    }

    static void update() {
        if (gameTimer.getElapsedTime().asSeconds() < 0.37f) {
            return;
        }

        canUpdate = true;

        auto prevBlob = snake::body.front();

        auto [x,y] = prevBlob.getPosition();

        for (auto& blob : snake::body) {
            if (blob.getPosition() != prevBlob.getPosition()) {
                auto temp = blob;
                blob.setPosition(prevBlob.getPosition());
                blob.setPosition(prevBlob.getPosition());
                prevBlob = temp;
            }
        }

        isOccupied[static_cast<int>(prevBlob.getPosition().x/25)][static_cast<int>(prevBlob.getPosition().y/25)] = false;

        switch (snake::direction) {

            case DIRECTION::UP :
                snake::body.front().setPosition({x,y-25.f});
                break;
            case DIRECTION::LEFT :
                snake::body.front().setPosition({x-25.f,y});
                break;
            case DIRECTION::DOWN:
                snake::body.front().setPosition({x,y+25.f});
                break;
            case DIRECTION::RIGHT :
                snake::body.front().setPosition({x+25.f,y});
                break;
            default:
                break;
        }

        x = snake::body.front().getPosition().x;
        y = snake::body.front().getPosition().y;

        if (x >= SCREEN::MAX_X) {
            x = SCREEN::MIN_X;
        } else if (x < SCREEN::MIN_X) {
            x = SCREEN::MAX_X - 25;
        }

        if (y >= SCREEN::MAX_Y) {
            y = SCREEN::MIN_Y;
        } else if (y < SCREEN::MIN_Y) {
            y = SCREEN::MAX_Y - 25;
        }

        snake::body.front().setPosition({x,y});
        if (isOccupied[static_cast<int>(x/25)][static_cast<int>(y/25)]) {
            gameOver();
            return;
        }

        isOccupied[static_cast<int>(x/25)][static_cast<int>(y/25)] = true;

        if (snake::body.front().getPosition() == apple::fruit.getPosition()) {
            snake::body.push_back(prevBlob);

            apple::eaten++;

            x = snake::body.back().getPosition().x;
            y = snake::body.back().getPosition().y;
            isOccupied[static_cast<int>(x/25)][static_cast<int>(y/25)] = true;

            score = "Score: " + std::to_string(apple::eaten);

            apple::generator();
        }

        gameTimer.reset();
        gameTimer.start();
    }
};
int8_t game::status = game::RUNNING;
bool game::canUpdate = true;
std::string game::score = "Score: 0";
sf::Clock game::gameTimer;

int main() {
    game::start();

    auto window = sf::RenderWindow(sf::VideoMode({800u, 600u}), "I like apples :P", sf::Style::Close);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    sf::Texture floorTexture("floor.png");
    floorTexture.setSmooth(true);
    floorTexture.setRepeated(true);
    sf::Sprite floor(floorTexture,sf::IntRect({5,5},{800,600}));
    sf::Texture wallTexture("wall.png");
    wallTexture.setSmooth(true);
    sf::Sprite wall(wallTexture,sf::IntRect({0,0},{25,25}));

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto keyPressed= event->getIf<sf::Event::KeyPressed>()) {
                int8_t prevDirection = snake::direction;

                switch (keyPressed->scancode) {
                    case sf::Keyboard::Scancode::Up :
                    case sf::Keyboard::Scancode::W :
                        if (snake::direction != DIRECTION::DOWN and game::canUpdate) {
                            snake::changeDirection(DIRECTION::UP);
                        }
                        break;
                    case sf::Keyboard::Scancode::Left :
                    case sf::Keyboard::Scancode::A :
                        if (snake::direction != DIRECTION::RIGHT and game::canUpdate) {
                            snake::changeDirection(DIRECTION::LEFT);
                        }
                        break;
                    case sf::Keyboard::Scancode::Down :
                    case sf::Keyboard::Scancode::S :
                        if (snake::direction != DIRECTION::UP and game::canUpdate) {
                            snake::changeDirection(DIRECTION::DOWN);
                        }
                        break;
                    case sf::Keyboard::Scancode::Right :
                    case sf::Keyboard::Scancode::D :
                        if (snake::direction != DIRECTION::LEFT and game::canUpdate) {
                            snake::changeDirection(DIRECTION::RIGHT);
                        }
                        break;
                    case sf::Keyboard::Scancode::Space :
                        switch (game::status) {
                            case game::STOP :
                                game::status = game::RUNNING;
                                break;
                            case game::OVER :
                                game::start();
                                break;
                            default:
                                break;
                        }
                        break;
                    case sf::Keyboard::Scancode::Escape :
                        if (game::status == game::STOP) {
                            window.close();
                        }
                        game::status = game::STOP;
                        break;
                    default:
                        break;
                }

                if (prevDirection != snake::direction and game::status != game::OVER) {
                    game::canUpdate = false;
                }
            } else if (event->is<sf::Event::FocusLost>()) {
                if (game::status != game::OVER) {
                    game::status = game::STOP;
                }
            }
        }

        window.clear(sf::Color::Black);

        if (game::status != game::OVER) {
            window.draw(floor);
            for (int i = 0; i <= SCREEN::MAX_Y; i += 25) {
                wall.setPosition({0,1.f*i});
                window.draw(wall);
                wall.setPosition({SCREEN::MAX_X,1.f*i});
                window.draw(wall);
            }
            for (int i = 0; i <= SCREEN::MAX_X; i += 25) {
                wall.setPosition({1.f*i,0});
                window.draw(wall);
                wall.setPosition({1.f*i,SCREEN::MAX_Y});
                window.draw(wall);
            }
        }

        if (game::status == game::RUNNING) {
            game::update();
        }

        window.draw(apple::fruit);
        for (const auto &blob: snake::body) {
            window.draw(blob);
        }

        if (game::status != game::OVER) {
            sf::Font font("pixelFont.ttf");
            sf::Text text(font);
            text.setString(game::score);
            text.setCharacterSize(23);
            text.setFillColor(sf::Color::White);
            text.setPosition({0,0});
            window.draw(text);
        }

        if (game::status == game::STOP) {
            sf::Font font("pixelFont.ttf");
            sf::Text text(font);
            text.setString("             Game Paused\nPress space to continue");
            text.setCharacterSize(36);
            text.setFillColor(sf::Color::White);
            text.setPosition({window.getSize().x / 3.f - 65, window.getSize().y / 3.f - 90});
            window.draw(text);
        } else if (game::status == game::OVER) {
            sf::Font font("pixelFont.ttf");
            sf::Text text(font);
            text.setString("            Game Over\nPress space to start");
            text.setCharacterSize(36);
            text.setFillColor(sf::Color(128, 32, 12));
            text.setPosition({window.getSize().x / 3.f - 65, window.getSize().y / 3.f - 80});
            window.draw(text);
        }

        window.display();
    }

    return 0;
}