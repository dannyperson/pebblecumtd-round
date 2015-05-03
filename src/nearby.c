#include <pebble.h>
#include "nearby.h"
#include "main.h"
#include "favorites.h"
#include "stops.h"
  
static Departure deps[5];
static char * cur_stop;
static Window* window;
static MenuLayer *menu_layer;
static int num_stops = 5;


void nearby_draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
  menu_cell_basic_draw(ctx, cell_layer, deps[cell_index->row].headsign, deps[cell_index->row].expected_time, NULL);
}

static void nearby_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "                CUMTD");
}

uint16_t nearby_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context) {
  return 1;
}

int16_t nearby_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return 16;
}

uint16_t nearby_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
  return num_stops;
}
 
void nearby_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  send_stop(cur_stop, 1, window);
}

void nearby_window_load(Window *window)
{
  //Create it - 12 is approx height of the top bar
  menu_layer = menu_layer_create(GRect(0, 0, 144, 168 - 16));

  //Let it receive clicks
  menu_layer_set_click_config_onto_window(menu_layer, window);

  //Give it its callbacks
  MenuLayerCallbacks callbacks = {
    .draw_header = (MenuLayerDrawHeaderCallback) nearby_menu_draw_header_callback,
    .draw_row = (MenuLayerDrawRowCallback) nearby_draw_row_callback,
    .get_num_sections = (MenuLayerGetNumberOfSectionsCallback) nearby_num_sections_callback,
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) nearby_num_rows_callback,
    .get_header_height = (MenuLayerGetHeaderHeightCallback) nearby_header_height_callback,
    .select_click = (MenuLayerSelectCallback) nearby_select_click_callback
  };
  menu_layer_set_callbacks(menu_layer, NULL, callbacks);

  //Add to Window
  layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));

}
 
void nearby_window_unload(Window *window)
{
  menu_layer_destroy(menu_layer);

}

void nearby_init(Departure departs[], int departure_num, char * stop)
{

  APP_LOG(APP_LOG_LEVEL_DEBUG, "START COPYING OVER DEPARTURES");
    for (int i = 0; i < departure_num; i++) {
      strcpy(deps[i].headsign, departs[i].headsign);
      strcpy(deps[i].expected_time, departs[i].expected_time);
      
      APP_LOG(APP_LOG_LEVEL_DEBUG, "SIGN: %s", deps[i].headsign);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "TIME: %s", deps[i].expected_time);
    }
    cur_stop = malloc(sizeof(char) * (strlen(stop) + 1));
    strcpy(cur_stop, stop);
    num_stops = departure_num;    
  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "start init");
    window = window_create();
    WindowHandlers handlers = {
        .load = nearby_window_load,
        .unload = nearby_window_unload
    };

    window_set_window_handlers(window, (WindowHandlers) handlers);
    window_stack_push(window, true);
}
 
void nearby_deinit()
{
    window_destroy(window);
}

