#ifndef FREE_LIST
#define FREE_LIST

#include <stdio.h> 
#include <stdlib.h> 


typedef struct Block Block;

// doubly linked list node template 
struct node { 
	Block* data; 
	struct node* next; 
	struct node* prev; 
}; 

struct free_list{
  struct node* head;
  size_t size;
  struct node* tail;
}; 

// creating a pointer to head and tail of the linked list 
struct node* head = NULL; 
struct node* tail = NULL; 

// create a new node with the given data and return a 
// pointer to it 
struct node* create_node(Block* data) 
{ 
	struct node* new_node 
		= (struct node*)malloc(sizeof(struct node)); 
	new_node->data = data; 
	new_node->next = NULL; 
	new_node->prev = NULL; 
	return new_node; 
} 

struct free_list init_freelist(){
  struct free_list fl;
  fl.head = head;
  fl.size = 0;
  fl.tail = tail;
  return fl;
}

// // insert a node at the beginning of the list 
// void insert_at_head(Block* data) 
// { 
// 	struct node* new_node = create_node(data); 
// 	if (head == NULL) { 
// 		head = new_node; 
// 		tail = new_node; 
// 	} 
// 	else { 
// 		new_node->next = head; 
// 		head->prev = new_node; 
// 		head = new_node; 
// 	} 
// } 
//
// // // insert a node at the end of the list 
// void insert_at_tail(Block* data) 
// { 
// 	struct node* new_node = create_node(data); 
// 	if (tail == NULL) { 
// 		head = new_node; 
// 		tail = new_node; 
// 	} 
// 	else { 
// 		new_node->prev = tail; 
// 		tail->next = new_node; 
// 		tail = new_node; 
// 	} 
// } 

void insert_into_freelist(Block* data,struct free_list* fl){
    // insert_at_head(data);
	

    struct node* new_node = create_node(data);
    struct node* temp_node = head;


    while(temp_node!=NULL && temp_node->next!=NULL){
        temp_node = temp_node->next;   
    }

    if (temp_node!=NULL) temp_node->next = new_node;
    new_node->prev = temp_node;
    new_node->next = NULL;
  
    if(head==NULL || fl->size==0){
      head = new_node;
    } 

    tail = new_node;

    fl->head = head;
    fl->tail = tail;
    fl->size = fl->size+1;

	// printf("data size of head %ld",fl->head->datasize);
}

void remove_from_freelist(Block* data,struct free_list* fl){
//   printf("removed from freelist");
	 
//   printf("%ld",data->size);
  
  if(fl->size==0) return ;

  struct node* temp_node = head;
  
  while(temp_node!=NULL && temp_node->data!=data){
	temp_node=temp_node->next;
  }

  if(temp_node==NULL){
	printf("No such element in free list\n");
	exit(EXIT_FAILURE);
  }

  if(temp_node->data==data){
    
    if(temp_node==tail){
      tail = temp_node->prev;
    }
    if(temp_node==head){
      head = temp_node->next;
    //   head->next = NULL;
    }
    struct node* prev_node = temp_node->prev;
    struct node* next_node = temp_node->next;
    
    temp_node->prev = NULL;
    temp_node->next = NULL;

    if(prev_node!=NULL) prev_node->next = next_node;
    if(next_node!=NULL) next_node->prev = prev_node;
  }
  
  if(fl->size>0)fl->size-=1;
  fl->head = (fl->size==0)?NULL:head;
  fl->tail = (fl->size==0)?NULL:tail;

}

#endif
