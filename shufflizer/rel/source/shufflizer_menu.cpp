#include "shufflizer_menu.h"

#include <cstdint>
#include <cstdio>
#include <ttyd/common_types.h>
#include <ttyd/fontmgr.h>
#include <ttyd/seqdrv.h>
#include <ttyd/string.h>
#include <ttyd/system.h>
#include <ttyd/windowdrv.h>

#include "common.h"
#include "shufflizer_options.h"

namespace mod::shufflizer {
    
namespace {
    
namespace ButtonId = ttyd::common::ButtonId;
    
// Menu states.
namespace MenuState {
    enum e {
        INVALID_MENU_STATE = 0,
        CHANGE_PAGE,
        HP_MODIFIER,
        ATK_MODIFIER,
        DEF_MODIFIER,
        SUPERGUARD_COST,
        SHUFFLE_MISC_ITEMS,
        PIT_SHUFFLE_FLOORS,
        CHARLIETON_FIXED_ITEMS,
        SELECT_STACKED_POWER,
    };
}

// Menu constants.
const int32_t kFadeoutStartTime     = 240;
const int32_t kFadeoutEndTime       = 250;
const uint16_t kMenuUpCommand       = ButtonId::Z | ButtonId::DPAD_UP;
const uint16_t kMenuDownCommand     = ButtonId::Z | ButtonId::DPAD_DOWN;
const uint16_t kMenuLeftCommand     = ButtonId::Z | ButtonId::DPAD_LEFT;
const uint16_t kMenuRightCommand    = ButtonId::Z | ButtonId::DPAD_RIGHT;
const uint16_t kMenuSelectCommand   = ButtonId::Z | ButtonId::L;

// For automatically ticking options +1/-1 at a time if a direction is held.
const int32_t kMenuCommandSlowTickStart     = 24;
const int32_t kMenuCommandSlowTickRate      = 12;
const int32_t kMenuCommandFastTickStart     = 120;
const int32_t kMenuCommandFastTickRate      = 3;
    
bool OnTitleScreen() {
    int32_t sequence = ttyd::seqdrv::seqGetNextSeq();
    return sequence == static_cast<int32_t>(ttyd::seqdrv::SeqIndex::kTitle);
}

bool InMainGameModes() {
    int32_t sequence = ttyd::seqdrv::seqGetNextSeq();
    int32_t game = static_cast<int32_t>(ttyd::seqdrv::SeqIndex::kGame);
    int32_t game_over = static_cast<int32_t>(ttyd::seqdrv::SeqIndex::kGameOver);

    bool next_map_demo = !ttyd::string::strcmp(common::kNextMap, "dmo_00");
    bool next_map_title = !ttyd::string::strcmp(common::kNextMap, "title");
    
    return (sequence >= game) && (sequence <= game_over) && 
           !next_map_demo && !next_map_title;
}

void DrawWindow(
    uint8_t color[4], float x, float y, float width, float height, float curve) {
    ttyd::windowdrv::windowDispGX_Waku_col(
        0, color, x, y, width, height, curve);
}

void DrawString(
    const char* data, int32_t x, int32_t y, uint32_t color, float scale = 1.0) {
    if (!data) return;
    ttyd::fontmgr::FontDrawStart();
    ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t*>(&color));
    ttyd::fontmgr::FontDrawEdge();
    ttyd::fontmgr::FontDrawScale(scale);
    ttyd::fontmgr::FontDrawString(x, y, data);
}

bool ShouldTickOrAutotick(int32_t time_held) {
    if (time_held == 0) return true;
    if (time_held >= kMenuCommandFastTickStart &&
        time_held % kMenuCommandFastTickRate == 0) return true;
    if (time_held >= kMenuCommandSlowTickStart &&
        time_held % kMenuCommandSlowTickRate == 0) return true;
    return false;
}

// Global menu state variables.
uint16_t last_command_      = 0;
int32_t time_button_held_   = kFadeoutEndTime;
int32_t menu_selection_     = 1;
int32_t menu_page_          = 1;
int32_t menu_state_         = MenuState::HP_MODIFIER;

}

ShufflizerMenu::ShufflizerMenu(ShufflizerOptions* options) : options_(options) {}
    
void ShufflizerMenu::Update() {
    uint16_t buttons = ttyd::system::keyGetButton(0);
    
    if (last_command_ && (buttons & last_command_) == last_command_) {
        // If last command is non-empty and still held, increment timer.
        ++time_button_held_;
    } else {
        const uint16_t prev_command = last_command_;
        uint16_t new_command = 0;
        if ((buttons & kMenuSelectCommand) == kMenuSelectCommand) {
            new_command = kMenuSelectCommand;
        } else if ((buttons & kMenuUpCommand) == kMenuUpCommand) {
            new_command = kMenuUpCommand;
        } else if ((buttons & kMenuDownCommand) == kMenuDownCommand) {
            new_command = kMenuDownCommand;
        } else if ((buttons & kMenuLeftCommand) == kMenuLeftCommand) {
            new_command = kMenuLeftCommand;
        } else if ((buttons & kMenuRightCommand) == kMenuRightCommand) {
            new_command = kMenuRightCommand;
        }
        last_command_ = new_command;
        
        if (new_command && !prev_command && time_button_held_ >= kFadeoutEndTime) {
            // Command input when menu was previously not displayed;
            // no action should be taken until the next command is input.
            time_button_held_ = -999999999;
        } else if (new_command != prev_command) {
            // New command input; reset timer.
            time_button_held_ = 0;
        } else {
            // Still no command input; increment timer anyway.
            ++time_button_held_;
        }
    }
    
    switch (100 * menu_page_ + menu_selection_) {
        case 101: {
            menu_state_ = MenuState::HP_MODIFIER;
            break;
        }
        case 102: {
            menu_state_ = MenuState::ATK_MODIFIER;
            break;
        }
        case 103: {
            menu_state_ = MenuState::DEF_MODIFIER;
            break;
        }
        case 104: {
            menu_state_ = MenuState::SUPERGUARD_COST;
            break;
        }
        case 201: {
            menu_state_ = MenuState::SHUFFLE_MISC_ITEMS;
            break;
        }
        case 202: {
            menu_state_ = MenuState::PIT_SHUFFLE_FLOORS;
            break;
        }
        case 203: {
            menu_state_ = MenuState::CHARLIETON_FIXED_ITEMS;
            break;
        }
        case 204: {
            menu_state_ = MenuState::SELECT_STACKED_POWER;
            break;
        }
        default: {
            menu_state_ = MenuState::CHANGE_PAGE;
            break;
        }
    }
    
    if (time_button_held_ < 0) return;
    switch (last_command_) {
        case kMenuUpCommand: {
            if (time_button_held_ == 0) {
                if (menu_selection_ == 1) {
                    menu_selection_ = 5;
                } else {
                    --menu_selection_;
                }
            }
            break;
        }
        case kMenuDownCommand: {
            if (time_button_held_ == 0) {
                if (menu_selection_ == 5) {
                    menu_selection_ = 1;
                } else {
                    ++menu_selection_;
                }
            }
            break;
        }
        case kMenuSelectCommand: {
            if (time_button_held_ == 0) {
                switch (menu_state_) {
                    case MenuState::CHANGE_PAGE: {
                        menu_page_ = menu_page_ ^ 3;
                        break;
                    }
                    case MenuState::SHUFFLE_MISC_ITEMS: {
                        options_->shuffle_misc_items =
                        !options_->shuffle_misc_items;
                        break;
                    }
                    case MenuState::PIT_SHUFFLE_FLOORS: {
                        options_->shuffle_pit_floors =
                        !options_->shuffle_pit_floors;
                        break;
                    }
                    case MenuState::CHARLIETON_FIXED_ITEMS: {
                        options_->charlieton_fixed =
                        !options_->charlieton_fixed;
                        break;
                    }
                    case MenuState::SELECT_STACKED_POWER: {
                        options_->select_move_power =
                        !options_->select_move_power;
                        break;
                    }
                    default: break;
                }
            }
            break;
        }
        case kMenuLeftCommand: {
            if (ShouldTickOrAutotick(time_button_held_)) {
                switch (menu_state_) {
                    case MenuState::HP_MODIFIER: {
                        if (options_->enemy_hp_modifier > 1) {
                            --options_->enemy_hp_modifier;
                        }
                        break;
                    }
                    case MenuState::ATK_MODIFIER: {
                        if (options_->enemy_atk_modifier > 1) {
                            --options_->enemy_atk_modifier;
                        }
                        break;
                    }
                    case MenuState::DEF_MODIFIER: {
                        if (options_->enemy_def_modifier > 0) {
                            --options_->enemy_def_modifier;
                        }
                        break;
                    }
                    case MenuState::SUPERGUARD_COST: {
                        if (options_->superguard_cost > 0) {
                            --options_->superguard_cost;
                        }
                        break;
                    }
                    default: break;
                }
            }
            break;
        }
        case kMenuRightCommand: {
            if (ShouldTickOrAutotick(time_button_held_)) {
                switch (menu_state_) {
                    case MenuState::HP_MODIFIER: {
                        if (options_->enemy_hp_modifier < 5000) {
                            ++options_->enemy_hp_modifier;
                        }
                        break;
                    }
                    case MenuState::ATK_MODIFIER: {
                        if (options_->enemy_atk_modifier < 5000) {
                            ++options_->enemy_atk_modifier;
                        }
                        break;
                    }
                    case MenuState::DEF_MODIFIER: {
                        if (options_->enemy_def_modifier < 99) {
                            ++options_->enemy_def_modifier;
                        }
                        break;
                    }
                    case MenuState::SUPERGUARD_COST: {
                        if (options_->superguard_cost < 800) {
                            ++options_->superguard_cost;
                        }
                        break;
                    }
                    default: break;
                }
            }
        }
        default: break;
    }
}

void ShufflizerMenu::Draw() {
    if (OnTitleScreen()) {
        uint8_t title_window_color[4] = { 0, 0, 0, 0xCC };
        DrawWindow(title_window_color, -225, -16, 450, 70, 10);
        DrawString("   Shufflizer v1.22 by jdaster64", -175, -30, -1U, 0.75);
        DrawString("Install guide: https://goo.gl/VhiqZH", -175, -53, -1U, 0.75);
    } else if (!InMainGameModes()) {
        return;
    }
    
    uint32_t alpha = 0xFF;
    if (!last_command_ && time_button_held_ >= kFadeoutEndTime) return;
    if (!last_command_ && time_button_held_ >= kFadeoutStartTime) {
        alpha = 0xFF * (kFadeoutEndTime - time_button_held_)
                     / (kFadeoutEndTime - kFadeoutStartTime);
    }
    uint8_t window_alpha = static_cast<uint8_t>(alpha * 4 / 5);
        
    uint8_t menu_window_color[4] = { 0, 0, 0, window_alpha };
    DrawWindow(menu_window_color, -220, -65, 335, 109, 10);
    
    char buf[128];
    uint32_t color;
    
    if (menu_page_ == 1) {
        color = (menu_state_ == MenuState::HP_MODIFIER 
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "HP modifier: %ld%s", options_->enemy_hp_modifier, "%");
        DrawString(buf, -205, -73, color, 0.75);
        
        color = (menu_state_ == MenuState::ATK_MODIFIER
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "ATK modifier: %ld%s", options_->enemy_atk_modifier, "%");
        DrawString(buf, -205, -92, color, 0.75);
        
        color = (menu_state_ == MenuState::DEF_MODIFIER
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "DEF modifier: +%ld", options_->enemy_def_modifier);
        DrawString(buf, -205, -111, color, 0.75);
        
        color = (menu_state_ == MenuState::SUPERGUARD_COST 
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "Superguard cost: %.2f SP",
                options_->superguard_cost * 0.01f);
        DrawString(buf, -205, -130, color, 0.75);
    } else if (menu_page_ == 2) {
        color = (menu_state_ == MenuState::SHUFFLE_MISC_ITEMS 
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "Misc. field items shuffled: %s", 
                options_->shuffle_misc_items ? "Yes" : "No");
        DrawString(buf, -205, -73, color, 0.75);
        
        color = (menu_state_ == MenuState::PIT_SHUFFLE_FLOORS 
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "Pit floors shuffled: %s", 
                options_->shuffle_pit_floors ? "Yes" : "No");
        DrawString(buf, -205, -92, color, 0.75);
        
        color = (menu_state_ == MenuState::CHARLIETON_FIXED_ITEMS 
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "Pit Charlieton items: %s", 
                options_->charlieton_fixed ? "Fixed" : "Random");
        DrawString(buf, -205, -111, color, 0.75);
        
        color = (menu_state_ == MenuState::SELECT_STACKED_POWER 
                 ? -0xFFFFU : -0xFFU) | alpha;
        sprintf(buf, "Stacked move power: %s", 
                options_->select_move_power ? "Custom" : "Fixed");
        DrawString(buf, -205, -130, color, 0.75);
    }
    
    color = (menu_state_ == MenuState::CHANGE_PAGE ? -0xFFFFU : -0xFFU) | alpha;
    sprintf(buf, "Next Page (%ld of 2)", menu_page_);
    DrawString(buf, -205, -149, color, 0.75);
}

}