#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PAGESIZE 4096
typedef unsigned char BYTE;

typedef struct chunkhead
{
    int size;
    unsigned int info;
    struct chunkhead *next,*prev;
}   chunkhead;

chunkhead *head = NULL;
int heapsize; 

int x = 0; 
int i = 0;

unsigned char *mymalloc(unsigned int size)
{
    unsigned int sizeNeeded;
    chunkhead *p;
    chunkhead* lastElement;
    chunkhead* array[heapsize];
    
    int sizes[heapsize];
    int index; 
    
    sizeNeeded = size + sizeof(chunkhead);

    if (sizeNeeded % PAGESIZE != 0)
    {
        unsigned int ceilingResult = (sizeNeeded + PAGESIZE - 1) / PAGESIZE;
        unsigned int newSize =  PAGESIZE * ceilingResult; 
        sizeNeeded = newSize; 
    }

    if (heapsize == 0)
    {
        void *p = sbrk(sizeNeeded);
        head = (chunkhead*) p;
        head -> size = sizeNeeded - sizeof(chunkhead);
        head -> info = 1;
        head -> prev = 0;
        head -> next = 0;
        heapsize++; 
        return (BYTE*) p + sizeof(chunkhead);
    }   
    


    for (p = head; p != 0; p = p -> next)
    {   
        lastElement = p;

        if (p -> info == 0 && (sizeNeeded - sizeof(chunkhead)) == p -> size)
        {
            p -> info = 1;
            return (BYTE*) p + sizeof(chunkhead);
        }

        else if (p -> info == 0 && sizeNeeded < p -> size)
        {
            for (i; i < heapsize;)
            {
                array[i] = p;
                sizes[i] = p -> size;
                x = 1;
                i++;
                break;
            } 
        }

    }

    if (x == 1)
    {
        int min = sizes[0];        
        for (int i = 0; i < heapsize; i++) 
        {     
            if(sizes[i] < min)    
                min = sizes[i];    
        }  

        for (int i = 0; i < heapsize; i++) 
        {     
            if(sizes[i] == min)    
                index = i;
                 
        }

        p = array[index]; 
    }
    

    if (p != 0)
    {
        BYTE *newAddress = (BYTE*)p + sizeNeeded;
        chunkhead *newElement = (chunkhead*) newAddress; 

        newElement -> info = 0;
        newElement -> prev = p;
        newElement -> next = p -> next; 
        newElement -> size = p -> size - sizeNeeded;

        p -> info = 1;
        p -> size = sizeNeeded - sizeof(chunkhead);
        p -> next = newElement;
        
        heapsize++;
        return (BYTE*) p + sizeof(chunkhead);
    }


    void *newAddress = sbrk(sizeNeeded);
    chunkhead *newElement = (chunkhead*) newAddress;

    newElement -> info = 1;
    newElement -> prev = lastElement;
    newElement -> next = 0; 
    newElement -> size = sizeNeeded - sizeof(chunkhead);

    lastElement -> next = newElement;
    heapsize++;
    return (BYTE*) newElement + sizeof(chunkhead); 
}   

void myfree(unsigned char *address)
{

    chunkhead *p; 
    unsigned char *destination = address - sizeof(chunkhead);
    
    for (p = head; (unsigned char*)p != destination; p = p -> next);
    
    if (p != head)
    {   
        if (p -> next != 0)
        {
            if (p -> prev -> info == 1 && p -> next -> info == 1)
            {
                p -> info = 0;
            }

            else if (p -> prev -> info == 0 && p -> next -> info == 1)
            {
                p -> prev -> size += p -> size + sizeof(chunkhead);
                p -> prev -> next = p -> next;
                p -> next -> prev = p -> prev; 
                heapsize --;
            }

            else if (p -> prev -> info == 1 && p -> next -> info == 0)
            {
                p -> info = 0;
                p -> size += p -> next -> size + sizeof(chunkhead);
                p -> next -> next -> prev = p;
                p -> next = p -> next -> next;
                heapsize --;
            }

            else if (p -> prev -> info == 0 && p -> next -> info == 0)
            {
                p -> prev -> size += (p -> size + p -> next -> size + 2*(sizeof(chunkhead))); 
                p -> prev -> next = p -> next -> next;
                p -> next -> next -> prev = p -> prev; 
                heapsize -= 2; 

            }
        }

        else 
        {
            if (p -> prev -> info == 0)
            {
                p -> prev -> size += p -> size + sizeof(chunkhead);
                p -> prev -> next = p -> next; 
            }

            else 
            {
                p -> prev -> next = 0;
                sbrk(-(p -> size + sizeof(chunkhead)));
            }

            heapsize --;
            
        }
        
    }

    else 
    {
        p -> info = 0;
        
        if (heapsize != 1)
        {
            if (p -> next -> info == 0)
            {
                p -> size += p -> next -> size + sizeof(chunkhead);
                p -> next -> next -> prev = p;
                p -> next = p -> next -> next;
                heapsize--;
            }
            
        }
    }
    
    if (heapsize == 0)
    {
        sbrk(-(head -> size + sizeof(chunkhead)));
    }
    
}

void analyze()
{

    printf("\n--------------------------------------------------------------\n");
    
    if(!head)
    {
        printf("no heap");
        return;
    }

    chunkhead* ch = (chunkhead*)head;
    
    for (int no=0; ch; ch = (chunkhead*)ch->next,no++)
    {
        printf("%d | current addr: %p |", no, ch);
        printf("size: %d | ", ch->size);
        printf("info: %d | ", ch->info);
        printf("next: %p | ", ch->next);
        printf("prev: %p", ch->prev);
        printf("      \n");
    }

    printf("program break on address: %p\n",sbrk(0));

}

int main()
{
    heapsize = 0; 
    unsigned char *a,*b,*c,*d,*e;

    a = mymalloc(9000); 
    b = mymalloc(5000);
    c = mymalloc(5000);
    d = mymalloc(5000);
    myfree(a);
    myfree(c);
    e = mymalloc(1000);
    analyze();
    //myfree(a);
    
    return 0;
 
}