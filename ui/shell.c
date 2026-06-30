#include "shell.h"
#include "widget.h"
#include "gfx.h"
#include "text.h"
#include "input.h"
#include "drivers/framebuffer.h"
#include "drivers/timer.h"
#include "fs/vfs.h"

#define SHELL_SIDEBAR_W 180

static ui_panel_t main_panel;
static ui_panel_t sidebar_panel;
static ui_button_t btn_clear;
static ui_button_t btn_about;
static ui_textfield_t cmd_field;
static char cmd_buf[INPUT_MAX];
static bool dirty;

static void shell_layout(void) {
    framebuffer_t *fb = fb_info();
    if (!fb_ready())
        return;

    uint32_t sw = fb->width;
    uint32_t sh = fb->height;
    uint32_t top = 40;
    uint32_t bottom = 48;

    sidebar_panel.bounds.x = 0;
    sidebar_panel.bounds.y = top;
    sidebar_panel.bounds.w = SHELL_SIDEBAR_W;
    sidebar_panel.bounds.h = sh - top - bottom;
    sidebar_panel.fill = 0x1A1A2E;
    sidebar_panel.border = 0x533483;
    sidebar_panel.title_bg = 0x16213E;
    sidebar_panel.title = "Apps";

    main_panel.bounds.x = SHELL_SIDEBAR_W + 12;
    main_panel.bounds.y = top + 12;
    main_panel.bounds.w = sw - SHELL_SIDEBAR_W - 24;
    main_panel.bounds.h = sh - top - bottom - 24;
    main_panel.fill = 0x16213E;
    main_panel.border = 0xE94560;
    main_panel.title_bg = 0x0F3460;
    main_panel.title = "Desktop";

    btn_clear.bounds.x = 16;
    btn_clear.bounds.y = top + 40;
    btn_clear.bounds.w = SHELL_SIDEBAR_W - 32;
    btn_clear.bounds.h = 32;
    btn_clear.fill = 0x0F3460;
    btn_clear.border = 0x53D8FB;
    btn_clear.text_color = 0xFFFFFF;
    btn_clear.label = "Clear";
    btn_clear.pressed = false;

    btn_about.bounds.x = 16;
    btn_about.bounds.y = top + 80;
    btn_about.bounds.w = SHELL_SIDEBAR_W - 32;
    btn_about.bounds.h = 32;
    btn_about.fill = 0x0F3460;
    btn_about.border = 0x53D8FB;
    btn_about.text_color = 0xFFFFFF;
    btn_about.label = "About";
    btn_about.pressed = false;

    cmd_field.bounds.x = 16;
    cmd_field.bounds.y = sh - bottom + 8;
    cmd_field.bounds.w = sw - 32;
    cmd_field.bounds.h = 32;
    cmd_field.fill = 0x1E1E2E;
    cmd_field.border = 0x5C5C7A;
    cmd_field.text_color = 0xFFFFFF;
    cmd_field.buffer = cmd_buf;
    cmd_field.buflen = INPUT_MAX;
    cmd_field.focused = true;
}

static void shell_draw_statusbar(void) {
    framebuffer_t *fb = fb_info();
    gfx_draw_rect(0, 0, fb->width, 40, 0x0F3460);
    text_draw(16, 12, "MiraOS", 0xFFFFFF, 0x0F3460);

    char tickbuf[24];
    uint64_t t = timer_ticks();
    int pos = 0;
    tickbuf[pos++] = 't';
    tickbuf[pos++] = '=';
    if (t == 0) {
        tickbuf[pos++] = '0';
    } else {
        char tmp[20];
        int ti = 0;
        while (t > 0) {
            tmp[ti++] = '0' + (t % 10);
            t /= 10;
        }
        while (ti > 0)
            tickbuf[pos++] = tmp[--ti];
    }
    tickbuf[pos] = 0;
    text_draw(fb->width - 100, 12, tickbuf, 0xA0A0A0, 0x0F3460);
}

static void shell_draw_main_content(void) {
    uint32_t cx = main_panel.bounds.x + 16;
    uint32_t cy = main_panel.bounds.y + 32;

    text_draw(cx, cy, "Welcome to MiraOS", 0xFFFFFF, main_panel.fill);
    text_draw(cx, cy + 48, "Type in the command bar below. Enter clears the line.", 0x888888, main_panel.fill);

    int fd = vfs_open("/readme.txt", VFS_O_RDONLY);
    if (fd >= 0) {
        char filebuf[80];
        ssize_t n = vfs_read(fd, filebuf, 79);
        if (n > 0) {
            filebuf[n] = 0;
            text_draw(cx, cy + 56, filebuf, 0x53D8FB, main_panel.fill);
        }
        vfs_close(fd);
    }

    fd = vfs_open("/version", VFS_O_RDONLY);
    if (fd >= 0) {
        char verbuf[16];
        ssize_t n = vfs_read(fd, verbuf, 15);
        if (n > 0) {
            verbuf[n] = 0;
            text_draw(cx, cy + 88, "Version: ", 0xAAAAAA, main_panel.fill);
            text_draw(cx + 72, cy + 88, verbuf, 0xE94560, main_panel.fill);
        }
        vfs_close(fd);
    }

    gfx_draw_rect(cx, cy + 120, 200, 80, 0x0F3460);
    gfx_draw_rect_outline(cx, cy + 120, 200, 80, 0x53D8FB, 2);
    text_draw(cx + 16, cy + 140, "Widget Demo", 0xFFFFFF, 0x0F3460);
    text_draw(cx + 16, cy + 160, "Rect + text", 0xCCCCCC, 0x0F3460);
}

void shell_init(void) {
    dirty = true;
    input_init();
    cmd_buf[0] = 0;
    cmd_field.len = 0;
    shell_layout();
}

void shell_tick(void) {
    size_t prev = input_length();
    input_poll();
    const char *buf = input_buffer();
    size_t len = input_length();
    if (len >= INPUT_MAX)
        len = INPUT_MAX - 1;
    for (size_t i = 0; i < len; i++)
        cmd_buf[i] = buf[i];
    cmd_buf[len] = 0;
    cmd_field.len = len;
    if (len != prev)
        dirty = true;
    if (len == 5 && cmd_buf[0] == 'c' && cmd_buf[1] == 'l' && cmd_buf[2] == 'e' && cmd_buf[3] == 'r' && cmd_buf[4] == 'a') {
        input_clear();
        cmd_buf[0] = 0;
        cmd_field.len = 0;
        dirty = true;
    }
}

void shell_render(void) {
    if (!fb_ready())
        return;

    shell_layout();
    framebuffer_t *fb = fb_info();
    gfx_draw_rect(0, 0, fb->width, fb->height, 0x0A0A14);

    shell_draw_statusbar();
    ui_panel_draw(&sidebar_panel);
    ui_panel_draw(&main_panel);
    ui_button_draw(&btn_clear);
    ui_button_draw(&btn_about);
    shell_draw_main_content();
    ui_textfield_draw(&cmd_field);

    gfx_draw_rect(0, fb->height - 40, fb->width, 40, 0x0F3460);
    text_draw(16, fb->height - 28, "Enter: clear line | Backspace: edit", 0x888888, 0x0F3460);

    dirty = false;
}

bool shell_dirty(void) {
    return dirty || input_dirty();
}

void shell_clear_dirty(void) {
    dirty = false;
    input_clear_dirty();
}
