#include "LedStripManager.h"

uint32_t LedStripManager::add_color_offset(uint32_t color, int8_t offset)
{
  int16_t red16 = (color & 0x00FF0000) >> 16;
  int16_t green16 = (color & 0x0000FF00) >> 8;
  int16_t blue16 = (color & 0x000000FF);

  red16 += offset;
  green16 += offset;
  blue16 += offset;

  uint8_t red = red16 < 0 ? 0 : (red16 > 255 ? 255 : red16);
  uint8_t green = green16 < 0 ? 0 : (green16 > 255 ? 255 : green16);
  uint8_t blue = blue16 < 0 ? 0 : (blue16 > 255 ? 255 : blue16);

  return Adafruit_NeoPixel::Color(red, green, blue);
};

void LedStripManager::set_color(uint8_t red, uint8_t green, uint8_t blue) {
  color = Adafruit_NeoPixel::Color(red, green, blue);
  light_change_index = 0;
}

void LedStripManager::initialize() {
  led_strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  led_strip.show();            // Turn OFF all pixels ASAP
  led_strip.setBrightness(30); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void LedStripManager::set_pulse_step(uint8_t pulse_step) {
  this->pulse_step = pulse_step;
}

void LedStripManager::update() {
  bool changed_a_light = false;
  // Do permanent updates
  if (light_change_index < led_count)
  {
    led_strip.setPixelColor(light_change_index, color);
    light_change_index += 1;
    changed_a_light = true;
  } 

  // save base state
  uint8_t* pixel_data_ptr = led_strip.getPixels();
  uint8_t original_colors[4*led_count];
  for (uint8_t pi = 0; pi < 4*led_count; pi++){
    original_colors[pi] = pixel_data_ptr[pi];
  }

  // Apply pulse
  if (pulse_step > 0) {
    pulse_offset = pulse_offset + pulse_direction * pulse_step;

    if (pulse_offset >= pulse_length) {
      pulse_direction = -1;
      pulse_offset = pulse_length - 1;
    } else if (pulse_offset <= -1 * pulse_length) {
      pulse_direction = 1;
      pulse_offset = -1 * pulse_length + 1;
    }

    for (uint8_t i = 0; i < led_count; i++){
      led_strip.setPixelColor(i, add_color_offset(led_strip.getPixelColor(i), pulse_offset));
    }

    changed_a_light = true;
  }

  // TODO apply flicker if any

  // TODO apply wave

  if (changed_a_light) {
    led_strip.show();
    delay(10);
  }

  // restore base state
  for (uint8_t pi = 0; pi < 4*led_count; pi++){
    pixel_data_ptr[pi] = original_colors[pi];
  }
}