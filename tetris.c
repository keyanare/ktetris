#include "tetris.h"
#include "raylib.h"
#include <string.h>

static const char *SHAPES[7] = {
    "..X...X...X...X.", // I
    "..X..XX...X.....", // T
    ".....XX..XX.....", // O
    "..X..XX..X......", // Z
    ".X...XX...X.....", // S
    ".X...X...XX.....", // L
    "..X...X..XX....."  // J
};

static int rot_index(int rot, int x, int y) {
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
    return SHAPES[piece][rot_index(rot, x, y)] == 'X';
}

int fits(const Game *game, int piece, int rot, int px, int py) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (!has_tetris_piece(piece, rot, x, y))
        continue;
      int fx = px + x, fy = py + y;
      if (fx < 0 || fx >= WIDTH || fy >= HEIGHT)
        return 0;
      if (fy >= 0 && game->field[fy][fx])
        return 0;
    }
  }
  return 1;
}

void lock_piece(Game *game) {
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      if (has_tetris_piece(game->piece, game->rot, x, y) && game->py + y >= 0)
        game->field[game->py + y][game->px + x] = game->piece + 1;
}

int clear_lines(Game *game) {
  int cleared = 0;
  for (int y = HEIGHT - 1; y >= 0; y--) {
    int full = 1;
    for (int x = 0; x < WIDTH; x++)
      if (!game->field[y][x]) {
        full = 0;
        break;
      }
    if (!full)
      continue;

    for (int yy = y; yy > 0; yy--)
      memcpy(game->field[yy], game->field[yy - 1], sizeof(game->field[yy]));
    memset(game->field[0], 0, sizeof(game->field[0]));
    cleared++;
    y++;
  }
  return cleared;
}

int game_landing_y(const Game *game) {
    int y = game->py;

    while(fits(game, game->piece, game->rot, game->px, y + 1)){
        y++;
    }

    return y;
}

void game_update(Game *game, const GameInput *input, double now) {
    if (game->is_game_over) {
      if (input->restart) {
        memset(game->field, 0, sizeof(game->field));
        game->score = 0;
        game->last_fall = now;
        game->is_game_over = 0;
      }
    } else {
      if (input->move_left && fits(game, game->piece, game->rot, game->px - 1, game->py)) {
        game->px--;
      }
      if (input->move_right && fits(game, game->piece, game->rot, game->px + 1, game->py)) {
        game->px++;
      }
      if (input->rotate && fits(game, game->piece, game->rot + 1, game->px, game->py)) {
        game->rot++;
      }

      const double fall_delay = 0.5;

      if (input->hard_drop) {
        game->py = game_landing_y(game);
      }

      double delay = input->soft_drop ? 0.05 : fall_delay;

      // gravity
      if (input->hard_drop || now - game->last_fall >= delay) {
        game->last_fall = now;
        if (fits(game, game->piece, game->rot, game->px, game->py + 1)) {
          game->py++;
        } else {
          lock_piece(game);
          game->score += clear_lines(game) * 100;

          game->piece = GetRandomValue(0, 6);
          game->rot = 0;
          game->px = WIDTH / 2 - 2;
          game->py = -1;

          if (!fits(game, game->piece, game->rot, game->px, game->py)) { // game over
            game->is_game_over = 1;
          }
        }
      }
    }
}