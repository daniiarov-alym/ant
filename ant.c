#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <pthread.h>
int done = 0;
const int DELAY = 50; //100 ms
int width;
int height;
char **field;
WINDOW* w;
const char BLANK = ' ';
const char FILLED = '#';
const char ANT = '8';

struct Ant {
    int y;
    int x;
    char under;
    int dy;
    int dx;
};

struct Ant ant;

int curr_dir_y = -1;
int curr_dir_x = 0;

void curses_init() {
    w = initscr();
    noecho();
    setlocale(LC_ALL, "");
    raw();
    curs_set(0);
    keypad(stdscr, true);  
}

void field_init() {
    ant.y = height/2;
    ant.x = width/2;
    ant.under = FILLED;
    ant.dy = -1;
    ant.dx = 0;
    field = (char**)malloc(sizeof(char*) * height);
    for (int i = 0; i < height; i++) { 
        field[i] = (char*)malloc(width);
        for (int j = 0; j < width; j++) {
            field[i][j] = BLANK;
        }
    }
    field[ant.y][ant.x] = ANT;
}

void move_ant() {
    int ny = ant.y + ant.dy;
    int nx = ant.x + ant.dx;
    if (nx >= width) {
        nx = 0;
    }
    else if (nx < 0) {
        nx = width-1;
    }
    if (ny >= height) {
        ny = 0;
    }
    else if (ny < 0) {
        ny = height-1;
    }
    ant.y = ny;
    ant.x = nx;
    int ndy;
    int ndx;
    if (field[ant.y][ant.x] == BLANK) {
        if (ant.dy == -1) {
            ndy = 0;
            ndx = 1;
        }
        else if (ant.dy == 1) {
            ndy = 0;
            ndx = -1;
        }
        else if (ant.dx == -1) {
            ndy = -1;
            ndx = 0;
        }
        else if (ant.dx == 1) {
            ndy = 1;
            ndx = 0;
        }
    }
    else if (field[ant.y][ant.x] == FILLED) {
        if (ant.dy == -1) {
            ndy = 0;
            ndx = -1;
        }
        else if (ant.dy == 1) {
            ndy = 0;
            ndx = 1;
        }
        else if (ant.dx == -1) {
            ndy = 1;
            ndx = 0;
        }
        else if (ant.dx == 1) {
            ndy = -1;
            ndx = 0;
        }
    }
    ant.dy = ndy;
    ant.dx = ndx;
    ant.under = (field[ant.y][ant.x] == BLANK) ? FILLED : BLANK; 
}

void update() {
    int cy = ant.y;
    int cx = ant.x;
    char cunder = ant.under;
    move_ant();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            char curr = field[i][j];
            if (i == cy && j == cx) {
                field[i][j] = cunder;
            }
            else if (i == ant.y && j == ant.x) {
                field[i][j] = ANT;
            }
        }
    }
}

void render() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            char texture = field[i][j];            
            mvaddch(i,j,texture);
        }
    }
}


void *control_function() {
    int ch;
    int d = done; 
    for (;;) {
        ch = wgetch(w);
        if (ch == 'q') {
            done = 1;
        }
    }
}

int main() {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, control_function, NULL);
    curses_init();
    refresh();
    getmaxyx(stdscr,height,width);
    field_init();
    render();
    while (done == 0) {
        update();
        render();
        napms(DELAY);
        refresh();
    }
    endwin();
    return 0;
}