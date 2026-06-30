#include "framebuffer.h"
#include "driver.h"
#include "kernel.h"
#include "kernel/panic.h"

static framebuffer_t fb;

static struct multiboot_tag *mbi_next(struct multiboot_tag *tag) {
    return (struct multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7));
}

static int fb_driver_init(void) {
    return fb_ready() ? 0 : -1;
}

static driver_t fb_driver = {
    .name = "fb",
    .id = DRIVER_ID_FB,
    .init = fb_driver_init,
    .irq = 0,
    .next = 0
};

DRIVER_REGISTER(fb_driver);

bool fb_init(uint64_t mbi) {
    if (*(uint32_t *)mbi != MULTIBOOT2_MAGIC)
        return false;

    struct multiboot_tag *tag = (struct multiboot_tag *)(mbi + 8);
    while (tag->type != 0) {
        if (tag->type == 8) {
            struct multiboot_tag_framebuffer *fbt = (void *)tag;
            fb.addr = fbt->framebuffer_addr;
            fb.pitch = fbt->framebuffer_pitch;
            fb.width = fbt->framebuffer_width;
            fb.height = fbt->framebuffer_height;
            fb.bpp = fbt->framebuffer_bpp;
            return fb.bpp == 32;
        }
        tag = mbi_next(tag);
    }
    return false;
}

bool fb_ready(void) {
    return fb.addr != 0 && fb.bpp == 32;
}

framebuffer_t *fb_info(void) {
    return &fb;
}

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!fb_ready() || x >= fb.width || y >= fb.height)
        return;
    uint32_t *pixel = (uint32_t *)(fb.addr + y * fb.pitch + x * 4);
    *pixel = color;
}

void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (!fb_ready())
        return;
    for (uint32_t row = y; row < y + h && row < fb.height; row++) {
        uint32_t *line = (uint32_t *)(fb.addr + row * fb.pitch + x * 4);
        for (uint32_t col = 0; col < w && x + col < fb.width; col++)
            line[col] = color;
    }
}
