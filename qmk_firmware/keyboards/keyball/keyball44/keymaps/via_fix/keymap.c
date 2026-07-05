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
        register_code(KC_LCTL);
        break;
    case SINGLE_HOLD:
        register_code(KC_LCTL);
        break;
    case DOUBLE_TAP:
        register_code(KC_LALT); // まず左Altを押しっぱなしにする
        register_code(KC_GRV);  // その状態でKC_GRV（バッククォート/半角全角）を押す
    break;
    case SINGLE_TAP_HOLD:
        register_code(KC_LCTL);
        break;
  }
}

void x_reset_1 (tap_dance_state_t *state, void *user_data) {
  switch (xtap_state.state) {
    case SINGLE_TAP: 
        unregister_code(KC_LCTL);
        break;
    case SINGLE_HOLD:
        unregister_code(KC_LCTL);
        break;
    case DOUBLE_TAP:
        unregister_code(KC_GRV);  // 先にKC_GRVを離す
        unregister_code(KC_LALT); // 最後に左Altを離す
        break;
    case SINGLE_TAP_HOLD:
        unregister_code(KC_LCTL);
        break;
  }
  xtap_state.state = 0;
}

/* X_TAP_DANCE_2の定義 */
void x_finished_2 (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  switch (xtap_state.state) {
    case SINGLE_TAP:
        register_code(KC_LALT);
        break;
    case SINGLE_HOLD:
        register_code(KC_LALT);
        break;
    case DOUBLE_TAP:
        layer_on(3); // シンプルにレイヤー3に固定する
        break;
    case SINGLE_TAP_HOLD:
        register_code(KC_LALT);
        break;
  }
}

void x_reset_2 (tap_dance_state_t *state, void *user_data) {
  switch (xtap_state.state) {
    case SINGLE_TAP: 
        unregister_code(KC_LALT);
        break;
    case SINGLE_HOLD:
        unregister_code(KC_LALT);
        break;
    case DOUBLE_TAP:
        // 何もしない
        break;
    case SINGLE_TAP_HOLD:
        unregister_code(KC_LALT);
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
// 2. カスタムキーコードの定義
// ==========================================

enum custom_keycodes {
    DYNAMIC_KEYMAP_KEYCODES = QK_KB_0, // VIA対応のための記述
    // ここにあなたのマクロキーコードを追加します
    MC_SAIZEN,   //  ppt最前に移動
    MC_SAIKOU,   //  ppt最末尾に移動
    MC_05,   //   ppt
    MC_04,   //  縦幅
    MC_03,   //  横幅
    TD_ALT_GRV, // かな変換
    MC_OBJECT, // オブジェクト作成
    MC_Z100, // zoom 100%
    MC_Z200, // zoom 200%
    MC_Z400, // zoom 400%
};

// 長押し判定用のタイマーを記録する変数（ファイルの上のほうか、関数の外に置いてください）
static uint16_t my_tap_timer = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // -----------------------------------------------------------------
    // 【ワンショット風の自前処理（完全版）】
    // レイヤー3にいる状態で、何らかのキーが「離された（!pressed）」瞬間にレイヤーを戻す！
    // ただし、タップダンス（TAP_2）自体のイベントは無視する。
    // -----------------------------------------------------------------
    bool should_clear_layer3 = false;
    if (!record->event.pressed && get_highest_layer(layer_state) == 3) {
        if (keycode != TD(X_TAP_DANCE_2)) {
            should_clear_layer3 = true;
        }
    }

    // マクロ処理
    switch (keycode) {
      
      case TD_ALT_GRV:
        if (record->event.pressed) {
            my_tap_timer = timer_read();
            register_code(KC_LCTL);
        } else {
            unregister_code(KC_LCTL);
            if (timer_elapsed(my_tap_timer) < TAPPING_TERM) {
                clear_mods();
                register_code16(LALT(KC_GRV));
                unregister_code16(LALT(KC_GRV));
            }
        }
        if (should_clear_layer3) layer_off(3);
        return false;
        
      case MC_SAIZEN: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_H) SS_TAP(X_G) SS_TAP(X_R));
        }
        if (should_clear_layer3) layer_off(3);
        return false;

      case MC_SAIKOU: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_H) SS_TAP(X_G) SS_TAP(X_K));
        }
        if (should_clear_layer3) layer_off(3);
        return false;

      case MC_05: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_0) SS_TAP(X_5));
        }
        if (should_clear_layer3) layer_off(3);
        return false;

      case MC_04: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_0) SS_TAP(X_4));
        }
        if (should_clear_layer3) layer_off(3);
        return false;

      case MC_03: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_0) SS_TAP(X_3));
        }
        if (should_clear_layer3) layer_off(3);
        return false;
        
      case MC_OBJECT: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_H) SS_TAP(X_S) SS_TAP(X_H));
        }
        if (should_clear_layer3) layer_off(3);
        return false;
      case MC_Z100: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_W) SS_TAP(X_Q) SS_TAP(X_1) SS_TAP(X_ENT));
        }
        if (should_clear_layer3) layer_off(3);
        return false;
      case MC_Z200: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_W) SS_TAP(X_Q) SS_TAP(X_2) SS_TAP(X_ENT));
        }
        if (should_clear_layer3) layer_off(3);
        return false;
      case MC_Z400: 
        if (record->event.pressed) {
            SEND_STRING(SS_TAP(X_LALT) SS_TAP(X_W) SS_TAP(X_Q) SS_TAP(X_4) SS_TAP(X_ENT));
        }
        if (should_clear_layer3) layer_off(3);
        return false;
    }
    
    // 通常のキー（KC_NやLALT(KC_1)など）が「離された」タイミングであれば、レイヤー3をオフにする
    if (should_clear_layer3) {
        layer_off(3);
    }
    return true; 
}
// ==========================================
// 2. キーマップの配置（TAP_0 の定義より後ろに置く）
// ==========================================
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default (VIA)
  [0] = LAYOUT_universal(
    KC_TAB   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_BSPC  ,
    TD_ALT_GRV  , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                       KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , KC_ENT  ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , MO(4)  ,
             MO(4), KC_LGUI  , TAP_2, KC_BTN1  , KC_SPC     ,                                        LT(1,KC_EQL),LT(2,KC_MINS), _______, _______  , LT(3,KC_QUOT)
  ),

  [1] = LAYOUT_universal(
    _______  ,  KC_F1   , KC_F2    , KC_F3   , KC_F4    , KC_F5    ,                                         KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   , KC_F11   ,
    _______  ,  _______ , LGUI(KC_LEFT), LGUI(KC_DOWN), LGUI(KC_UP), LGUI(KC_RIGHT),                         KC_LEFT  , KC_DOWN  , KC_UP    , KC_RIGHT , KC_END  , KC_F12   ,
    _______  ,  _______ , _______  , _______ , _______  , _______  ,                                         KC_HOME  , KC_PGUP  , KC_PGDN  , KC_END   , _______  , _______  ,
                  _______  , _______ , _______  ,         _______  , _______  ,                   _______  , _______  , _______       , _______  , _______
  ),

  [2] = LAYOUT_universal(
    _______  , KC_1      , KC_2     , KC_3    , KC_4     , KC_5    ,                                           KC_6     , KC_7    , KC_8     , KC_9     , KC_0     , _______ ,
    _______  , S(KC_1)   , S(KC_2)  , S(KC_3) , S(KC_4)  , S(KC_5) ,                                         S(KC_6)  , S(KC_7)   , S(KC_8)  , S(KC_9)  , S(KC_0)  , _______ ,
    _______  , _______   , _______     , _______    , _______     ,_______,                                        KC_LBRC  , KC_RBRC   , KC_EQL   , KC_COMM  , KC_GRV   , KC_BSLS ,
                  _______  , _______ , _______  ,         _______  , _______  ,                   _______  , _______  , _______       , _______  , _______
  ),  

  [3] = LAYOUT_universal(
    _______  , MC_OBJECT  , KC_L  , LALT(KC_7)  , LALT(KC_8)  , LALT(KC_9)  ,                                        _______  , _______  , _______  , _______ , _______ , _______  ,
    _______  , _______  , KC_M  , LALT(KC_4)  , LALT(KC_5)  , LALT(KC_6) ,                                        _______  , MC_03  , MC_04  , _______ , _______  , _______  ,
    LSG(KC_S)  , _______  , KC_H  , LALT(KC_1)  , LALT(KC_2)  , LALT(KC_3) ,                                        _______  , _______ , _______ , _______  , _______  , _______ ,
                  QK_BOOT  , MC_05  , _______  ,        KC_N  , _______  ,                   MC_SAIZEN  , MC_SAIKOU  , _______       , _______  , _______
  ),

  [4] = LAYOUT_universal(
    _______  , _______   , LALT(KC_F4)  , _______  , _______  , _______  ,                                        LALT(KC_MS_WH_LEFT)  , LALT(KC_MS_WH_DOWN)  , LALT(KC_MS_WH_UP)  , LALT(KC_MS_WH_RIGHT) , _______ , _______  ,
    _______  , _______  , KC_MS_WH_LEFT  , KC_MS_WH_DOWN , KC_MS_WH_UP   , KC_MS_WH_RIGHT   ,                KC_MS_WH_LEFT  , KC_MS_WH_DOWN  , KC_MS_WH_UP  , KC_MS_WH_RIGHT , _______  , _______  ,
    _______ , _______  , _______  , _______  , _______  , _______ ,                                        MC_Z100  , MC_Z200 , MC_Z400 , _______  , _______  , _______ ,
                  QK_BOOT  , KBC_RST  , _______  ,        _______  , _______  ,                   _______  , _______  , _______       , KBC_RST  , QK_BOOT
  ),
};

// ==========================================
// 3. レイヤー変更時のスクロールモード制御
// ==========================================
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t highest_layer = get_highest_layer(state);
    
    // レイヤー3（横スクロール用）または レイヤー4（縦スクロール用）のときにスクロールを有効化
    if (highest_layer == 4 || highest_layer == 2) {
        keyball_set_scroll_mode(true);
    } else {
        keyball_set_scroll_mode(false);
    }
    return state;
}

// ==========================================
// 4. トラックボールの挙動カスタム（レイヤー3を横スクロール化）
// ==========================================
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // 現在のレイヤーが 4 のときだけ処理
    if (get_highest_layer(layer_state) == 2) {
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
const uint16_t PROGMEM my_hj[] = {KC_H, KC_J, COMBO_END};
const uint16_t PROGMEM my_jk[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM my_kl[] = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM my_lscln[] = {KC_L, KC_SCLN, COMBO_END};
const uint16_t PROGMEM my_kscln[] = {KC_K, KC_SCLN, COMBO_END};
const uint16_t PROGMEM my_gh[] = {KC_G, KC_H, COMBO_END};
const uint16_t PROGMEM my_bn[] = {KC_B, KC_N, COMBO_END};
const uint16_t PROGMEM my_tabq[] = {KC_TAB, KC_Q, COMBO_END};
const uint16_t PROGMEM my_bacp[] = {KC_BSPC, KC_P, COMBO_END};
const uint16_t PROGMEM my_spcmo1[] = {KC_SPC, MO(1), COMBO_END};
const uint16_t PROGMEM my_qw[] = {KC_Q, KC_W, COMBO_END};
const uint16_t PROGMEM my_sd[] = {KC_S, KC_E, COMBO_END};
const uint16_t PROGMEM my_we[] = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM my_yu[] = {KC_Y, KC_U, COMBO_END};
const uint16_t PROGMEM my_df[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM my_fg[] = {KC_F, KC_G, COMBO_END};
const uint16_t PROGMEM my_vb[] = {KC_V, KC_B, COMBO_END};


combo_t key_combos[] = {  
  COMBO(my_hj, KC_MINS), // - _
  COMBO(my_jk, LALT(KC_GRV)), // 左クリック
  COMBO(my_kl, KC_BTN1), // 右クリック
  COMBO(my_lscln, MO(4)), // 縦スクロール
  COMBO(my_kscln, KC_BTN2), // 縦スクロール
  COMBO(my_gh, KC_EQL), // = +
  COMBO(my_bn, KC_MINS), // - _
  COMBO(my_tabq, KC_ESC), // エスケープ
  COMBO(my_bacp, KC_DEL), // デリート
  COMBO(my_spcmo1, KC_QUOT), // '  "
  COMBO(my_qw, LCA(KC_PAUSE)), // Ctrl + Alt + Pause/Break
  COMBO(my_sd, KC_ENT), // エンター
  COMBO(my_we, KC_DEL), // デリート
  COMBO(my_yu, LGUI(KC_SPC)), // Win + Space
  COMBO(my_df, MO(4)), // 右クリック
  COMBO(my_fg, MO(2)), // 縦スクロール
  // COMBO(my_vb, MO(3)), // 横スクロール
};
#endif

