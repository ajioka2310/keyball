/*
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

#include QMK_KEYBOARD_H

#include "quantum.h"

// ==========================================
// 1. タップダンスの定義
// ==========================================
#ifdef TAP_DANCE_ENABLE
typedef struct {
  bool is_press_action;
  int state;
} tap;

enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD,
  DOUBLE_TAP,
  SINGLE_TAP_HOLD,
  DOUBLE_SINGLE_TAP,
};

enum {
 X_TAP_DANCE_1 = 0,
 X_TAP_DANCE_2,
};

int cur_dance (tap_dance_state_t *state);

void x_finished (tap_dance_state_t *state, void *user_data);
void x_reset (tap_dance_state_t *state, void *user_data);

#define TAP_1 TD(X_TAP_DANCE_1)
#define TAP_2 TD(X_TAP_DANCE_2)
#endif

/* Tap danceの設定 */
#ifdef TAP_DANCE_ENABLE
int cur_dance (tap_dance_state_t *state) {
  if (state->count ==1) {
    if (!state->pressed) return SINGLE_TAP;
    else return SINGLE_HOLD;
  }
  else if (state->count == 2) {
    if (state->interrupted) return DOUBLE_SINGLE_TAP;
    else if (state->pressed) return SINGLE_TAP_HOLD;
    else return DOUBLE_TAP;
  }
  else return 8; //magic number. At some point this method will expand to work for more presses
}

static tap xtap_state = {
  .is_press_action = true,
  .state = 0
};

/* X_TAP_DANCE_1の定義 */
void x_finished_1 (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  switch (xtap_state.state) {
    case SINGLE_TAP:
        register_code(KC_Y);
        break;
    case SINGLE_HOLD:
        layer_on(0);
        break;
    case DOUBLE_TAP:
        register_code(LALT(KC_GRV));
        break;
    case SINGLE_TAP_HOLD:
        layer_on(0);
        break;
  }
}

void x_reset_1 (tap_dance_state_t *state, void *user_data) {
  switch (xtap_state.state) {
    case SINGLE_TAP: 
        unregister_code(KC_Y);
        break;
    case SINGLE_HOLD:
        layer_off(0);
        break;
    case DOUBLE_TAP:
        unregister_code(LALT(KC_GRV));
        break;
    case SINGLE_TAP_HOLD:
        layer_off(0);
        break;
  }
  xtap_state.state = 0;
}

/* X_TAP_DANCE_2の定義 */
void x_finished_2 (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  switch (xtap_state.state) {
    case SINGLE_TAP:
        register_code(KC_MUTE);
        break;
    case SINGLE_HOLD:
        layer_on(0);
        break;
    case DOUBLE_TAP:
        layer_invert(0);
        break;
    case SINGLE_TAP_HOLD:
        layer_on(0);
        break;
  }
}

void x_reset_2 (tap_dance_state_t *state, void *user_data) {
  switch (xtap_state.state) {
    case SINGLE_TAP: 
        unregister_code(KC_MUTE);
        break;
    case SINGLE_HOLD:
        layer_off(0);
        break;
    case DOUBLE_TAP:
        break;
    case SINGLE_TAP_HOLD:
        layer_off(0);
        break;
  }
  xtap_state.state = 0;
}


tap_dance_action_t tap_dance_actions[] = {
 [X_TAP_DANCE_1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished_1, x_reset_1),
 [X_TAP_DANCE_2] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished_2, x_reset_2),
};
#endif


// ==========================================
// 2. キーマップの配置（TAP_0 の定義より後ろに置く）
// ==========================================
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default (VIA)
  [0] = LAYOUT_universal(
    KC_TAB   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        TAP_1     , KC_U     , KC_I     , KC_O     , KC_P     , KC_BSPC  ,
    KC_LCTL  , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                        KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , KC_ENT  ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , MO(4)  ,
              MO(4),KC_LGUI,  KC_LALT     ,KC_BTN1,   KC_SPC,                 MO(1),MO(2), RCTL_T(KC_LNG2),     KC_RALT  , MO(3)
  ),

  [1] = LAYOUT_universal(
    _______  ,  KC_F1   , KC_F2    , KC_F3   , KC_F4    , KC_F5    ,                                         KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   , KC_F11   ,
    _______  ,  _______ , _______  , KC_UP   , KC_ENT   , KC_DEL   ,                                         KC_LEFT  , KC_DOWN  , KC_UP    , KC_RIGHT , KC_BTN3  , KC_F12   ,
    _______  ,  _______ , KC_LEFT  , KC_DOWN , KC_BTN2  , KC_BSPC  ,                                         KC_HOME  , KC_PGUP  , KC_PGDN  , KC_END   , _______  , _______  ,
                  _______  , _______ , _______  ,         _______  , _______  ,                   _______  , _______  , _______       , _______  , _______
  ),

  [2] = LAYOUT_universal(
    _______  , KC_1      , KC_2     , KC_3    , KC_4     , KC_5    ,                                           KC_6     , KC_7    , KC_8     , KC_9     , KC_0     , _______ ,
    _______  , S(KC_1)   , S(KC_2)  , S(KC_3) , S(KC_4)  , S(KC_5) ,                                         S(KC_6)  , S(KC_7)   , S(KC_8)  , S(KC_9)  , S(KC_0)  , _______ ,
    _______  , S(KC_MINS), KC_1     , KC_2    , KC_3     ,S(KC_RBRC),                                        KC_LBRC  , KC_RBRC   , KC_EQL   , KC_COMM  , KC_GRV   , KC_BSLS ,
                  _______  , _______ , _______  ,         _______  , _______  ,                   _______  , _______  , _______       , _______  , _______
  ),

  [3] = LAYOUT_universal(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , _______  , _______  ,                                        RGB_M_P  , RGB_M_B  , RGB_M_R  , RGB_M_SW , RGB_M_SN , RGB_M_K  ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , _______  , SCRL_DVI ,                                        RGB_M_X  , RGB_M_G  , RGB_M_T  , RGB_M_TW , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , _______  , SCRL_DVD ,                                        CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , _______  , KBC_SAVE ,
                  QK_BOOT  , KBC_RST  , _______  ,        _______  , _______  ,                   _______  , _______  , _______       , KBC_RST  , QK_BOOT
  ),

  [4] = LAYOUT_universal(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , _______  , _______  ,                                        RGB_M_P  , RGB_M_B  , RGB_M_R  , RGB_M_SW , RGB_M_SN , RGB_M_K  ,
    RGB_MOD  , AML_TO  , RGB_SAI  , RGB_VAI  , _______  , SCRL_DVI ,                                        RGB_M_X  , RGB_M_G  , RGB_M_T  , RGB_M_TW , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , _______  , SCRL_DVD ,                                        CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , _______  , KBC_SAVE ,
                  QK_BOOT  , KBC_RST  , _______  ,        _______  , _______  ,                   _______  , _______  , _______       , KBC_RST  , QK_BOOT
  ),
};

// ==========================================
// 3. レイヤー変更時のスクロールモード制御
// ==========================================
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t highest_layer = get_highest_layer(state);
    
    // レイヤー4（横スクロール用）または レイヤー3（縦スクロール用）のときにスクロールを有効化
    if (highest_layer == 4 || highest_layer == 3) {
        keyball_set_scroll_mode(true);
    } else {
        keyball_set_scroll_mode(false);
    }
    return state;
}

// ==========================================
// 4. トラックボールの挙動カスタム（レイヤー2を横スクロール化）
// ==========================================
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // 現在のレイヤーが 4 のときだけ処理
    if (get_highest_layer(layer_state) == 4) {
        // トラックボールの縦スクロールの動きを、横スクロールにコンバートする
        if (mouse_report.v != 0) {
            mouse_report.h = mouse_report.v; // 縦の移動量を横に移植
            mouse_report.v = 0;              // 縦の動きを殺す
        }
    }
    return mouse_report;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif

#ifdef COMBO_ENABLE
const uint16_t PROGMEM my_jk[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM my_gh[] = {KC_G, KC_H, COMBO_END};
const uint16_t PROGMEM my_bn[] = {KC_B, KC_N, COMBO_END};
const uint16_t PROGMEM my_tabq[] = {KC_TAB, KC_Q, COMBO_END};
const uint16_t PROGMEM my_bacp[] = {KC_BSPC, KC_P, COMBO_END};
const uint16_t PROGMEM my_spcmo1[] = {KC_SPC, MO(1), COMBO_END};
const uint16_t PROGMEM my_qw[] = {KC_Q, KC_W, COMBO_END};
const uint16_t PROGMEM my_sd[] = {KC_S, KC_E, COMBO_END};
const uint16_t PROGMEM my_we[] = {KC_S, KC_D, COMBO_END};




combo_t key_combos[] = {  
COMBO(my_jk, KC_BTN1), // クリック
COMBO(my_gh, KC_EQL), // = +
COMBO(my_bn, KC_MINS), // ー _
COMBO(my_tabq, KC_ESC), // エスケープ
COMBO(my_bacp, KC_DEL), // デリート
COMBO(my_spcmo1, KC_QUOT), // '  "
COMBO(my_qw, LCA(KC_PAUSE)), // Ctrl + Alt + Pause/Break
COMBO(my_sd, KC_ENT), // エンター
COMBO(my_we, KC_DEL), // デリート



};
#endif

