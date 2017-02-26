/*
 * Yabar - A modern and lightweight status bar for X window managers.
 *
 * Copyright (c) 2016, George Badawi
 * See LICENSE for more information.
 *
 */

#include "yabar.h"


yabar_info_t ya;

int main (int argc, char * argv[]) {
	ya_process_opt(argc, argv);
	ya_init();
	ya_execute();


	xcb_generic_event_t *ev;
	while((ev = xcb_wait_for_event(ya.c))) {
		uint8_t type = ev->response_type & ~0x80;
#ifdef YA_MONS
		if(type == ya.xrandr_offset + XCB_RANDR_SCREEN_CHANGE_NOTIFY) {
			ya_handle_screen_change((xcb_randr_screen_change_notify_event_t *) ev);
		}
		else {
#endif //YA_MONS
		switch(type) {
#ifdef YA_INTERNAL_EWMH
			case XCB_PROPERTY_NOTIFY: {
				ya_handle_prop_notify((xcb_property_notify_event_t *) ev);
				break;
			}
#endif //YA_INTERNAL_EWMH
			case XCB_BUTTON_PRESS: {
				ya_handle_button((xcb_button_press_event_t *) ev);
				break;
			}
			case XCB_EXPOSE: {
				if (((xcb_expose_event_t *) ev)->count != 0)
					break;
				ya_bar_t *curbar = ya.curbar;
				for(;curbar; curbar = curbar->next_bar) {
					ya_redraw_bar(curbar);
				}
				break;
			}
			}
#ifdef YA_MONS
		}
#endif //YA_MONS
		free(ev);
	}
	//shouldn't get here
	xcb_disconnect(ya.c);
}

