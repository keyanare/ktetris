#ifndef TETRIS_RENDER_H
#define TETRIS_RENDER_H

#define CELL 30

#include "raylib.h"
#include "tetris.h"

void draw_game(const Game *game, Font font);

#endif