#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont *s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // get tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // create long-lived buffer
  static char buffer[] = "00:00";

  // write time into the buffer
  if (clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // display on text layer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  // setup bg image
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FISH);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // setup time text layer
  s_time_layer = text_layer_create(GRect(5, 82, 139, 70));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);

  // setup custom font
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOURNAL_64));

  // improve layout
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // add it to window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

  // display time from start
  update_time();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_time_font);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits time_changed) {
  update_time();
}

static void init() {
  // create and assign main window element
  s_main_window = window_create();

  // set handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // show the window on the watch with an animation
  window_stack_push(s_main_window, true);

  // register tick handler
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
