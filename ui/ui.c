#include "ui.h"
#include "shell.h"
#include "gfx.h"
#include "text.h"
#include "widget.h"
#include "drivers/framebuffer.h"
#include "kernel/heap.h"

static ui_widget_t *root_widget = 0;
static ui_widget_t *focused_widget = 0;
static bool ui_dirty_flag = false;

void ui_init(void) {
    gfx_init();
    text_init();
    shell_init();
    
    /* Create root widget */
    framebuffer_t *fb = fb_info();
    ui_rect_t root_bounds = {0, 0, fb->width, fb->height};
    root_widget = ui_create_widget(0, &root_bounds);
    root_widget->visible = true;
}

void ui_tick(void) {
    shell_tick();
}

static void ui_paint_widget(ui_widget_t *widget) {
    if (!widget || !widget->visible)
        return;
    
    /* Paint children first */
    ui_widget_t *child = widget->children;
    while (child) {
        ui_paint_widget(child);
        child = child->next;
    }
    
    /* Paint this widget */
    if (widget->on_paint)
        widget->on_paint(widget);
}

void ui_render(void) {
    if (!fb_ready())
        return;
    
    /* Clear screen */
    gfx_clear(0x0A0A14);
    
    /* Paint widget tree */
    ui_paint_widget(root_widget);
    
    ui_clear_dirty();
}

bool ui_dirty(void) {
    return ui_dirty_flag || shell_dirty();
}

void ui_clear_dirty(void) {
    ui_dirty_flag = false;
    shell_clear_dirty();
}

ui_widget_t *ui_create_widget(ui_widget_t *parent, const ui_rect_t *bounds) {
    ui_widget_t *widget = kmalloc(sizeof(ui_widget_t));
    if (!widget)
        return 0;
    
    widget->bounds = *bounds;
    widget->absolute_bounds = *bounds;
    widget->parent = parent;
    widget->children = 0;
    widget->next = 0;
    widget->prev = 0;
    widget->visible = true;
    widget->enabled = true;
    widget->focused = false;
    widget->user_data = 0;
    widget->on_paint = 0;
    widget->on_event = 0;
    widget->on_focus = 0;
    
    if (parent)
        ui_add_child(parent, widget);
    
    return widget;
}

void ui_destroy_widget(ui_widget_t *widget) {
    if (!widget)
        return;
    
    /* Remove from parent */
    if (widget->parent)
        ui_remove_child(widget->parent, widget);
    
    /* Destroy children */
    while (widget->children) {
        ui_destroy_widget(widget->children);
    }
    
    kfree(widget);
}

void ui_add_child(ui_widget_t *parent, ui_widget_t *child) {
    if (!parent || !child)
        return;
    
    child->parent = parent;
    child->next = parent->children;
    child->prev = 0;
    
    if (parent->children)
        parent->children->prev = child;
    
    parent->children = child;
    
    /* Update absolute bounds */
    ui_widget_t *ancestor = parent;
    while (ancestor) {
        child->absolute_bounds.x += ancestor->bounds.x;
        child->absolute_bounds.y += ancestor->bounds.y;
        ancestor = ancestor->parent;
    }
}

void ui_remove_child(ui_widget_t *parent, ui_widget_t *child) {
    if (!parent || !child || child->parent != parent)
        return;
    
    if (child->prev)
        child->prev->next = child->next;
    else
        parent->children = child->next;
    
    if (child->next)
        child->next->prev = child->prev;
    
    child->parent = 0;
    child->next = 0;
    child->prev = 0;
}

bool ui_post_event(const ui_event_t *event) {
    if (!event)
        return false;
    
    /* Find widget at event position */
    ui_widget_t *target = root_widget;
    while (target) {
        if (ui_rect_contains(&target->absolute_bounds, event->x, event->y)) {
            /* Check children first (top to bottom) */
            ui_widget_t *child = target->children;
            while (child) {
                if (ui_rect_contains(&child->absolute_bounds, event->x, event->y)) {
                    target = child;
                    break;
                }
                child = child->next;
            }
            
            /* Handle event */
            if (target->on_event) {
                if (target->on_event(target, event))
                    return true;
            }
        }
        target = target->next;
    }
    
    return false;
}

void ui_set_focus(ui_widget_t *widget) {
    if (focused_widget == widget)
        return;
    
    /* Blur old focus */
    if (focused_widget && focused_widget->on_focus)
        focused_widget->on_focus(focused_widget, false);
    
    focused_widget = widget;
    
    /* Focus new widget */
    if (focused_widget && focused_widget->on_focus)
        focused_widget->on_focus(focused_widget, true);
    
    ui_dirty_flag = true;
}

ui_widget_t *ui_get_focus(void) {
    return focused_widget;
}

void ui_set_visible(ui_widget_t *widget, bool visible) {
    if (widget)
        widget->visible = visible;
}

void ui_set_enabled(ui_widget_t *widget, bool enabled) {
    if (widget)
        widget->enabled = enabled;
}