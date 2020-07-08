#include <stdio.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

typedef struct {
  int cell_width;
  int cell_height;

  ALLEGRO_COLOR snake_color;
} gfx_config;

void draw_cell(int row, int col, gfx_config* cfg) {
  int y = row * cfg->cell_height;
  int x = col * cfg->cell_width;

  al_draw_filled_rectangle(x, y, x + cfg->cell_width, y + cfg->cell_height, cfg->snake_color);
}

int main()
{
    al_init();
    al_install_keyboard();
    if (!al_init_primitives_addon()) {
      printf("Primitives init failed\n");
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(320, 200);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    const int NUM_COLS = 50;
    const int NUM_ROWS = 50;

    gfx_config g = {0};
    g.cell_width = al_get_display_width(disp) / NUM_COLS;
    g.cell_height = al_get_display_height(disp) / NUM_ROWS;
    g.snake_color = al_map_rgb(0, 255, 50);

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        else if((event.type == ALLEGRO_EVENT_KEY_DOWN) || (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");

            draw_cell(13, 5, &g);
            draw_cell(13, 7, &g);
            
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
