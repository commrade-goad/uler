#include <iostream>
#include <curses.h>
#include <chrono>
#include <vector>
#include <random>

#define BOARD_X 20
#define BOARD_Y 10
#define SPEED 0.2

enum Movement {
    RIGHT,
    LEFT,
    UP,
    DOWN,
};

struct Coords {
    int x;
    int y;
};

void init_curses() {
    initscr();
    raw();
    curs_set(0);
    nodelay(stdscr, true);
    timeout(0);
}

int gen_rand_num(size_t min, size_t max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(min, max);
    return distr(gen);
}

Coords gen_food(std::vector<Coords> uler_pos) {
    Coords result = {
        .x = gen_rand_num(1, BOARD_X-2),
        .y = gen_rand_num(1, BOARD_Y-2)
    };
    for (Coords body: uler_pos) {
        if (result.x == body.x && result.y == body.y) {
            gen_food(uler_pos);
        }
    }
    return result;
}

int main() {
    init_curses();
    Coords uler = {BOARD_X/2,BOARD_Y/2};
    size_t uler_len = 30;
    std::vector<Coords> uler_snapshot;
    Coords food = gen_food(uler_snapshot);
    int ch;
    Movement move = RIGHT;
    auto movement = std::chrono::high_resolution_clock::now();

    int running = 1;
    while (running == 1) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto current_time_interval = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - movement);
        for (int y = 0; y < BOARD_Y; y++) {
            for (int x = 0; x < BOARD_X; x++) {
                if (y == 0 || y == BOARD_Y - 1 || x == 0 || x == BOARD_X - 1) {
                    mvprintw(y, x, "#");
                } else {
                    mvprintw(y, x, " ");
                }
            }
        }
        mvprintw(food.y, food.x, "$");
        if (uler.x == food.x && uler.y == food.y) {
            food = gen_food(uler_snapshot);
            uler_len += 1;
        }

        if (uler_snapshot.size() > uler_len) {
            uler_snapshot.erase(uler_snapshot.begin());
        }
        mvprintw(uler.y, uler.x, "@");
        if (current_time_interval.count() >= SPEED) {
            uler_snapshot.push_back({uler.x, uler.y});
            switch (move) {
                case RIGHT: {
                    if (uler.x < BOARD_X - 2) {
                        uler.x += 1;
                    } else {
                        uler.x = 1;
                    }
                    break;
                }
                case LEFT: {
                    if (uler.x > 1) {
                        uler.x -= 1;
                    } else {
                        uler.x = BOARD_X - 2;
                    }
                    break;
                }
                case DOWN: {
                    if (uler.y < BOARD_Y - 2) {
                        uler.y += 1;
                    } else {
                        uler.y = 1;
                    }
                    break;
                }
                case UP: {
                    if (uler.y > 1) {
                        uler.y -= 1;
                    } else {
                        uler.y = BOARD_Y - 2;
                    }
                    break;
                }

            }
            movement = std::chrono::high_resolution_clock::now();
        }
        for (size_t i = 0; i < uler_snapshot.size(); i++) {
            mvprintw(uler_snapshot[i].y, uler_snapshot[i].x, "*");
        }

        for (int j = 0; j < (int) uler_snapshot.size(); j++) {
            if (uler.x == uler_snapshot[j].x && uler.y == uler_snapshot[j].y) {
                endwin();
                return 0;
            }
        }

        mvprintw(0, 1, "SCORE: %lu", uler_len);

        ch = getch();
        switch (ch) {
            case 'w':{
                move = UP;
                break;
            }
            case 'a':{
                move = LEFT;
                break;
            }
            case 's':{
                move = DOWN;
                break;
            }
            case 'd':{
                move = RIGHT;
                break;
            }
            case 'q': {
                endwin();
                return 0;
            }
            default:break;
        }
        curs_set(0);
        refresh();
    }
    endwin();
    return 0;
}
