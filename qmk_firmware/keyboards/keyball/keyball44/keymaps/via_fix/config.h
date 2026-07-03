/*
This is the c configuration file for the keymap

Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef RGBLIGHT_ENABLE
//#    define RGBLIGHT_EFFECT_BREATHING
//#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
//#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
//#    define RGBLIGHT_EFFECT_SNAKE
//#    define RGBLIGHT_EFFECT_KNIGHT
//#    define RGBLIGHT_EFFECT_CHRISTMAS
//#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
//#    define RGBLIGHT_EFFECT_RGB_TEST
//#    define RGBLIGHT_EFFECT_ALTERNATING
//#    define RGBLIGHT_EFFECT_TWINKLE
#endif

#undef RGB_MATRIX_ENABLE // RGB LEDのアニメーション機能を削って容量確保

#define TAP_CODE_DELAY 5

// #define POINTING_DEVICE_AUTO_MOUSE_ENABLE
// #define AUTO_MOUSE_DEFAULT_LAYER 1

#define TAPPING_TERM 180

#define DYNAMIC_KEYMAP_LAYER_COUNT 5

#pragma once

// ==========================================
// キーによるホイールスクロールの感度調整
// ==========================================
#define MOUSEKEY_WHEEL_DELAY 0         // キーを押してからスクロールが始まるまでの遅延（0で即反応）
#define MOUSEKEY_WHEEL_INTERVAL 80     // スクロール信号を送る間隔（ミリ秒）。数値を小さくすると高速化します（デフォルト: 100前後）
#define MOUSEKEY_WHEEL_MAX_SPEED 1     // キーを押し続けたときの最大速度。数値を大きくすると1回あたりのスクロール量が増えます
#define MOUSEKEY_WHEEL_TIME_TO_MAX 20  // 最大速度（MAX_SPEED）に達するまでのキーの連打数（または押し続け時間）

// ==========================================
// トラックボールによるスクロールの速度調整（減速）
// ==========================================
#define KEYBALL_SCROLL_DIVIDER 8  

// VIA（Vial）のコンソールやメニューからCPIを調整できるようにする設定
#define POINTING_DEVICE_TASK_THROTTLE_MS 1
#define PMW3360_CPI 600