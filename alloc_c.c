#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
// using word_t = intptr_t;
#define word_t intptr_t

// static intptr_t word_t = intptr_t;

typedef struct Block{
  size_t size;
  bool used;
  struct Block* next;
  word_t data[1];
}Block;

Block* heapStart = NULL;
Block* top = NULL;

// using bitmask to align
static inline size_t align_mem(size_t n){
  return (n+sizeof(word_t)-1) & ~(sizeof(word_t)-1);
}

static inline size_t allocSize(size_t size){
  return size + sizeof(Block) - sizeof(((Block*)0)->data);
}

Block* req_from_os(size_t size){
  Block* block = (Block*)sbrk(0);
  if(sbrk(allocSize(size))==(void*)-1){
    return NULL;
  }
  return block;
}
word_t* alloc_mem(size_t size){
  size = align_mem(size);

  Block* block = req_from_os(size);
  block->size = size;
  block->used = true;

  if(heapStart==NULL){
    heapStart = block; 
  }

  if(top!=NULL){
    top->next = block;
  }

  top = block;

  return block->data;

  // printf("this is size %ld\n",size);
}

Block* getHeader(word_t *data){
  return (Block*)((char*)data + sizeof(((Block*)0)->data)-sizeof(Block));
}


int main(){
  // word_t* word = alloc_mem(5);
  // printf("hello world\n");

  // Aligned to 8 bytes 

  word_t* p1 = alloc_mem(3);
  Block* p1b = getHeader(p1);
  assert(p1b->size==sizeof(word_t));

  word_t* p2 = alloc_mem(8);
  Block* p2b = getHeader(p2);
  assert(p2b->size==8);

  printf("\nAll Assertions Passed\n");


  return 0;
}
