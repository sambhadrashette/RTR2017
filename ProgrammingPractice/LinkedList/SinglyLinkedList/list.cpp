
#include <iostream>
#include <iomanip>
#include "list.h"
#include "list_aux.h"

using namespace std;

//create the list and return back its pointer
list_t* create_list(void) {
	list_t* list = (list_t*) xcalloc(1, sizeof(list_t));
	list->next = NULL;
	return list;

}
//---------------------------------------------------------------operations---------------------------------------------------------------------------
//ndata = newdata element sdata = search data element edata = existing data element
//INSERTION
result_t insert_beg(list_t *list, data_t ndata) {
	list_t* temp = list;
	if (list != NULL)
	{
		node_t* newNode = (node_t*) xcalloc(1, sizeof(node_t));
		newNode->data = ndata;
		newNode->next = list -> next;
		
		if (!list->next) {
			list->next = newNode;
			printf("Empty list added with %d\n", newNode->data);
			return 0;
		}
		list->next = newNode;

		//printf("%d\n", newNode->data);
		return 0;
	}
	return 1;
}

result_t insert_end(list_t *list, data_t ndata) {
	list_t* temp = list;
	list_t* prev = NULL;

	if (list)
	{
		node_t* newNode = (node_t*) xcalloc(1, sizeof(node_t));
		newNode->data = ndata;
		newNode->next = NULL;
		if (!(list->next)) {
			list->next = newNode;
			return 0;
		}

		while (temp->next) {
			temp = temp->next;
			prev = temp;
		}
		prev-> next = newNode;
		return 0;
	}
	return 1;
}

result_t insert_after_data(list_t *list, data_t edata, data_t ndata) {
	list_t* temp = list;
	list_t* prev = NULL;
	list_t* next = NULL;

	if (list)
	{
		node_t* newNode = (node_t*)xcalloc(1, sizeof(node_t));
		newNode->data = ndata;
		newNode->next = NULL;
		if (!(list->next)) {
			list->next = newNode;
			return 0;
		}

		while (temp->next && temp->data != edata) {
			temp = temp->next;
			prev = temp;
			next = temp->next;
		}
		newNode->next = next;
		prev->next = newNode;
		return 0;
	}
	return 1;
}	//append
result_t insert_before_data(list_t *list, data_t edata, data_t ndata) {
	list_t* temp = list;
	list_t* prev = NULL;
	list_t* next = NULL;

	if (list)
	{
		node_t* newNode = (node_t*)xcalloc(1, sizeof(node_t));
		newNode->data = ndata;
		newNode->next = NULL;
		if (!(list->next)) {
			list->next = newNode;
			return 0;
		}

		while (temp->next && temp->data != edata) {
			prev = temp;
			temp = temp->next;
			next = temp->next;
		}
		newNode->next = temp;
		prev->next = newNode;
		return 0;
	}
	return 1;
}	//append before

	//DELETION
result_t delete_end(list_t *list)
{
	list_t* temp = list;
	list_t* prev = NULL;

	if (list)
	{
		
		if (!(list->next)) {
			printf("List is empty, no element deleted.\n");
			return 0;
		}

		while (temp->next) {
			prev = temp;
			temp = temp->next;
		}
		printf("deleting list element : %d\n", temp->data);
		free(temp);
		temp = NULL;
		prev->next = NULL;
		return 0;
	}
	return 1;
}

result_t delete_beg(list_t *list) {
	if (!list->next) {
		printf("List is empty, no element deleted.\n");
		return 0;
	}
	list_t* temp;
	temp = list -> next;
	list -> next = list -> next -> next;

	printf("deleting list element : %d\n", temp->data)
;
	free(temp);
	temp = NULL;
	return 0;

}
result_t delete_data(list_t *list, data_t edata) {
	list_t* current = list;
	list_t* prev = NULL;
	if (list)
	{

		if (!(list->next)) {
			printf("List is empty, no element deleted.\n");
			return 0;
		}

		while (current->next) {
			prev = current;
			current = prev->next;
			if (current->data == edata)
			{
				break;
			}
		}
		printf("deleting list element : %d\n", current->data);
		if (!current->next) {
			prev->next = NULL;
		}
		else {
			prev->next = current->next;
			free(current);
		}
	}
	return 0;

}

//SEARCH
result_t search_list(list_t *list, data_t sdata) {
	list_t* current = list;
	list_t* prev = NULL;
	int isSuccess = 1;

	if (list)
	{

		if (!(list->next)) {
			printf("List is empty.\n");
			return isSuccess;
		}

		while (current->next) {
			prev = current;
			current = prev->next;
			if (current->data == sdata)
			{
				isSuccess = 0;
				break;
			}
		}
	}
	return isSuccess;
}

//FIND AND REPLACE
result_t find_n_replace(list_t *list, data_t edata, data_t ndata) {
	list_t* current = list;
	list_t* prev = NULL;
	int isSuccess = 1;
	if (list)
	{

		if (!(list->next)) {
			printf("List is empty.\n");
			return 0;
		}

		while (current->next) {
			prev = current;
			current = prev->next;
			if (current->data == edata)
			{
				current->data = ndata;
				isSuccess = 0;
				break;
			}
		}
	}
	return isSuccess;
}

//PRINT
result_t display_list(list_t  *list) {
	if (list) {
		while (list->next != NULL) {
			list = list->next;
			printf("%i\n", list->data);
		}
		return 0;
	}
	return 1;
}

//DESTROY LIST
result_t destroy_list(list_t **p_list) {
	list_t* current = (*p_list);
	list_t* prev = NULL;
	int isSuccess = 1;
	if (current)
	{

		if (!(current->next)) {
			printf("List is empty\n");
			isSuccess = 0;
		}

		while (current->next) {
			prev = current;
			current = prev->next;
			free(prev);
			prev = NULL;
		}
		(*p_list) = NULL;
		printf("List is destroyed\n");

		isSuccess = 0;

	}
	return isSuccess;
}


//MISCALLANEOUS
result_t is_empty(list_t *list) {
	return list->next ? 1 : 0; // 0 for empty and 1 for not empty
}

result_t sort_list(list_t *my_list) {
	list_t* current = my_list->next;
	list_t* tempList = my_list;
	list_t* prev = NULL;
	int max;
	if (!my_list->next) {
		return 0;
	}

	while (tempList ->next) {
		current = tempList->next;
		while (current) {
			if (current->next && (current->data > current->next->data)) {
				int temp = current->data;
				current->data = current->next->data;
				current->next->data = temp;
			}
			current = current->next;
		}
		tempList = tempList->next;
	}

	return 0;
}

len_t length_of_list(list_t *list) {
	int length = 0;
	list_t* temp = list;
	list_t* prev = NULL;

	if (list)
	{
		
		if (!list->next) {
			return 0;
		}

		while (temp->next) {
			temp = temp->next;
			length++;
		}
	}
	return length;
}

result_t merge_list(list_t *src_list, list_t *dest_list) {
	list_t* current = src_list;
	list_t* prev = NULL;
	if (src_list)
	{

		if (!(current->next)) {
			current->next = dest_list->next;
			return 0;
		}

		while (current->next) {
			prev = current;
			current = prev->next;
		}
		current->next = dest_list->next;
	}
	return 0;
}

result_t reverse_list(list_t *list) {
	list_t* current = list;
	list_t* prev = NULL;
	list_t* temp = (list_t*)xcalloc(1, sizeof(list_t));
	temp->next = NULL;

	while (current->next) {
		prev = current;
		current = current->next;
		insert_beg(temp, current->data);
		free(prev);
		prev = NULL;
	}
	//display_list(temp);
	*list = *temp;
	return 0;
}
