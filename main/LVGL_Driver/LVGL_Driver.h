#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "esp_lcd_types.h"
#include "lvgl.h"

#define LVGL_BUF_LEN  (EXAMPLE_LCD_H_RES * 20)

extern lv_disp_drv_t disp_drv; 

bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);

void LVGL_Init(void);                     // Call this function to initialize the screen (must be called in the main function) !!!!!