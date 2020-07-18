#pragma once

#define MAX_SNAKE_PARTS 200
#define MAX_PENDING_DIRECTIONS 10

#define NUM_COLS 80
#define NUM_ROWS 50

typedef enum {
              DIRECTION_UP = 1,
              DIRECTION_DOWN,
              DIRECTION_LEFT,
              DIRECTION_RIGHT
} SNAKE_DIRECTION;

typedef struct snake_part_struct {
  int row, col;
} snake_part;

typedef struct {
  int tail_length;
  snake_part parts[MAX_SNAKE_PARTS];
  SNAKE_DIRECTION direction;
  SNAKE_DIRECTION pending_direction_changes[MAX_PENDING_DIRECTIONS];
  int pending_direction_count;
} snake_info;


typedef struct game_state_struct {
  snake_part apple;
  snake_info snake;
} game_state;

void push_direction_change(snake_info* snake, SNAKE_DIRECTION direction);
void update_game(game_state* game);
