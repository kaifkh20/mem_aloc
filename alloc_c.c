#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
// using word_t = intptr_t;
#define word_t intptr_t

typedef enum{
  FirstFit,
  NextFit,
  BestFit
}SearchMode;


typedef struct Block{
  size_t size;
  bool used;
  struct Block* next;
  word_t data[1];
}Block;


Block* heapStart = NULL;
Block* top = NULL;
Block* searchStart = NULL;

SearchMode searchMode = FirstFit;


void reset_heap(){
  if(heapStart==NULL) return;

  brk(heapStart);
  heapStart = NULL;
  top = NULL;
  searchStart = NULL;
}

void init( SearchMode mode){
  searchMode = mode;
  reset_heap();
}

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

Block* splitBlock(Block* block,size_t size){

  // printf("reaching here");

    Block* block1 = block;
    
    Block* block2;
    block1->next = block2;
    block1->size = size;

    Block* next_pointed_to_by_block = block->next;
    block2->next = next_pointed_to_by_block;
    block2->size = abs(block->size-size);
    block2->used = false;

    return block1;
}

static inline bool canSplit(Block* block,size_t size){
  if(block!=NULL && block->size>size){
    return true;
  }else false;
}

Block* listAllBlock(Block* block,size_t size){
  if(block==NULL) return NULL;
  if(canSplit(block,size)){
    block = splitBlock(block,size);
  }
  block->used = true;
  block->size = size;

  return block;
}

// First fit algo

Block* first_fit(size_t size){
  Block* block = heapStart;
  while(block!=NULL){
    if(block->used || block->size < size){
      block=block->next;
      continue;
    }
    return block;
  }

  return NULL;
}

Block* next_fit(size_t size){
  searchStart = heapStart;
  Block* block = searchStart;
  while(block!=NULL){
    if(block->used || block->size<size){
      block = block->next;
      continue;
    }
    searchStart = block;
    return block;
  }
  return NULL;
}

Block* best_fit(size_t size){
  searchStart = heapStart;
  Block* block = searchStart;
  
  Block* min_block = NULL;
  uint64_t min_size = UINT64_MAX;

  while(block!=NULL){
    // printf("reaching here");
    if(block->used || block->size < size){
      block = block->next;
      continue;
    }

    if(!block->used && min_size>block->size){
        min_size = block->size;
        min_block = block;
        block = block->next;
    }

  }

  // printf("block : %ld ",min_block->size);
  // free(block);
  return min_block;  
}

Block* findBlock(size_t size){
  switch (searchMode) 
  {
  case FirstFit:
    return first_fit(size);
  case NextFit:
    return next_fit(size); 
  case BestFit:
    return best_fit(size);
  default:
    break;
  }


  // return first_fit(size);
}



word_t* alloc_mem(size_t size){
  size = align_mem(size);

  Block* block = findBlock(size);
  // printf("calling split");
  // block = listAllBlock(block,size);

  if(block!=NULL){
    return block->data;
  }

  block = req_from_os(size);
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

static inline bool canMerge(Block *block){
  return block->next && !block->next->used;
}

// Merger adjacent block

Block* merge(Block* block){
    Block* block1 = block;
    Block* block2 = block->next;

    Block* pointed_to_by_block2 = block2->next;

    Block* block3;

    size_t size_block3 = block1->size + block2->size;

    block3->size = size_block3;
    block3->next = pointed_to_by_block2;
    
    return block3;
}


void free_mem(word_t* data){
  Block* block = getHeader(data);
  if(canMerge(block)){
    block = merge(block);
  }
  block->used = false;
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

// Freeing mem(setting block->used as false)

  free_mem(p2);
  assert(p2b->used==false);

// First fit

  word_t* p3 = alloc_mem(8);
  Block* p3b = getHeader(p3);
  assert(p3b->size==8);
  assert(p3b==p2b);

// Next Fit
  init(NextFit);
  alloc_mem(8);
  alloc_mem(8);
  alloc_mem(8);

  word_t* o1 = alloc_mem(16);
  word_t* o2 = alloc_mem(16);

  free_mem(o1);
  free_mem(o2);

  word_t* o3 = alloc_mem(16);
  assert(searchStart==getHeader(o3));


// Best Fit
  init(BestFit);
  alloc_mem(8);
  word_t* z1 = alloc_mem(64);
  alloc_mem(8);
  word_t* z2 = alloc_mem(16);

  free_mem(z2);
  free_mem(z1);

  // printf("%ld",getHeader(z1)->size);

  word_t* z3 = alloc_mem(16);

  // printf("block size : %ld",getHeader(z3)->size);

  assert(getHeader(z3)==getHeader(z2));
  
  // z3 = alloc_mem(16);
  // assert(getHeader(z3) == getHeader(z1));
  
  printf("\nAll Assertions Passed\n");


  return 0;
}
