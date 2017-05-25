#ifndef _link_h_
#define _link_h_

#include <stddef.h>

#define insert_to_link(node, link_head, next_link) { \
	if (link_head == NULL) { \
		link_head = node; \
	} else { \
		typeof(node) old_head = link_head; \
		node -> next_link = old_head; \
		link_head = node; \
	} \
}

#endif
