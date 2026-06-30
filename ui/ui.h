#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "ui/widget.h"

typedef struct ui_widget ui_widget_t;

typedef enum {
    UI_EVENT_NONE = 0,
    UI_EVENT_MOUSE_MOVE,
    UI_EVENT_MOUSE_DOWN,
    UI_EVENT_MOUSE_UP,
    UI_EVENT_KEY_DOWN,
    UI_EVENT_KEY_UP,
    UI_EVENT_FOCUS,
    UI_EVENT_BLUR
} ui_event_type_t;

typedef struct {
    ui_event_type_t type;
    uint32_t x;
    uint32_t y;
    uint32_t button;
    uint32_t keycode;
    bool handled;
} ui_event_t;

typedef struct ui_widget {
    ui_rect_t bounds;
    ui_rect_t absolute_bounds;
    struct ui_widget *parent;
    struct ui_widget *children;
    struct ui_widget *next;
    struct ui_widget *prev;
    bool visible;
    bool enabled;
    bool focused;
    void *user_data;
    void (*on_paint)(struct ui_widget *widget);
    bool (*on_event)(struct ui_widget *widget, const ui_event_t *event);
    void (*on_focus)(struct ui_widget *widget, bool focused);
} ui_widget_t;

void ui_init(void);
void ui_tick(void);
void ui_render(void);
bool ui_dirty(void);
void ui_clear_dirty(void);

ui_widget_t *ui_create_widget(ui_widget_t *parent, const ui_rect_t *bounds);
void ui_destroy_widget(ui_widget_t *widget);
void ui_add_child(ui_widget_t *parent, ui_widget_t *child);
void ui_remove_child(ui_widget_t *parent, ui_widget_t *child);
bool ui_post_event(const ui_event_t *event);
void ui_set_focus(ui_widget_t *widget);
ui_widget_t *ui_get_focus(void);
void ui_set_visible(ui_widget_t *widget, bool visible);
void ui_set_enabled(ui_widget_t *widget, bool enabled);