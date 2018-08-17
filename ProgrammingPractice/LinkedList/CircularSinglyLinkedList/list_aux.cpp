#include<stdio.h>
#include<stdlib.h>
#include "list.h"

//----------------------------------------------------------------internal apis---------------------------------------------------------
//we call it the list auxillaries

node_t* get_node(data_t data) {
	return 0;
}
node_t* get_last_node(list_t *lst) {
	return 0;
}

node_t* get_prev_node(list_t*lst, node_t *e_node) {
	return 0;
}
void generic_insert(node_t *prev, node_t *target) {
	//TODO to be implemented
}
void generic_delete(node_t *prev, node_t *target) {
	//TODO to be implemented
}

node_t *search_node(list_t *lst, data_t data) {
	return 0;
}

node_t *search_prev_node(list_t *lst, data_t data) {
	return 0;
}
void* xcalloc(int number_of_elements, int size_of_elements) {
	return (void*) malloc(number_of_elements*size_of_elements);
}

