#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H

#define WIDTH 10
#define HEIGHT 20

#define DAS 0.17 // Delayed Auto Shift
#define ARR 0.05 // Auto Repeat Rate

typedef struct Game {
  int field[HEIGHT][WIDTH];
  int score;
  int is_game_over;
  int piece;
  int rot;
  int px;
  int py;
  double last_fall;
} Game;

typedef struct GameInput {
  int move_left;
  int move_right;
  int rotate;
  int soft_drop;
  int hard_drop;
  int restart;
} GameInput;

void game_update(Game *game, const GameInput *input, double now);
int has_tetris_piece(int piece, int rot, int x, int y);
int fits(const Game *game, int piece, int rot, int px, int py);
void lock_piece(Game *game);
int clear_lines(Game *game);
int game_landing_y(const Game *game);

#endif
