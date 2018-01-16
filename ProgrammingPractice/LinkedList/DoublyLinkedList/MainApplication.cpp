#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include "list.h"


int main(void) {
	
	list_t* list =  create_list();
	if (list == NULL) {
		printf("Failed to create the list.\n");
		return EXIT_FAILURE;
	}

	// add node at the beginning
	insert_beg(list, 10);
	insert_beg(list, 160);

	insert_end(list, 11);
	insert_end(list, 12);
	insert_after_data(list, 11, 15);
	insert_after_data(list, 12, 21);
	insert_before_data(list, 11, 18);
	insert_before_data(list, 12, 90);
	delete_end(list);
	delete_beg(list);
	delete_data(list, 11);

	printf("result to search element 1010 is : %d (0 for success and 1 for failure)\n", search_list(list, 1010)); // 0 for success and 1 for failure
	find_n_replace(list, 12, 1011);
	printf("length of list is %d\n", length_of_list(list));
	

	display_list(list);
	printf("--------------Reverse List--------------------------.\n");
	reverse_list(list);
	display_list(list);

	printf("--------------Sort List--------------------------.\n");
	sort_list(list);
	display_list(list);


	printf("--------------Merge List--------------------------.\n");
	list_t* src_list = create_list();
	list_t* dst_list = create_list();
	insert_beg(src_list, 124);
	insert_beg(dst_list, 131);

	merge_list(src_list, dst_list);
	display_list(src_list);
	destroy_list(&src_list);
	printf("--------------After Destroy-------------------------.\n");

	display_list(src_list);



	return EXIT_SUCCESS;
}