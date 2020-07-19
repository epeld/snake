#include <stdio.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "snake.h"

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
              STATE_MENU = 1,
              STATE_GAME
} GAME_STATE;

typedef struct app_state_struct {
  GAME_STATE state;
  game_state game;
} app_state;

void draw_cell(int row, int col, gfx_config* cfg, ALLEGRO_COLOR color) {
  int y = row * cfg->cell_height;
  int x = col * cfg->cell_width;

  al_draw_filled_rectangle(x, y, x + cfg->cell_width, y + cfg->cell_height, color);
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

const char* get_count_down_text(double time_remaining) {
  if (time_remaining < 1.0) {
    return "Go!";
  }
  if (time_remaining < 2.0) {
    return "1";
  }
  if (time_remaining < 3.0) {
    return "2";
  }
  if (time_remaining < 4.0) {
    return "3";
  }
  return "Waiting..";
}

void draw_game(game_state* game, gfx_config* g, double current_time) {
  for (int i = 0; i < game->snake.tail_length; i++) {
    snake_part* current = game->snake.parts + i;
    draw_cell(current->row, current->col, g, g->snake_color);
  }

  draw_cell(game->apple.row, game->apple.col, g, g->apple_color);

  double count_down = calculate_count_down(game, current_time);
  if (-COUNT_DOWN_LINGER_TIME < count_down) {
    const char* text = get_count_down_text(count_down);
    ALLEGRO_COLOR color = al_map_rgb(255, 255, 0);
    int x = g->screen_width / 2.0f;
    int y = g->screen_height / 3.0f;
    al_draw_text(g->font, color, x, y, ALLEGRO_ALIGN_CENTRE, text);
  }
}

void draw_menu(gfx_config* g) {
  const char* text = "Press any key to play!";
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
    app.state = STATE_MENU;

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
              update_game(&app.game, al_get_time());
            } else if (app.state == STATE_MENU) {
              //
            }
            redraw = true;
          } else {
            printf("Bad timer event source\n");
            break;
          }
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
          if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            break;
          }
          if (app.state == STATE_GAME) {
            handle_game_input(&app.game, &event);
          } else if (app.state == STATE_MENU) {
            app.state = STATE_GAME;
            app.game.started = al_get_time();
          }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
          break;
        }

        if(redraw && al_is_event_queue_empty(queue)) {
          al_clear_to_color(al_map_rgb(0, 0, 0));

          if (app.state == STATE_GAME) {
            draw_game(&app.game, &g, al_get_time());
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
