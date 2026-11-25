#include "raylib.h"
#include "render.h"
#include "tetris.h"
#include <stdint.h>
#include <string.h>
#include <time.h>

static int key_repeat(int key, double *next) {
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

GameInput read_input(void) {
  int move_left, move_right, rotate, soft_drop, hard_drop, restart;
  static double next_left = 0, next_right = 0;

  move_left = key_repeat(KEY_LEFT, &next_left);
  move_right = key_repeat(KEY_RIGHT, &next_right);
  rotate = IsKeyPressed(KEY_UP);
  soft_drop = IsKeyDown(KEY_DOWN);
  hard_drop = IsKeyPressed(KEY_SPACE);
  restart = IsKeyPressed(KEY_ENTER);

  return (GameInput){.move_left = move_left,
                     .move_right = move_right,
                     .rotate = rotate,
                     .soft_drop = soft_drop,
                     .hard_drop = hard_drop,
                     .restart = restart};
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

  Game game = {
      .piece = GetRandomValue(0, 6),
      .px = WIDTH / 2 - 2,
      .py = -1,
      .last_fall = GetTime(),
      .score = 0,
      .is_game_over = 0,
  };

  while (!WindowShouldClose()) {
    GameInput input = read_input();
    game_update(&game, &input, GetTime());

    BeginDrawing();
    draw_game(&game, font);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
