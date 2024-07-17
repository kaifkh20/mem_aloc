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
Block* top;

// using bitmask to align
static inline size_t align_mem(size_t n){
  return (n+sizeof(word_t)-1) & ~(sizeof(word_t)-1);
}

static inline size_t allocSize(size_t size){
  return size + sizeof(Block) - sizeof(((Block*)0)->data);
}

word_t* alloc_mem(size_t size){
  size = align_mem(size);
  printf("this is size %ld\n",size);
}

Block* req_from_os(size_t size){
  Block* block = (Block*)sbrk(0);
  if(sbrk(allocSize(size))==(void*)-1){
    return NULL;
  }
  return block;
}

int main(){
  word_t* word = alloc_mem(5);
  printf("hello world\n");
  return 0;
}
