// Standard includes
#include "pebble.h"

// App-specific data
Window *window; // All apps must have at least one window
TextLayer *time_layer; // The clock
TextLayer *name_layer;


static GBitmap *foreground_image;
static GBitmap *background_image;
static BitmapLayer *foreground_image_layer;
static BitmapLayer *background_image_layer;

static GFont font; 

// Called once per second
static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00"; // Needs to be static because it's used by the system later.

  if(clock_is_24h_style())
    strftime(time_text, sizeof(time_text), "%R", tick_time);
  else
    strftime(time_text, sizeof(time_text), "%I:%M", tick_time);
  
  layer_set_hidden(bitmap_layer_get_layer(background_image_layer), (tick_time->tm_sec % 2) ? true : false);

  text_layer_set_text(time_layer, time_text);
}


// Handle the start-up of the app
static void do_init(void) {

  // Create our app's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MAIN_18)); 

  // Init the text layer used to show the time
  time_layer = text_layer_create(GRect(0, 120, 144 /* width */, 168-120 /* height */));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));


  // Init the text layer used to show the company name
  name_layer = text_layer_create(GRect(0, 90, 144 /* width */, 168-85 /* height */));
  text_layer_set_text_color(name_layer, GColorWhite);
  text_layer_set_background_color(name_layer, GColorClear);
  text_layer_set_text_alignment(name_layer, GTextAlignmentCenter);
  text_layer_set_font(name_layer, font);
  text_layer_set_text(name_layer, "Interactive Intelligence");



  foreground_image = gbitmap_create_with_resource(RESOURCE_ID_LOGO_FORE);
  background_image = gbitmap_create_with_resource(RESOURCE_ID_LOGO_BACK);

  GRect image_frame = GRect(0, 0, 144, 100); 

  // Use GCompOpClear to display the black portions of the image
  background_image_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(background_image_layer, background_image);
  //bitmap_layer_set_compositing_mode(background_image_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_image_layer));


  // Use GCompOpOr to display the white portions of the image
  foreground_image_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(foreground_image_layer, foreground_image);
  bitmap_layer_set_compositing_mode(foreground_image_layer, GCompOpOr);
  layer_add_child(window_layer, bitmap_layer_get_layer(foreground_image_layer));


  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(name_layer));
}

static void do_deinit(void) {
  text_layer_destroy(time_layer);
  text_layer_destroy(name_layer);
  window_destroy(window);
  bitmap_layer_destroy(foreground_image_layer);
  bitmap_layer_destroy(background_image_layer);
  gbitmap_destroy(foreground_image);
  gbitmap_destroy(background_image);
  fonts_unload_custom_font(font); 
}

// The main event/run loop for our app
int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
