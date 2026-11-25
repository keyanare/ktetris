#include "render.h"
#include "tetris.h"
#include "raylib.h"

static const Color WINDOW_BACKGROUND = {17, 24, 39, 255};  // #111827
static const Color BOARD_BACKGROUND = {35, 50, 73, 255};   // #233249
static const Color EMPTY_CELL = {48, 67, 92, 255};         // #30435c
static const Color SCORE_TEXT = {199, 213, 232, 255};      // #C7D5E8
static const Color GAME_OVER_TEXT = {232, 154, 164, 255};  // #E89AA4
static const Color RESTART_TEXT = {158, 172, 193, 255};    // #9EACC1

static const Color COLORS[8] = {
  BLACK,
  {108, 184, 198, 255}, // I
  {174, 145, 207, 255}, // T
  {220, 191, 115, 255}, // O
  {207, 112, 126, 255}, // Z
  {128, 176, 136, 255}, // S
  {218, 154, 112, 255}, // L
  {110, 143, 205, 255}, // J
};

void draw_game(const Game *game, Font font) {
    ClearBackground(WINDOW_BACKGROUND);

    if (game->is_game_over) {
      DrawTextEx(font, "Game over!", (Vector2){.x = (WIDTH * CELL) / 2, .y = (HEIGHT * CELL - 32) / 2}, 16, 0, GAME_OVER_TEXT);
      DrawTextEx(font, "Press \"Enter\" to try again!", (Vector2){.x = (WIDTH * CELL - 260) / 2, .y = ((HEIGHT * CELL - 32) / 2) + 18}, 16, 0, RESTART_TEXT);
    } else {
      DrawRectangle(0, 0, WIDTH * CELL, HEIGHT * CELL, BOARD_BACKGROUND);

      for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++) {
          DrawRectangle(x * CELL, y * CELL, CELL - 1, CELL - 1,
                        game->field[y][x] ? COLORS[game->field[y][x]] : EMPTY_CELL);
        }

      int ghost_piece_y = game_landing_y(game);
      const Color ghost_color = Fade(COLORS[game->piece + 1], 0.3f);

      for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
          if (has_tetris_piece(game->piece, game->rot, x, y)) {
            DrawRectangle((game->px + x) * CELL, (game->py + y) * CELL, CELL - 1, CELL - 1,
                          COLORS[game->piece + 1]);
            DrawRectangle((game->px + x) * CELL, (ghost_piece_y + y) * CELL, CELL - 1, CELL - 1,
                          ghost_color);
          }
        
      DrawTextEx(font, TextFormat("Score:\n%d", game->score),
                (Vector2){.x = WIDTH * CELL + 24, .y = 20}, 16, 1, SCORE_TEXT);
    }
}