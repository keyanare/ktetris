#include "tetris.h"

static const char *SHAPES[7] = {
    "..X...X...X...X.", // I
    "..X..XX...X.....", // T
    ".....XX..XX.....", // O
    "..X..XX..X......", // Z
    ".X...XX...X.....", // S
    ".X...X...XX.....", // L
    "..X...X..XX....."  // J
};

static int rot_index(int x, int y, int rot) {
  switch (rot % 4) {
  case 0:
    return y * 4 + x;
  case 1:
    return 12 + y - x * 4;
  case 2:
    return 15 - y * 4 - x;
  default:
    return 3 - y + x * 4;
  }
}

int has_tetris_piece(int piece, int rot, int x, int y) {
    return SHAPES[piece][rot_index(x, y, rot)] == 'X';
}