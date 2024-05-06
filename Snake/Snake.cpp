#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <vector>

using namespace std;

const int MAX_WIDTH = 30;
const int MAX_HEIGHT = 30;
const int DEFAULT_WIDTH = 20;
const int DEFAULT_HEIGHT = 20;
const int DEFAULT_SPEED = 100;

struct Object {
    int x, y;
};

enum class Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct GameSettings {
    int width;
    int height;
    int speed;
    int obstacleFrequency;
    int fruitSpawnRate;
};

struct GameState {
    bool gameOver;
    int score;
    Direction dir;
    Object snake;
    vector<Object> tail; // Represent the snake's tail
    vector<Object> obstacles;
    vector<Object> fruits;
};

void Setup(GameState& state, GameSettings& settings) {
    state.gameOver = false;
    state.score = 0;
    state.dir = Direction::STOP;
    state.snake.x = settings.width / 2;
    state.snake.y = settings.height / 2;
    state.tail.clear(); // Clear the snake's tail

    srand(time(NULL));

    for (int i = 0; i < 2; i++) {
        state.fruits.push_back({ rand() % settings.width, rand() % settings.height });
    }

    for (int i = 0; i < 3; i++) {
        state.obstacles.push_back({ rand() % settings.width, rand() % settings.height });
    }
}

void Draw(GameState& state, GameSettings& settings) {
    system("cls");
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = 0; i < settings.width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < settings.height; i++) {
        for (int j = 0; j < settings.width; j++) {
            if (j == 0)
                cout << "#";

            bool print = false;
            if (i == state.snake.y && j == state.snake.x) {
                cout << "O";
                print = true;
            }
            else {
                bool isTailSegment = false;
                for (auto& tail : state.tail) {
                    if (j == tail.x && i == tail.y) {
                        cout << "o"; // Tail segment
                        print = true;
                        isTailSegment = true;
                        break;
                    }
                }
                if (!isTailSegment) {
                    for (auto& obstacle : state.obstacles) {
                        if (j == obstacle.x && i == obstacle.y) {
                            cout << "X";
                            print = true;
                            break;
                        }
                    }
                    if (!print) {
                        for (auto& fruit : state.fruits) {
                            if (j == fruit.x && i == fruit.y) {
                                cout << "F";
                                print = true;
                                break;
                            }
                        }
                        if (!print)
                            cout << " ";
                    }
                }
            }

            if (j == settings.width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < settings.width + 2; i++)
        cout << "#";
    cout << endl;
    cout << "Score:" << state.score << endl;
}

void Input(GameState& state) {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            state.dir = Direction::LEFT;
            break;
        case 'd':
            state.dir = Direction::RIGHT;
            break;
        case 'w':
            state.dir = Direction::UP;
            break;
        case 's':
            state.dir = Direction::DOWN;
            break;
        case 'x':
            state.gameOver = true;
            break;
        }
    }
}

void Logic(GameState& state, GameSettings& settings) {
    int prevX = state.snake.x;
    int prevY = state.snake.y;

    // Move the tail
    if (!state.tail.empty()) {
        int prev2X = state.snake.x;
        int prev2Y = state.snake.y;
        int tempX, tempY;
        for (size_t i = 0; i < state.tail.size() - 1; i++) {
            tempX = state.tail[i].x;
            tempY = state.tail[i].y;
            state.tail[i].x = prev2X;
            state.tail[i].y = prev2Y;
            prev2X = tempX;
            prev2Y = tempY;
        }
        state.tail.back().x = prevX;
        state.tail.back().y = prevY;
    }

    switch (state.dir) {
    case Direction::LEFT:
        state.snake.x--;
        break;
    case Direction::RIGHT:
        state.snake.x++;
        break;
    case Direction::UP:
        state.snake.y--;
        break;
    case Direction::DOWN:
        state.snake.y++;
        break;
    default:
        break;
    }

    if (state.snake.x >= settings.width) state.snake.x = 0;
    else if (state.snake.x < 0) state.snake.x = settings.width - 1;
    if (state.snake.y >= settings.height) state.snake.y = 0;
    else if (state.snake.y < 0) state.snake.y = settings.height - 1;

    // Check collision with obstacles
    for (auto& obstacle : state.obstacles) {
        if (state.snake.x == obstacle.x && state.snake.y == obstacle.y) {
            state.gameOver = true;
            cout << "Collision with obstacle!" << endl;
            return;
        }
    }

    // Check collision with fruits
    for (auto& fruit : state.fruits) {
        if (state.snake.x == fruit.x && state.snake.y == fruit.y) {
            state.score += 10;
            fruit.x = rand() % settings.width;
            fruit.y = rand() % settings.height;
            state.tail.push_back({ prevX, prevY }); // Add segment to the tail
            settings.speed -= 5;
            cout << "Fruit collected!" << endl;
            break;
        }
    }

    // Dynamic difficulty adjustment
    if (state.score >= 100 && settings.speed > 50) {
        settings.speed -= 5; // Increase speed
    }
}

void GameOver(GameState& state) {
    system("cls");
    cout << "Game Over!" << endl;
    cout << "Final Score: " << state.score << endl;
    cout << "Press 'R' to restart or 'X' to quit." << endl;

    char userInput;
    do {
        userInput = _getch();
    } while (userInput != 'r' && userInput != 'R' && userInput != 'x' && userInput != 'X');

    if (userInput == 'r' || userInput == 'R') {
        GameSettings settings = { DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_SPEED, 5, 10 };
        Setup(state, settings);
    }
    else {
        state.gameOver = true;
    }
}

int main() {
    GameState state;
    GameSettings settings = { DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_SPEED, 5, 10 };
    Setup(state, settings);

    while (!state.gameOver) {
        Draw(state, settings);
        Input(state);
        Logic(state, settings);
        Sleep(settings.speed);
    }

    GameOver(state);

    return 0;
}
