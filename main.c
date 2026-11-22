#include "raylib.h"
#include "tetris.h"
#include <stdint.h>
#include <string.h>
#include <time.h>

static int field[HEIGHT][WIDTH];
static int score = 0;
static int is_game_over = 0;

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

int fits(int p, int r, int px, int py) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (!has_tetris_piece(p, r, x, y))
        continue;
      int fx = px + x, fy = py + y;
      if (fx < 0 || fx >= WIDTH || fy >= HEIGHT)
        return 0;
      if (fy >= 0 && field[fy][fx])
        return 0;
    }
  }
  return 1;
}

static void lock_piece(int p, int r, int px, int py) {
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      if (has_tetris_piece(p, r, x, y) && py + y >= 0)
        field[py + y][px + x] = p + 1;
}

static int clear_lines(void) {
  int cleared = 0;
  for (int y = HEIGHT - 1; y >= 0; y--) {
    int full = 1;
    for (int x = 0; x < WIDTH; x++)
      if (!field[y][x]) {
        full = 0;
        break;
      }
    if (!full)
      continue;

    for (int yy = y; yy > 0; yy--)
      memcpy(field[yy], field[yy - 1], sizeof(field[yy]));
    memset(field[0], 0, sizeof(field[0]));
    cleared++;
    y++;
  }
  return cleared;
}

static int key_repeat(int key, double* next) {
  const double now = GetTime();

  if (IsKeyPressed(key)) {
    *next = now + DAS;
    return 1;
  }

  if (!IsKeyDown(key)) {
    *next = now + DAS;
    return 0;
  }

  if (now >= *next) {
    *next += ARR;

    if (now - *next > ARR) {
      *next = now + ARR;
    }
    return 1;
  }
  return 0;
}

int main(void) {
#ifdef _WIN32
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
#endif
  InitWindow(WIDTH * CELL + 160, HEIGHT * CELL, "kTetris");
  SetTargetFPS(60);

  unsigned seed = (unsigned)time(NULL) ^ (unsigned)clock();

  TraceLog(LOG_INFO, "RANDOM SEED: %u", seed);
  SetRandomSeed(seed);

  const Font font = LoadFontEx("./resources/font.ttf", 16, NULL, 0);
  SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);  

  int piece = GetRandomValue(0, 6), rot = 0, px = WIDTH / 2 - 2, py = -1;
  double last_fall = GetTime();

  double next_left = 0, next_right = 0;

  while (!WindowShouldClose()) {
    if (is_game_over) {
      if (IsKeyPressed(KEY_ENTER)) {
        memset(field, 0, sizeof(field));
        score = 0;
        last_fall = GetTime();
        is_game_over = 0;
      }
    } else {
      const double fall_delay = 0.5;
      
      if (key_repeat(KEY_LEFT, &next_left) && fits(piece, rot, px - 1, py)) {
        px--;
      }
      if (key_repeat(KEY_RIGHT, &next_right) && fits(piece, rot, px + 1, py)) {
        px++;
      }
      if (IsKeyPressed(KEY_UP) && fits(piece, rot + 1, px, py)) {
        rot++;  
      }

      int hard_drop = IsKeyPressed(KEY_SPACE) && fits(piece, rot, px, py);

      if (hard_drop) {
        while (fits(piece, rot, px, py + 1))
          py++;
      }

      double delay = IsKeyDown(KEY_DOWN) ? 0.05 : fall_delay;
      double now = GetTime();

      // gravity
      if (hard_drop || now - last_fall >= delay) {
        last_fall = now;
        if (fits(piece, rot, px, py + 1)) {
          py++;
        } else {
          lock_piece(piece, rot, px, py);
          score += clear_lines() * 100;

          piece = GetRandomValue(0, 6);
          rot = 0;
          px = WIDTH / 2 - 2;
          py = -1;

          if (!fits(piece, rot, px, py)) { // game over
            is_game_over = 1;
          }
        }
      }
    }
    // drawing
    BeginDrawing();
    ClearBackground(WINDOW_BACKGROUND);

    if (is_game_over) {
      DrawTextEx(font, "Game over!", (Vector2){.x = (WIDTH * CELL) / 2, .y = (HEIGHT * CELL - 32) / 2}, 16, 0, GAME_OVER_TEXT);
      DrawTextEx(font, "Press \"Enter\" to try again!", (Vector2){.x = (WIDTH * CELL - 260) / 2, .y = ((HEIGHT * CELL - 32) / 2) + 18}, 16, 0, RESTART_TEXT);
    } else {
      DrawRectangle(0, 0, WIDTH * CELL, HEIGHT * CELL, BOARD_BACKGROUND);

      for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++) {
          DrawRectangle(x * CELL, y * CELL, CELL - 1, CELL - 1,
                        field[y][x] ? COLORS[field[y][x]] : EMPTY_CELL);
        }

      int ghost_piece_y = py;
      while(fits(piece, rot, px, ghost_piece_y + 1)) {
        ghost_piece_y++;
      }
      const Color ghost_color = Fade(COLORS[piece + 1], 0.3f);

      for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
          if (has_tetris_piece(piece, rot, x, y)) {
            DrawRectangle((px + x) * CELL, (py + y) * CELL, CELL - 1, CELL - 1,
                          COLORS[piece + 1]);
            DrawRectangle((px + x) * CELL, (ghost_piece_y + y) * CELL, CELL - 1, CELL - 1,
                          ghost_color);
          }
        
      DrawTextEx(font, TextFormat("Score:\n%d", score),
                (Vector2){.x = WIDTH * CELL + 24, .y = 20}, 16, 1, SCORE_TEXT);
    }

    EndDrawing();
  }
  CloseWindow();
  return 0;
}
