#include <stdio.h>
#include <stdlib.h>

#include "snake.h"

void randomize_position(snake_part* part) {
  part->row = rand() % NUM_ROWS;
  part->col = rand() % NUM_COLS;
}

void push_direction_change(snake_info* snake, SNAKE_DIRECTION direction) {
  if (snake->pending_direction_count < MAX_PENDING_DIRECTIONS) {
    snake->pending_direction_changes[snake->pending_direction_count] = direction;
    snake->pending_direction_count++;
  } else {
    fprintf(stderr, "Overflow in buffer for pending direction changes. Disregarding commands\n");
  }
}

void grow_snake(snake_info* snake) {
  if (snake->tail_length + 1 >= MAX_SNAKE_PARTS) {
    fprintf(stderr, "max snake size reached\n");
    return;
  }
  snake->tail_length++;
  snake_part* end = snake->parts + snake->tail_length;
  snake_part* prev_end = snake->parts + snake->tail_length - 1;
  end->row = prev_end->row;
  end->col = prev_end->col;
}

void update_snake(snake_info* snake) {
  // Handle direction changes
  if (snake->pending_direction_count) {
    SNAKE_DIRECTION change = snake->pending_direction_changes[0];
    switch (change) {
    case DIRECTION_LEFT:
      if (snake->direction != DIRECTION_RIGHT) {
        snake->direction = DIRECTION_LEFT;
      }
      break;
    case DIRECTION_RIGHT:
      if (snake->direction != DIRECTION_LEFT) {
        snake->direction = DIRECTION_RIGHT;
      }
      break;
    case DIRECTION_UP:
      if (snake->direction != DIRECTION_DOWN) {
        snake->direction = DIRECTION_UP;
      }
      break;
    case DIRECTION_DOWN:
      if (snake->direction != DIRECTION_UP) {
        snake->direction = DIRECTION_DOWN;
      }
      break;
    }

    // Rearrange all pending items to simulate FIFO
    for (int i = 0; i < snake->pending_direction_count - 1; i++) {
      snake->pending_direction_changes[i] = snake->pending_direction_changes[i + 1];
    }
    snake->pending_direction_count--;
  }
  
  // Update snake body parts
  for (int i = snake->tail_length - 1; 0 < i; i--) {
    snake_part* prev = snake->parts + i - 1;
    snake_part* current = snake->parts + i;

    current->row = prev->row;
    current->col = prev->col;
  }

  // Update snake parts[0]!
  switch (snake->direction) {
  case DIRECTION_UP:
    snake->parts[0].row--;
    break;
  case DIRECTION_DOWN:
    snake->parts[0].row++;
    break;
  case DIRECTION_LEFT:
    snake->parts[0].col--;
    break;
  case DIRECTION_RIGHT:
    snake->parts[0].col++;
    break;
  }
}

void update_game(game_state* game) {
  if (game->apple.row == game->snake.parts[0].row && game->apple.col == game->snake.parts[0].col) {
    randomize_position(&game->apple);
    grow_snake(&game->snake);
  }
  update_snake(&game->snake);
}
