#include "driver.h"

#define MAX_DRIVERS 32

static driver_t *driver_list[MAX_DRIVERS];
static int driver_count;

extern driver_t *const __start_drivers[];
extern driver_t *const __stop_drivers[];

void driver_subsystem_init(void) {
    driver_count = 0;
    for (driver_t **p = (driver_t **)__start_drivers; p < (driver_t **)__stop_drivers; p++) {
        if (*p)
            driver_register(*p);
    }
    for (int i = 0; i < driver_count; i++) {
        if (driver_list[i]->init)
            driver_list[i]->init();
    }
}

int driver_register(driver_t *drv) {
    if (driver_count >= MAX_DRIVERS)
        return -1;
    driver_list[driver_count++] = drv;
    return 0;
}

driver_t *driver_find(uint32_t id) {
    for (int i = 0; i < driver_count; i++) {
        if (driver_list[i]->id == id)
            return driver_list[i];
    }
    return 0;
}

void driver_dispatch_irq(uint8_t irq) {
    for (int i = 0; i < driver_count; i++) {
        if (driver_list[i]->irq)
            driver_list[i]->irq(irq);
    }
}
