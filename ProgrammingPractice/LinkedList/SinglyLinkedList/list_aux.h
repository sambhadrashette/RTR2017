#include "list.h"

#ifndef _LIST_AUX_H
#define _LIST_AUX_H

//----------------------------------------------------------------internal apis---------------------------------------------------------
//we call it the list auxillaries

node_t* get_node(data_t data);
node_t* get_last_node(list_t *lst);
node_t* get_prev_node(list_t*lst, node_t *e_node);
void generic_insert(node_t *prev, node_t *target);
void generic_delete(node_t *prev, node_t *target);
node_t *search_node(list_t *lst, data_t data);
node_t *search_prev_node(list_t *lst, data_t data);
void* xcalloc(int number_of_elements, int size_of_elements);

#endif
