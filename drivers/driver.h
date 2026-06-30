#pragma once

#include <stdint.h>

typedef int (*driver_init_fn)(void);
typedef void (*driver_irq_fn)(uint8_t irq);

typedef struct driver {
    const char *name;
    uint32_t id;
    driver_init_fn init;
    driver_irq_fn irq;
    struct driver *next;
} driver_t;

#define DRIVER_REGISTER(drv) \
    __attribute__((used, section(".drivers"))) \
    static driver_t *const __driver_ptr_##drv = &(drv)

void driver_subsystem_init(void);
int driver_register(driver_t *drv);
driver_t *driver_find(uint32_t id);
void driver_dispatch_irq(uint8_t irq);

#define DRIVER_ID_TIMER    1
#define DRIVER_ID_KEYBOARD 2
#define DRIVER_ID_FB       3
