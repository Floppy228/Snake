#include <time.h>
#include <conio.h>
#include <stdint.h>
#include <iostream>
#include <windows.h>
using namespace std;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // Create a handle for the output stream
CONSOLE_CURSOR_INFO cci = { sizeof(cci), false }; // Create parameters for displaying the cursor
string continue_game = "y";
int16_t N = 1, M = 1; // N - width M - length

int16_t EntryDimension(int16_t v) {
    char requests[2][50] = { "Enter the length: ",
                 "Enter the width: " };
    int16_t number;
    char *end = nullptr;
    std::cout << requests[v];

    while (true) {
        char dimension[50];
        fgets(dimension, 50, stdin);
        number = (int16_t)strtol(dimension, &end, 10);
        if (*end != '\n' || number <= 0) {
            std::cout << "Please enter a natural number: ";
        }
        else { break; }
    }
    return number;
}

void PrintArea() {
    system("cls");
    SetConsoleTextAttribute(h, 7);
    std::cout << (char)218; // upper left corner
    for (int16_t i = 0; i < M; ++i)
        std::cout << (char)196; // top and bottom borders
    std::cout << (char)191; // upper right corner
    std::cout << '\n';
    for (int16_t i = 0; i < N; ++i) {
        std::cout << (char)179; // left and right borders
        for (int16_t j = 0; j < M; ++j)
            std::cout << (char)32;
        std::cout << (char)179; // left and right borders
        std::cout << '\n';
    }
    std::cout << (char)192; // lower left corner
    for (int16_t i = 0; i < M; ++i)
        std::cout << (char)196; // top and bottom borders
    std::cout << (char)217; // lower right corner
    std::cout << '\n';
}


bool CheckCorrectPosition(int16_t **arr, int16_t length, int16_t x, int16_t y) {
    x += arr[length - 1][0];
    y += arr[length - 1][1];

    for (int16_t i = 0; i <= length; ++i) {
        if (arr[i][0] == x && arr[i][1] == y)
            return false;
    }
    if (x < 1 || x > M || y < 1 || y > N) { return false; }

    return true;
}


void SetCursorAndPrint(COORD coord, int16_t pos_x, int16_t pos_y, char c, int16_t color) {
    coord.X = pos_x, coord.Y = pos_y;
    SetConsoleTextAttribute(h, color);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    std::cout << c;
}


struct FOOD {
    int16_t x, y;
    void GenerateFood(COORD &coord, int16_t **pos, int16_t length) {
        do {
            x = rand() % (M + 1);
            y = rand() % (N + 1);
        } while (!CheckCorrectPosition(pos, length, x - pos[length - 1][0],
            y - pos[length - 1][1]));
        SetCursorAndPrint(coord, x, y, 'o', 14);
    }
} food;


struct SNAKE {
    char l_head;
    int16_t **pos, length, l_x, l_y, extra_block_x, extra_block_y;

    void SetPosition(COORD &coord, int16_t setX, int16_t setY, char head) {
        if ((l_head == '<' && head == '>' || l_head == '>' && head == '<' ||
            l_head == '^' && head == 'v' || l_head == 'v' && head == '^') &&
            length > 1) {
            setX = l_x, setY = l_y, head = l_head;
        }

        SetCursorAndPrint(coord, pos[length - 1][0], pos[length - 1][1], head, 10);
        if (CheckCorrectPosition(pos, length, setX, setY)) {
            extra_block_x = pos[0][0], extra_block_y = pos[0][1];
            l_x = setX, l_y = setY, l_head = head;

            for (int16_t i = 0; i < length - 1; ++i) {
                pos[i][0] = pos[i + 1][0];
                pos[i][1] = pos[i + 1][1];
                SetCursorAndPrint(coord, pos[i][0], pos[i][1], 'O', 10);
            }
            pos[length - 1][0] += setX;
            pos[length - 1][1] += setY;
            SetCursorAndPrint(coord, pos[length - 1][0], pos[length - 1][1], head, 10);
            if (pos[length - 1][0] == food.x && pos[length - 1][1] == food.y) {
                for (int16_t i = length; i > 0; --i) {
                    pos[i][0] = pos[i - 1][0];
                    pos[i][1] = pos[i - 1][1];
                }
                pos[0][0] = extra_block_x;
                pos[0][1] = extra_block_y;
                SetCursorAndPrint(coord, pos[0][0], pos[0][1], 'O', 10);
                ++length;
                if (length < N * M)
                    food.GenerateFood(coord, pos, length);
            }
            else {
                SetCursorAndPrint(coord, extra_block_x, extra_block_y, ' ', 7);
            }

        }
        else { continue_game = "n"; }
    }
} snake;


struct DELAY {
    int32_t second;

    void SetSpeed(char act) {
        do {
            if (act == 43) {    // Increase speed
                if ((second -= 50) < 50)
                    second = 50;
            }
            else if (act == 45) {  // Decrease speed
                if ((second += 50) > 1000)
                    second = 1000;
            }
            else if (act == 32) {  // Pause
                while (_getch() != 32) {
                    Sleep(300);
                }
            }
            else { break; }
            if (_kbhit()) {
                act = _getch();
                if (act != 43 && act != 45) {
                    _ungetch(act); break;
                }
            }
            else break;
        } while (true);


    }
} delay;


int main()
{
    system("title Snake");
    keybd_event(VK_MENU, 0x38, 0, 0); //press ALT
    keybd_event(VK_RETURN, 0x1c, 0, 0); //press ENTER
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0); //release ENTER
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0); //release ALT
    SetConsoleCursorInfo(h, &cci); //Linking parameters and handle
    srand((unsigned int)time(NULL));
    char act = 0; // Stores key presses
    COORD coord{ 1, 1 }; // Working with cursor coordinates
    int16_t best_score = 0; // Best score

    cout << "*******Control*******\n\nA, <, 4 : left\n"
        "W, ^, 8 : up\nD, >, 6 : right\nS, v, 2 : down\n\n"
        "- : reduce speed\n+ : increase speed\nescape : pause\n\nesc : exit\n\n";

    M = EntryDimension(0); // Input the length of the game field
    N = EntryDimension(1); // Input the width of the game field

    // Dynamic memory allocation for snake coordinates
    int16_t **pos = new int16_t * [M * N];
    for (int16_t i = 0; i < M * N; ++i) {
        pos[i] = new int16_t[2];
    }

    while (continue_game == "y")
    {
        pos[0][0] = 1, pos[0][1] = 1; // Initial coordinates
        snake = { '>', pos, 1, 1, 0 };
        delay = { 300 };

        // Drawing the field, head, and food
        PrintArea();
        SetCursorAndPrint(coord, 1, 1, snake.l_head, 7);
        food.GenerateFood(coord, snake.pos, snake.length);

        while (continue_game == "y" && snake.length != N * M)
        {
            Sleep(delay.second);
            if (_kbhit()) {
                do { act = _getch(); } while (act == -32);
                switch (act) {
                    case 52: case 75: case 97:  snake.SetPosition(coord, -1, 0, '<'); break; // Left
                    case 56: case 72: case 119: snake.SetPosition(coord, 0, -1, '^'); break; // Up
                    case 54: case 77: case 100: snake.SetPosition(coord, +1, 0, '>'); break; // Right
                    case 50: case 80: case 115: snake.SetPosition(coord, 0, +1, 'v'); break; // Down
                    case 27: return 0;
                    default: case 43: case 45: delay.SetSpeed(act);
                        snake.SetPosition(coord, snake.l_x, snake.l_y, snake.l_head);
                }
            }
            else { snake.SetPosition(coord, snake.l_x, snake.l_y, snake.l_head); }

            coord.X = 0;
            coord.Y = N + 2;
            SetConsoleTextAttribute(h, 7);
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
            if (best_score < snake.length - 1) { best_score = snake.length - 1; } // best score
            std::cout << "Score: " << snake.length - 1 << "\tBest score : " << best_score << endl;
        }
        std::cout << (snake.length == N * M ? "You win!" : "Game over.") << " Restart [y/n]? ";
        std::cin >> continue_game;
        // Clearing the snake coordinates array
        for (int16_t i = 0; i < M * N; ++i) {
            pos[i][0] = 0, pos[i][1] = 0;
        }
    }
    for (uint16_t i = 0; i < N; ++i)
        delete[] pos[i];
    delete[] pos;
    return 0;
}