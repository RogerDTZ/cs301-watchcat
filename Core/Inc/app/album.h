#ifndef __ALBUM_H__
#define __ALBUM_H__

#include <stdint.h>

/**
 * @brief Set the current image's pixel at (x, y) to the given color.
 *
 * @param x     x coordinate    [0, 32)
 * @param y     y coordinate    [0, 32)
 * @param r     red             [0, 32)
 * @param g     green           [0, 64)
 * @param b     blue            [0, 32)
 */
void album_set_image_pixel(uint16_t x, uint16_t y, uint16_t r, uint16_t g,
                           uint16_t b);

/**
 * @brief Set the current image's pixel at (x, y) to the given color.
 *
 * @param x         x coordinate    [0, 32)
 * @param y         y coordinate    [0, 32)
 * @param rgb565    RGB565, R is the most significant 5 bits, G is the middle 6
 * bits, B is the least significant 5 bits.
 */
void album_set_image_pixel_raw(uint16_t x, uint16_t y, uint16_t rgb565);

/**
 * @brief Should be called to display the new image.
 */
void album_display_image();

void album_set_label(const char *str);

void open_app_album();

void close_app_album();

#endif /* __ALBUM_H__ */
