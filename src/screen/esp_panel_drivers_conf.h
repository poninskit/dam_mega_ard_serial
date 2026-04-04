/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

// *INDENT-OFF*

// File version — must match what the library expects (see esp_panel_versions.h)
#define ESP_PANEL_DRIVERS_CONF_FILE_VERSION_MAJOR 1
#define ESP_PANEL_DRIVERS_CONF_FILE_VERSION_MINOR 2
#define ESP_PANEL_DRIVERS_CONF_FILE_VERSION_PATCH 0

// Enable all bus and panel drivers (simplest, larger binary but always works)
#define ESP_PANEL_DRIVERS_BUS_USE_ALL                   (1)
#define ESP_PANEL_DRIVERS_LCD_USE_ALL                   (1)
#define ESP_PANEL_DRIVERS_TOUCH_USE_ALL                 (1)

// *INDENT-ON*
