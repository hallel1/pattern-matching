#include "slist.h"
#include <stdio.h>
#include <stdlib.h>


//private function that create node and insrert his values:
slist_node_t* createNode(void * data, slist_node_t* next){

	slist_node_t* node = (slist_node_t *)malloc(sizeof(slist_node_t));
	if(node!=NULL){
		slist_data(node)= data;
		slist_next(node)=next;
	}
	return node;
}

//private function that create node and insrert his values:
slist_node_t* createNode(void * data, slist_node_t* next){

	slist_node_t* node = (slist_node_t *)malloc(sizeof(slist_node_t));
	if(node!=NULL){
		slist_data(node)= data;
		slist_next(node)=next;
	}
	return node;
}

//-----------------------------------------------------------------------
/** Initialize a single linked list
	\param list - the list to initialize */
void slist_init(slist_t * l){
	if(l==NULL)
		return;
	slist_head(l)=NULL;
	slist_tail(l)=NULL;
	slist_size(l)=0;
}
//-----------------------------------------------------------------------

/** Destroy and de-allocate the memory hold by a list
	\param list - a pointer to an existing list
	\param dealloc flag that indicates whether stored data should also be de-allocated */

void slist_destroy(slist_t * l,slist_destroy_t do_free){


	void * data;
	if(l==NULL)
		return;
	while(slist_head(l)!=NULL){
	
		data =slist_pop_first(l);

		if(do_free == SLIST_FREE_DATA && data!=NULL)
			free(data);//free to the node and premotes the head

	}
	data =NULL;
	


}
//-----------------------------------------------------------------------
/** Append data to list (add as last node of the list)
	\param list - a pointer to a list
	\param data - the data to place in the list
	\return 0 on success, or -1 on failure */
int slist_append(slist_t * l,void * data){
	if(l==NULL)
		return -1;
	slist_node_t* newNode= createNode(data, NULL);
	if(newNode==NULL)
		return -1;
	slist_node_t * nodeTemp;
	
	if(slist_size(l)==0){
		slist_head(l)=newNode;
		slist_tail(l)=newNode;
	}else{
		nodeTemp = slist_tail(l);//pointer to the tail of the list
		slist_next(nodeTemp)=newNode;
		slist_tail(l)=slist_next(nodeTemp);
	
	}

	
	slist_size(l)++;//increase list size
	
	return 0;	
	
	
}
//-----------------------------------------------------------------------

/** Prepend data to list (add as first node of the list)
	\param list - a pointer to list
	\param data - the data to place in the list
	\return 0 on success, or -1 on failure
*/
int slist_prepend(slist_t * l,void * data){
	if(l==NULL)
		return -1;
		
	slist_node_t* newNode= createNode(data, slist_head(l));
	if(newNode==NULL)
		return -1;
		
	slist_head(l)=newNode;
	if(slist_size(l)==0)
		slist_tail(l)=newNode;

	
	slist_size(l)++;//increase list size
	return 0;	
	
	
}
//-----------------------------------------------------------------------
/** Pop the first element in the list
	\param list - a pointer to a list
	\return a pointer to the data of the element, or NULL if the list is empty */
void *slist_pop_first(slist_t * l){

	if(l==NULL || slist_head(l)==NULL)
		return NULL;
	slist_node_t * nodeTemp= slist_head(l);
	void * data = slist_data(nodeTemp);
	slist_head(l)=slist_next(slist_head(l));
	free(nodeTemp);
	nodeTemp=NULL;
	if(slist_head(l)==NULL)
		slist_tail(l)=NULL;
	slist_size(l)--;
	return data;
}
//-----------------------------------------------------------------------

/** \brief Append elements from the second list to the first list, use the slist_append function.
	you can assume that the data of the lists were not allocated and thus should not be deallocated in destroy 
	(the destroy for these lists will use the SLIST_LEAVE_DATA flag)
	\param to a pointer to the destination list
	\param from a pointer to the source list
	\return 0 on success, or -1 on failure
*/
int slist_append_list(slist_t* l1, slist_t* l2){
		
		if(l1==NULL)
			return -1;
		if(l2==NULL)
			return -1;
		slist_node_t * nodeTemp2= slist_head(l2);//temp pointer for l2
		while(nodeTemp2!=NULL){
			if(slist_append(l1,slist_data(nodeTemp2))==-1)
				return -1;
			nodeTemp2=slist_next(nodeTemp2);
		}

	
	return 0;
	
	

}


















