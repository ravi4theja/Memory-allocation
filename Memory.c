#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#define total 67108864

void *allocate (size_t request, size_t *info);
int deallocate (void *ptr);
size_t heap_max();
bool chkMerge(void *ptr);
int chkErr(void *ptr);
bool chkFree(void *ptr);
bool chkIsalloc(void *ptr);

typedef struct alloc_hdr {
	unsigned int chunk_size;
	unsigned int alloc_size;
	struct alloc_hdr *allochdr_next;
} allochdr;

size_t sofall = sizeof(allochdr);
allochdr heap[4194304];
allochdr *fl = heap;

void *chkPtr = heap;
void *returnptr;
bool init = true;
int merged = 0;
void *reptr;





void *allocate (size_t request, size_t *info)
{	
	if(init == true)
	{	
		if(request <= 67108864)
		{		
			allochdr *temp = fl;
			(temp-1)->chunk_size = request + sofall;
			(temp-1)->alloc_size = request;
			(temp-1)->allochdr_next = NULL;
			returnptr = temp - 1;
			fl += (request / sofall) + 1;
			fl->chunk_size = 67108864 - ((1 + (request/sofall) + 1) * sofall); // struct of the sent allocation until the end of the free chunk's struct
			fl->allochdr_next = NULL;
		
			init = false;
			return returnptr + sofall;
		}
		else
			return NULL;
	}
	else
	{	
		allochdr *temp = fl;
		while(temp != NULL)
		{
			if(temp->chunk_size >= request)
			{
				
				void *tmp = temp;
				allochdr *mid = temp->allochdr_next;
				unsigned int prevSize = temp->chunk_size;
				if(temp->chunk_size > ((request / 16) + 1) * 16 + 16)			//break check
				{
					
					temp->chunk_size = request + sofall;
					temp->alloc_size = request;
					*info = ((request / 16) + 1) * 16;
					temp->allochdr_next = NULL;
					
					temp = temp + 1 + (request / 16) + 1;							// breaking
					temp->allochdr_next = mid;
					temp->chunk_size = prevSize - ((request / 16) + 1 + 1) * 16;
					fl = temp;
				}
				else
				{
					temp->chunk_size = request + sofall;
					temp->alloc_size = request;
					*info = ((request / 16) + 1) * 16;
					temp->allochdr_next = NULL;
					
					temp = temp->allochdr_next;
					fl = temp;
				}				
				return tmp + sofall;
			}
			else
			{
				temp = temp->allochdr_next;
			}
		}
		*info = 0;
		return NULL;
	}
}

int deallocate(void *ptr)
{
	int err = chkErr(ptr);
	if(err == -1)
		return -1;
	else if(err == -2)
		return -2;
	else if(err == -3)
		return -3;
	else if(err == 0)
	{	
		bool chk = chkMerge(ptr);
		if(!chk)
		{
			//test = ptr - sofall;
			allochdr *tmp = ptr - sofall;
			unsigned int tmp_free = (((tmp->alloc_size)/16) + 1) * 16;
			
			allochdr *temp = fl;
			
			fl = tmp;
			fl->chunk_size = tmp_free;
			fl->allochdr_next = temp;
		}
		forMerge();
		return 0;
	}
}


int chkErr(void *ptr)
{
	
	if(ptr < chkPtr || ptr > (chkPtr + 67108864))
		return -1;
	else if (chkFree(ptr))
		return -2;
	else if (chkIsalloc(ptr))
		return -3;
	else 
		return 0;
}

bool chkFree(void *ptr)
{
	allochdr *tmp = ptr - sofall;
	allochdr *temp = fl;
	for(;temp != NULL; temp= temp->allochdr_next)
	{
		if (tmp == temp)
			return true;
	}
	return false;
}

bool chkIsalloc(void *ptr)
{
	allochdr *tmp = ptr - sofall;
	if(tmp->allochdr_next == NULL)
		return false;
	else
		return true;
}

bool chkMerge(void *ptr)						
{	
	merged = 0;
	allochdr *tmp = ptr - sofall;
	allochdr *temp = fl;
	for(;temp != NULL ; temp = temp->allochdr_next)			// Merging backwards
	{
		if(temp + 1 + ((temp->chunk_size) / 16) == tmp)
		{
			temp->chunk_size += sofall + (((tmp->alloc_size)/16) + 1) * 16;
			return true;
		}
	}
	return false;
}

forMerge()
{
	allochdr *temp = fl;
	for(;temp != NULL; temp = temp->allochdr_next)			// Merge forward
	{
		if((temp + 1 + ((temp->chunk_size) / 16)) == temp->allochdr_next)
		{
			allochdr *mid = temp->allochdr_next;
			temp->allochdr_next = mid->allochdr_next;
			temp->chunk_size = temp->chunk_size + mid->chunk_size + 16;
		}
	}
}
size_t heap_max()
{
	unsigned int max = 0;
	if(init)
		return 67108864;
	else
	{
		allochdr *temp = fl;
		for(;temp != NULL; temp = temp->allochdr_next)
		{
			if(temp->chunk_size > max)
			{
				max = temp->chunk_size;
			}
		}
		return max;
	}
}



