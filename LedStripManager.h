#ifndef ___LED_STRIP_MANAGER_H__
#define ___LED_STRIP_MANAGER_H__

#include <Adafruit_NeoPixel.h>

class LedStripManager {

  public:
    LedStripManager(Adafruit_NeoPixel& led_strip) : led_strip(led_strip) {
      this->led_count = led_strip.numPixels();
      this->set_color(10, 50, 100);
      this->light_change_index = 0;
    }

    void initialize();
    void update();
    void set_pulse_step(uint8_t pulse_length); // 0 for no pulse
    void set_color(uint8_t red, uint8_t green, uint8_t blue);
    uint32_t add_color_offset(uint32_t color, int8_t offset);

    Adafruit_NeoPixel& led_strip;
    uint32_t color;
    uint8_t led_count;
    uint8_t light_change_index; // index of next pixel to be changed during color change
    uint8_t pulse_length = 60; // +/- range for pulses
    int8_t pulse_step = 0;
    int8_t pulse_offset = 0;
    int8_t pulse_direction = 1;
};

#endif