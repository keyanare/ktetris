#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H

#define WIDTH  10
#define HEIGHT 20
#define CELL   30

#define DAS 0.17 // Delayed Auto Shift
#define ARR 0.05 // Auto Repeat Rate

int has_tetris_piece(int piece, int rot, int x, int y);
int fits(int piece, int rot, int px, int py);

#endif

