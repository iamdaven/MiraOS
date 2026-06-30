#include "ui.h"
#include "shell.h"
#include "gfx.h"
#include "text.h"

void ui_init(void) {
    gfx_init();
    text_init();
    shell_init();
}

void ui_tick(void) {
    shell_tick();
}

void ui_render(void) {
    shell_render();
}

bool ui_dirty(void) {
    return shell_dirty();
}

void ui_clear_dirty(void) {
    shell_clear_dirty();
}
