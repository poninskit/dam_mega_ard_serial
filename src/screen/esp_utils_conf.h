/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

// *INDENT-OFF*

// File version — must match ESP_UTILS_CONF_VERSION_* in esp_utils_versions.h
#define ESP_UTILS_CONF_FILE_VERSION_MAJOR 1
#define ESP_UTILS_CONF_FILE_VERSION_MINOR 2
#define ESP_UTILS_CONF_FILE_VERSION_PATCH 0

#define ESP_UTILS_CONF_CHECK_HANDLE_METHOD      (ESP_UTILS_CHECK_HANDLE_WITH_ERROR_LOG)
#define ESP_UTILS_CONF_LOG_LEVEL                (ESP_UTILS_LOG_LEVEL_INFO)

#if ESP_UTILS_CONF_LOG_LEVEL == ESP_UTILS_LOG_LEVEL_DEBUG
    #define ESP_UTILS_CONF_ENABLE_LOG_TRACE     (0)
#endif

// *INDENT-ON*
