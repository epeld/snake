#include <stdio.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define MAX_SNAKE_PARTS 200

const int NUM_COLS = 80;
const int NUM_ROWS = 50;

typedef struct {
  int cell_width;
  int cell_height;

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
} snake_info;

void draw_cell(int row, int col, gfx_config* cfg, ALLEGRO_COLOR color) {
  int y = row * cfg->cell_height;
  int x = col * cfg->cell_width;

  al_draw_filled_rectangle(x, y, x + cfg->cell_width, y + cfg->cell_height, color);
}

void randomize_position(snake_part* part) {
  part->row = rand() % NUM_ROWS;
  part->col = rand() % NUM_COLS;
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
    g.cell_width = al_get_display_width(disp) / NUM_COLS;
    g.cell_height = al_get_display_height(disp) / NUM_ROWS;
    g.snake_color = al_map_rgb(0, 255, 50);
    g.apple_color = al_map_rgb(255, 0, 50);

    snake_info snake = {0};
    snake.parts[0].row = 13;
    snake.parts[0].col = 15;
    snake.direction = DIRECTION_DOWN;
    snake.tail_length = 1;

    snake_part apple = {0};
    apple.row = 20;
    apple.col = 22;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER) {
          if (event.timer.source == timer) {
            if (apple.row == snake.parts[0].row && apple.col == snake.parts[0].col) {
              randomize_position(&apple);
              grow_snake(&snake);
            }
            update_snake(&snake);
            redraw = true;
          } else {
            printf("Bad timer event source\n");
            break;
          }
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
          int stop = 0;
          switch (event.keyboard.keycode) {
          case ALLEGRO_KEY_LEFT:
            if (snake.direction != DIRECTION_RIGHT) {
              snake.direction = DIRECTION_LEFT;
            }
            break;
          case ALLEGRO_KEY_RIGHT:
            if (snake.direction != DIRECTION_LEFT) {
              snake.direction = DIRECTION_RIGHT;
            }
            break;
          case ALLEGRO_KEY_UP:
            if (snake.direction != DIRECTION_DOWN) {
              snake.direction = DIRECTION_UP;
            }
            break;
          case ALLEGRO_KEY_DOWN:
            if (snake.direction != DIRECTION_UP) {
              snake.direction = DIRECTION_DOWN;
            }
            break;
          case ALLEGRO_KEY_ESCAPE:
            stop = 1;
            break;
          }
          if (stop) {
            break;
          }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
          break;
        }

        if(redraw && al_is_event_queue_empty(queue)) {
          al_clear_to_color(al_map_rgb(0, 0, 0));
          // al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");

          for (int i = 0; i < snake.tail_length; i++) {
            snake_part* current = snake.parts + i;
            draw_cell(current->row, current->col, &g, g.snake_color);
          }

          draw_cell(apple.row, apple.col, &g, g.apple_color);
            
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
