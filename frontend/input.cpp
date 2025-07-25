#include "iris.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace iris {

uint32_t map_button(SDL_Keycode k) {
    if (k == SDLK_X     ) return DS_BT_CROSS;
    if (k == SDLK_A     ) return DS_BT_SQUARE;
    if (k == SDLK_W     ) return DS_BT_TRIANGLE;
    if (k == SDLK_D     ) return DS_BT_CIRCLE;
    if (k == SDLK_RETURN) return DS_BT_START;
    if (k == SDLK_S     ) return DS_BT_SELECT;
    if (k == SDLK_UP    ) return DS_BT_UP;
    if (k == SDLK_DOWN  ) return DS_BT_DOWN;
    if (k == SDLK_LEFT  ) return DS_BT_LEFT;
    if (k == SDLK_RIGHT ) return DS_BT_RIGHT;
    if (k == SDLK_Q     ) return DS_BT_L1;
    if (k == SDLK_E     ) return DS_BT_R1;
    if (k == SDLK_1     ) return DS_BT_L2;
    if (k == SDLK_3     ) return DS_BT_R2;
    if (k == SDLK_Z     ) return DS_BT_L3;
    if (k == SDLK_C     ) return DS_BT_R3;

    return 0;
}

bool save_screenshot(iris::instance* iris, std::string path) {
    int w, h, bpp;

    void* ptr = renderer_get_buffer_data(iris->ctx, &w, &h, &bpp);

    if (!ptr) return false;

    uint32_t* buf = (uint32_t*)malloc((w * bpp) * h);

    memcpy(buf, ptr, (w * bpp) * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            buf[x + (y * w)] |= 0xff000000;
        }
    }

    stbi_write_png(path.c_str(), w, h, 4, buf, w * bpp);

    return true;
}

void handle_keydown_event(iris::instance* iris, SDL_KeyboardEvent& key) {
    switch (key.key) {
        case SDLK_SPACE: iris->pause = !iris->pause; break;
        case SDLK_F9: {
            bool saved = save_screenshot(iris, "screenshot.png");

            if (saved) {
                push_info(iris, "Screenshot saved to \'screenshot.png\'");
            } else {
                push_info(iris, "Couldn't save screenshot");
            }
        } break;
        case SDLK_F11: {
            iris->fullscreen = !iris->fullscreen;

            SDL_SetWindowFullscreen(iris->window, iris->fullscreen ? true : false);
        } break;
        case SDLK_F1: {
            printf("ps2: Sending poweroff signal\n");
            ps2_cdvd_power_off(iris->ps2->cdvd);
        } break;
        case SDLK_0: {
            ps2_iop_intc_irq(iris->ps2->iop_intc, IOP_INTC_USB);
        } break;
    }

    uint16_t mask = map_button(key.key);

    ds_button_press(iris->ds[0], mask);
}

void handle_keyup_event(iris::instance* iris, SDL_KeyboardEvent& key) {
    uint16_t mask = map_button(key.key);

    ds_button_release(iris->ds[0], mask);
}

}