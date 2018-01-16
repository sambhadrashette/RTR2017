#ifndef _LIST_H_
#define _LIST_H_

#define TRUE 1
#define FALSE 0
#define DATA_NOT_FOUND 2
#define LIST_EMPTY 3
#define SUCCESS 1
#define FAILURE 0

struct _node;
typedef int data_t;
typedef int result_t;
typedef int len_t;
typedef double dimension;
typedef struct _node list_t;
typedef struct _node node_t;


//node is implementation specific and needs to be modified accordingly
struct _node
{
	data_t data;
	node_t *next;
	node_t *prev;
};

//-----------------------------------------------------User interfaces available------------------------------------------------------------------

//create the list and return back its pointer
extern list_t*  create_list(void);
//---------------------------------------------------------------operations---------------------------------------------------------------------------
//ndata = newdata element sdata = search data element edata = existing data element
//INSERTION
extern result_t insert_beg(list_t *list, data_t ndata);
extern result_t insert_end(list_t *list, data_t ndata);
extern result_t insert_after_data(list_t *list, data_t edata, data_t ndata);	//append
extern result_t insert_before_data(list_t *list, data_t edata, data_t ndata);	//append before

																				//DELETION
result_t delete_end(list_t *list);
result_t delete_beg(list_t *list);
result_t delete_data(list_t *list, data_t edata);

//SEARCH
result_t search_list(list_t *list, data_t sdata);

//FIND AND REPLACE
result_t find_n_replace(list_t *list, data_t edata, data_t ndata);

//PRINT
result_t display_list(list_t  *lst);

//DESTROY LIST
result_t destroy_list(list_t **p_list);


//MISCALLANEOUS
result_t is_empty(list_t *list);

result_t sort_list(list_t *my_list);

len_t length_of_list(list_t *my_list);

result_t merge_list(list_t *src_list, list_t *dest_list);

result_t reverse_list(list_t *list);

#endif






