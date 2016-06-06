#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

void add_root(void *addr, size_t len);
void del_root(void *addr);
void *allocate(size_t request, size_t *collected);
size_t heap_max();
void *garbageCollect();
void* wordAllign(void *ptr);

double heap1[8388608] = {0};
double heap2[8388608] = {0};

void* hp1 = heap1;
void* hp2 = heap2;

void *heapBegins = heap1;
void *ptr = heap1;

void *ptr2 = heap2;

void *scan = heap2;
bool init = true;
size_t liveObj = 0;



typedef struct root_data 
{
	void **rAddr;
	unsigned int size;
} rData;


typedef struct alloc_data
{
	void *start;
	unsigned int size;
	void *new;
	struct alloc_data *next;
} alloc;


alloc *ch;
alloc *ch2 = NULL;
rData roots[1024];
int i = 0;        // i is a global variable indicating the no. of roots

void add_root(void *addr, size_t len)
{
	roots[i].rAddr = addr;
	roots[i].size = len;
	i++;
}

void del_root(void *addr)
{
	int j = 0;
	for(j = 0; j < i; j++)
	{
		if(roots[j].rAddr == addr)
			roots[j].size = 0;
	}
}

void *allocate(size_t request, size_t *collected)
{
	if(init == true)
	{
		ptr = wordAllign(ptr);
		alloc *temp = ptr - 32;
		temp -> start = ptr;
		temp -> new = ptr;
		temp->size = request;
		temp->next = NULL;
		ch = temp;
		
		void *tmp = ptr;
		ptr = ptr + request;
		init = false;
		*collected = 0;
		return tmp;
	}
	else
	{
		ptr = wordAllign(ptr);
		if((heapBegins + 67108864) - (ptr + 32) >= request)
		{
			alloc *temp = ptr;
			temp->start = ptr + 32;
			temp -> new = ptr + 32;
			temp->size = request;
			temp -> next = ch;
			ch = temp;
			
			void *tmp = ptr + 32;
			ptr = ptr + 32 + request;
			*collected = 0;
			return tmp;
		}
		else				// collection starts here with roots 
		{
			int j = 0;
			for(j = 0; j < i; j++)
			{
				if(roots[j].size != 0)
				{
					alloc *temp = *(roots[j].rAddr);
					alloc *tmp = ptr2;
					void *tmp2 = *(roots[j].rAddr) - 32;
					ptr2 = ptr2 - 32;
					while(tmp2 < *roots[j].rAddr + roots[j].size)
					{
						//*ptr2 = **roots[j].rAddr;
						
						memcpy(ptr2, tmp2, 1);
						ptr2++;
						tmp2++;
					}
					liveObj++;
					*(roots[j].rAddr) = ptr2 - roots[j].size;
					//change values in the struct before roots
					
					temp = temp - 1;
					temp->new = tmp;
					tmp = tmp - 1;
					tmp->start = ptr2 - roots[j].size;
					tmp->new = ptr2 - roots[j].size;
					tmp->next = ch2;
					ch2 = tmp;
					ptr2 = ptr2 + 32;  //increasing the ptr2 to accommodate struct in to heap
				}
			}
			ptr = garbageCollect();
			ptr2 = hp2;   //swapping the heaps
			scan = hp2;
			heapBegins = hp1;
			ch = ch2;
			ch2 = NULL;
			ptr = wordAllign(ptr);
			if((heapBegins + 67108864) - (ptr + 32) >= request)
			{
				alloc *temp = ptr;
				temp->start = ptr + 32;
				temp -> new = ptr + 32;
				temp->size = request;
				temp -> next = ch;
				ch = temp;
			
				void *tmp = ptr + 32;
				ptr = ptr + 32 + request;
				*collected = liveObj;
				liveObj = 0;
				return tmp;
			}
			else
			{
				*collected = 0;
				return NULL;
			}
			
			
			
			
		}
	}
}


void *garbageCollect()
{
	while(scan != ptr2)
	{
		alloc *temp = ch;
		void **tmp = scan;
		for(;temp != NULL; temp = temp->next) //remember to add breaks inside this for loop
		{
			if(temp->start == *tmp)
			{
				if(temp->new == temp->start) //if we have to copy
				{
					void *cp = temp->start - 32;
					ptr2 = ptr2 - 32;
					alloc *tmp2 = ptr2;
					while(cp < temp->start + temp->size)
					{
						memcpy(ptr2, cp, 1);
						ptr2++;
						cp++;
					}
					liveObj++;
					temp->new = ptr2 - temp->size; 			// updating struct in from heap
					*tmp = ptr2 - temp->size;  				// updating the pointer in to heap
					tmp2->start = ptr2 - temp->size;		// updating the struct in to heap
					tmp2->new = ptr2 - temp->size;
					tmp2->next = ch2;
					ch2 = tmp2;
					ptr2 = ptr2 + 32;
					break;
				}
				else			// if it's already copied and we only have to update the address value
				{
					*tmp = temp->new;
					break;
				}
			}
			else if(*tmp - temp->start > 0)
			{
				if(*tmp < (temp->start + temp->size))
				{
					if(temp->new == temp->start)
					{
						void *cp = temp->start - 32;
						ptr2 = ptr2 - 32;
						alloc *tmp2 = ptr2;
						while(cp < temp->start + temp->size)
						{
							memcpy(ptr2, cp, 1);
							ptr2++;
							cp++;
						}
						liveObj++;
						temp->new = ptr2 - temp->size;
						*tmp = temp->new + (*tmp - temp->start);
						tmp2->start = ptr2 - temp->size;		// updating the struct in to heap
						tmp2->new = ptr2 - temp->size;
						tmp2->next = ch2;
						ch2 = tmp2;
						ptr2 = ptr2 + 32;
						break;
					
					}
					else
					{
						*tmp = temp->new + (*tmp - temp->start);
						break;
					}
				}
			}
		}
		scan++;
	}
	double *swap = hp1;
	hp1 = hp2;
	hp2 = swap;
	
	
	return scan;
		

}

void* wordAllign(void *p)
{
	while(((uintptr_t)p % 8) != 0)
		p++;
	return p;
}


size_t heap_max()
{
	return ((hp1 + 67108864) - (ptr + 32));
}







	
