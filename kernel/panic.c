#include "panic.h"
#include "drivers/framebuffer.h"
#include "ui/text.h"

void panic(const char *msg) {
    if (fb_ready())
        text_draw(10, 10, msg, 0xFF0000, 0x000000);
    for (;;)
        __asm__ volatile ("cli; hlt");
}
