/*
 File: my_allocator.cpp
 */
#include "BuddyAllocator.h"
#include <iostream>
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
    //create initial memory block
    
    total_memory_length = _total_memory_length;
    basic_block_size= _basic_block_size;
    myArray.resize((log2 (_total_memory_length)) - (log2 (_basic_block_size)) + 1);
    
    //vector<LinkedList>
    
    memBlock = new char[_total_memory_length];
    
    //create header part of memory block
    BlockHeader* x = (BlockHeader*) memBlock;
    
    //assign header parts variables
    x->blockSize = _total_memory_length;
    x->isUsed = false;
    x->next = NULL;
    
    for(int i= 0; i < myArray.size();i++){
        myArray[i] = new LinkedList();
    }
    
    //inserting block into Linked list
    myArray[0]->insert(x);
    
}

BuddyAllocator::~BuddyAllocator (){
    for(int i = 0; i < myArray.size(); i++){
        delete myArray[i];
    }
}

char* BuddyAllocator::alloc(uint _length) {
    /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
     */
    int requiredMemory = _length + sizeof(BlockHeader);
    int closestBlock = pow(2, ceil(log2 (requiredMemory)));
    if(closestBlock < basic_block_size){closestBlock = basic_block_size;}
    int index = (log2 (total_memory_length)) - (log2 (closestBlock));
    int walk = index - 1;
    
    BlockHeader* tempHead = myArray[index]->getHead();
    if(tempHead != NULL){
        myArray[index]->remove(tempHead);
        return ((char*) tempHead + sizeof(BlockHeader));
    }
    
    while(myArray[walk]->getHead() == NULL){
        walk--;
    }
    
    while(walk < index){
        
        char* correctIndex = split((char*) myArray[walk]->getHead());
        walk++;
    }
    tempHead = myArray[index]->getHead();
    myArray[index]->remove(tempHead);
    return (char*) tempHead + sizeof(BlockHeader);
}

int BuddyAllocator::free(char* _a) {
    /* Same here! */
    BlockHeader* x = (BlockHeader*) (_a - sizeof(BlockHeader));
    int index = (log2 (x->blockSize)) - (log2 (basic_block_size));
    char* buddy = getbuddy(_a - sizeof(BlockHeader));
    BlockHeader* z = (BlockHeader*) buddy;
    char* spaghetti = _a - sizeof(BlockHeader);
    while(arebuddies(spaghetti, buddy) && z->blockSize != total_memory_length ){
        spaghetti =   merge(spaghetti, buddy);
        BlockHeader* temporary = (BlockHeader*) spaghetti;
        if(temporary->blockSize == total_memory_length){break;}
        buddy = getbuddy(spaghetti);
        z = (BlockHeader*) buddy;
    }
    return 0;
}

void BuddyAllocator::debug (){
    
}


char* BuddyAllocator::split (char* block){
    
    BlockHeader* x = (BlockHeader*) block;
    char* a = ((char*) block) + (x->blockSize / 2);
    BlockHeader* b = (BlockHeader*) a;
    int temp = x->blockSize;
    x->blockSize = (temp) / 2;
    b->blockSize = (temp) / 2;
    b->isUsed = false;
    
    int index = (log2 (total_memory_length)) - (log2 (temp));
    
    
    
    myArray[index]->remove(x);
    myArray[index+1]->insert(x);
    myArray[index+1]->insert(b);
    
    return (char*) block;
    
}



char* BuddyAllocator::merge (char* block1, char* block2){
    BlockHeader* y = (BlockHeader*) block1;
    BlockHeader* z = (BlockHeader*) block2;
    int temp = y->blockSize;
    int index = (log2 (total_memory_length)) - (log2 (temp));
    if(getbuddy(block1) == block2){
        myArray[index]->remove(y);
        myArray[index]->remove(z);
        if(block1<block2){
            myArray[index-1]->insert(y);
            y->blockSize = temp*2;
            return (char*) y;
        } else{
            myArray[index-1]->insert(z);
            z->blockSize = temp*2;
            return (char*) z;
        }
        
    } else{
        return NULL;
    }
    
}
