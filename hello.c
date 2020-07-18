#include <stdio.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define MAX_SNAKE_PARTS 200
#define MAX_PENDING_DIRECTIONS 10

const int NUM_COLS = 80;
const int NUM_ROWS = 50;

typedef struct {
  int cell_width;
  int cell_height;

  int screen_width;
  int screen_height;

  ALLEGRO_FONT* font;
  ALLEGRO_COLOR snake_color;
  ALLEGRO_COLOR apple_color;
} gfx_config;

typedef enum {
              DIRECTION_UP = 1,
              DIRECTION_DOWN,
              DIRECTION_LEFT,
              DIRECTION_RIGHT
} SNAKE_DIRECTION;

struct snake_part_struct;

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

typedef enum {
              STATE_MENU = 1,
              STATE_GAME
} GAME_STATE;

typedef struct game_state_struct {
  snake_part apple;
  snake_info snake;
} game_state;

typedef struct app_state_struct {
  GAME_STATE state;
  game_state game;
} app_state;

void draw_cell(int row, int col, gfx_config* cfg, ALLEGRO_COLOR color) {
  int y = row * cfg->cell_height;
  int x = col * cfg->cell_width;

  al_draw_filled_rectangle(x, y, x + cfg->cell_width, y + cfg->cell_height, color);
}

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

int handle_game_input(game_state* game, ALLEGRO_EVENT* event) {
  switch (event->keyboard.keycode) {
  case ALLEGRO_KEY_LEFT:
    push_direction_change(&game->snake, DIRECTION_LEFT);
    break;
  case ALLEGRO_KEY_RIGHT:
    push_direction_change(&game->snake, DIRECTION_RIGHT);
    break;
  case ALLEGRO_KEY_UP:
    push_direction_change(&game->snake, DIRECTION_UP);
    break;
  case ALLEGRO_KEY_DOWN:
    push_direction_change(&game->snake, DIRECTION_DOWN);
    break;
  }
}

void draw_game(game_state* game, gfx_config* g) {
  for (int i = 0; i < game->snake.tail_length; i++) {
    snake_part* current = game->snake.parts + i;
    draw_cell(current->row, current->col, g, g->snake_color);
  }

  draw_cell(game->apple.row, game->apple.col, g, g->apple_color);
}

void draw_menu(gfx_config* g) {
  const char* text = "This is the menu!";
  ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);

  int x = g->screen_width / 2.0f;
  int y = g->screen_height / 3.0f;
  al_draw_text(g->font, color, x, y, ALLEGRO_ALIGN_CENTRE, text);
}

int main()
{
    al_init();
    al_install_keyboard();
    if (!al_init_primitives_addon()) {
      printf("Primitives init failed\n");
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 10.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(320, 200);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    gfx_config g = {0};
    g.screen_width = al_get_display_width(disp);
    g.screen_height = al_get_display_height(disp);
    g.cell_width = al_get_display_width(disp) / NUM_COLS;
    g.cell_height = al_get_display_height(disp) / NUM_ROWS;
    g.font = font;
    g.snake_color = al_map_rgb(0, 255, 50);
    g.apple_color = al_map_rgb(255, 0, 50);

    // Init the app state
    app_state app = {0};
    app.state = STATE_GAME;

    app.game.snake.parts[0].row = 13;
    app.game.snake.parts[0].col = 15;
    app.game.snake.direction = DIRECTION_DOWN;
    app.game.snake.tail_length = 1;
    app.game.snake.pending_direction_count = 0;

    app.game.apple.row = 20;
    app.game.apple.col = 22;

    al_start_timer(timer);
    while(1) {
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER) {
          if (event.timer.source == timer) {
            if (app.state == STATE_GAME) {
              update_game(&app.game);
            } else if (app.state == STATE_MENU) {
              // TODO
            }
            redraw = true;
          } else {
            printf("Bad timer event source\n");
            break;
          }
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
          if (app.state == STATE_GAME) {
            handle_game_input(&app.game, &event);
          } else if (app.state == STATE_MENU) {
            // TODO
          }
          if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            break;
          }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
          break;
        }

        if(redraw && al_is_event_queue_empty(queue)) {
          al_clear_to_color(al_map_rgb(0, 0, 0));

          if (app.state == STATE_GAME) {
            draw_game(&app.game, &g);
          } else if (app.state == STATE_MENU) {
            draw_menu(&g);
          }
            
          al_flip_display();

          redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
