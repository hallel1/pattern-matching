#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "slist.h"
#include "pattern-matching.h"

void printMatch(pm_match_t* matchNode)//private function that print the match
{
	printf("Pattern: %s, start at: %d, ends at: %d, last state = %d\n",
	matchNode->pattern, 
	matchNode->start_pos,
	matchNode->end_pos,
	matchNode->fstate->id);
} 

int initState(pm_state_t *state, pm_int_t id, pm_int_t depth){
//private function that insrert desirable values to the state, Returns 0 on success, -1 on failure.

	slist_t *outputList =(slist_t*)malloc(sizeof(slist_t));
	if(outputList==NULL){
		return -1;
	} 
	slist_t *transitionsList =(slist_t*)malloc(sizeof(slist_t)); 
	if(transitionsList==NULL){
		free(outputList);
		outputList=NULL;
		return -1;
	}
	state->id=id;
	state->depth=depth;
	
	
	slist_init(outputList);
	state->output = outputList;
	
	slist_init(transitionsList);
	state->_transitions =transitionsList;
	state->fail=NULL;


	return 0;
}


pm_labeled_edge_t *initEdge(char ch,pm_state_t *toState){
	//private function that allocation memory for edge and put valus in it.
	pm_labeled_edge_t *edge= (pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));

	if(edge!=NULL){
		edge->label=ch;
		edge->state=toState;
	}
	
	return edge;
}

pm_match_t *initMatchNode(char* str,int end ,pm_state_t* fstate){
	//private function that allocation memory for matchNode and put valus in it.
	pm_match_t *matchNode= (pm_match_t*)malloc(sizeof(pm_match_t));

	if(matchNode!=NULL){
		matchNode->pattern=str;  //the matched pattern
		matchNode->start_pos=end-strlen(str)+1;  //where does it start in the text
		matchNode->end_pos=end; //where does it end in the text
		matchNode->fstate=fstate; //what is the final state


	}

	return matchNode;
}

/* Initializes the fsm parameters (the fsm itself sould be allocated).  Returns 0 on success, -1 on failure. 
*  this function should init zero state
*/
int pm_init(pm_t * pm){
	

	if(pm==NULL)
		return -1;
	pm->zerostate = (pm_state_t*)malloc(sizeof(pm_state_t));
	if(pm->zerostate==NULL){
		return -1;
	}

	if(initState(pm->zerostate,0, 0)==-1)
		return -1;
		
	
	pm->newstate = 1;
	pm->zerostate->fail=NULL;

	return 0;

}

/* Adds a new string to the fsm, given that the string is of length n. 
   Returns 0 on success, -1 on failure.*/
int pm_addstring(pm_t * pm,unsigned char * str, size_t n){
	if(str == NULL || strlen((char*)str)!=n)
		return -1;
	if(n==0)
		return 0;
	if(pm==NULL || pm->zerostate==NULL)
		return -1;
	int i=0;
	pm_state_t *currentState=pm->zerostate;//hold the current state for the set function
	pm_state_t *nextState=pm_goto_get(currentState, str[i]);
	pm_state_t *to_state;//hold the new state for the set function
	pm_int_t depth=1;
	
	while(i<n && nextState!=NULL ){
		
		i++;
		currentState= nextState;
		nextState=pm_goto_get(nextState, str[i]);
		
		depth++;
	}
 
	if(i==n){//if the pattern is beginning of another patterns
	
		if(slist_append(currentState->output,str) ==-1)
			return -1;
		return 0;
	}
	while(i<n){
		
		to_state= (pm_state_t*)malloc(sizeof(pm_state_t));
		if(to_state==NULL){
			return -1;
		}
		
		
		if(initState(to_state, pm->newstate, depth)==-1)
			return -1;
		(pm->newstate)++;
		depth++;
		 
		printf("Allocating state %u\n",to_state->id);
		
		if(pm_goto_set(currentState, str[i], to_state)==-1)
			return -1;
		printf("%u -> %c -> %u\n",currentState->id, str[i],to_state->id);
		currentState=to_state;//promotion current state
		i++;
		
	}
	
	if( slist_append(to_state->output,str) ==-1)
		return -1;
	return 0;
	

}

/* Finalizes construction by setting up the failrue transitions, as
   well as the goto transitions of the zerostate. 
   Returns 0 on success, -1 on failure.*/
int pm_makeFSM(pm_t * pm){
	if(pm==NULL || pm->zerostate==NULL)
		return -1;
	pm_state_t *sttq;//move on list of edge in the queue
	pm_state_t *sNext;//move to the 'son' state
	
	pm_state_t *sttF; //hold sttq.fail or else fail state
	
	slist_node_t *EdgeNodeT = slist_head(pm->zerostate->_transitions);//move on list of edge in the tree
	slist_t *Queue =(slist_t*)malloc(sizeof(slist_t)); 
	pm_labeled_edge_t *theEdge;
	if(Queue==NULL){
		return -1;
	}
	slist_init(Queue);
	
	
	for( ; EdgeNodeT!=NULL; EdgeNodeT = slist_next(EdgeNodeT) ){
		theEdge=slist_data(EdgeNodeT);
		if( slist_append(Queue,theEdge->state) ==-1)
			return -1;
		theEdge->state->fail= pm->zerostate;
	}
	
	while(slist_size(Queue)>0){
		sttq = slist_pop_first(Queue);
		
		EdgeNodeT = slist_head(sttq->_transitions);
	
		for(  ; EdgeNodeT!=NULL; EdgeNodeT = slist_next(EdgeNodeT) ){

			theEdge=slist_data(EdgeNodeT);
			if( slist_append(Queue,theEdge->state) ==-1)
				return -1;

			sttF=sttq->fail;
			sNext= pm_goto_get(sttF, theEdge->label);
			
			while( sNext==NULL){

				if(sttF!= pm->zerostate){
					sttF=sttF->fail;
				}
				
				sNext= pm_goto_get(sttF, theEdge->label);
				if(sttF== pm->zerostate && sNext==NULL ){

					sNext= pm->zerostate;
				}
			}
			theEdge->state->fail = sNext;
			printf("Setting f(%u) = %u\n",theEdge->state->id,theEdge->state->fail->id);
			slist_append_list(theEdge->state->output,sNext->output);
		}
	}
	////now queue is empty
	free(Queue);
	Queue=NULL;
	return 0;
}


/* Set a transition arrow from this from_state, via a symbol, to a
   to_state. will be used in the pm_addstring and pm_makeFSM functions.
   Returns 0 on success, -1 on failure.*/   
int pm_goto_set(pm_state_t *from_state, unsigned char symbol, pm_state_t *to_state){

	if(from_state==NULL || to_state==NULL)
		return -1;
	pm_labeled_edge_t *newEdge;
	
	newEdge=initEdge(symbol,to_state);//create new edge
	if(newEdge==NULL){
		return -1;
	}
	if( slist_append(from_state->_transitions,newEdge) ==-1)
		return -1;		   
	
	return 0;			   
}

/* Returns the transition state.  If no such state exists, returns NULL. 
   will be used in pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions. */
pm_state_t* pm_goto_get(pm_state_t *state, unsigned char symbol){

	if( state==NULL || state->_transitions==NULL)
		return NULL;
		
	slist_node_t *tempEdgeNode = slist_head(state->_transitions);
	pm_labeled_edge_t *theEdge;
	
	while(tempEdgeNode!=NULL){
	
		theEdge=slist_data(tempEdgeNode);
		
		if(symbol==theEdge->label)//compare one char
			return theEdge->state;	
		tempEdgeNode=slist_next(tempEdgeNode);//promotion pointer
	}
	
	
	return 	NULL;
}



/* Search for matches in a string of size n in the FSM. 
   if there are no matches return empty list */
slist_t* pm_fsm_search(pm_state_t * state,unsigned char * ch,size_t n){
	if(state==NULL || ch == NULL || strlen((char*)ch)!=n)
		return NULL;
		
	int i;
	pm_state_t * gotoState;
	pm_match_t* matchNode;
	slist_node_t *curr;
	slist_t *match =(slist_t*)malloc(sizeof(slist_t)); 
	if(match==NULL){
		return NULL;
	}
	slist_init(match);
	
	for(i=0; i<n; i++){
		gotoState=pm_goto_get(state, ch[i]);
		while(state->id!=0 && gotoState==NULL){
			if(state->fail==NULL)//the function pm_makeFSM is not called
				return match;
			state=state->fail;
			gotoState=pm_goto_get(state, ch[i]);
			
		}
		if(gotoState!=NULL){

			state = gotoState;

			for(curr = slist_head(state->output);curr!=NULL;curr= slist_next(curr) ){
				 
				matchNode=initMatchNode(slist_data(curr),i,state);
				if(matchNode==NULL)
					return NULL;	
				if( slist_append(match,matchNode) ==-1)
					return NULL;
				printMatch(matchNode);
			}
			
		}
	}
	
	
	return match;
	
}

void freeState(pm_state_t* state){
//private function - free one state
		slist_destroy(state->output,SLIST_LEAVE_DATA);
		free(state->output);
		state->output=NULL;
		
		slist_destroy(state->_transitions,SLIST_FREE_DATA);//free to edge
		free(state->_transitions);
		state->_transitions=NULL;
		
		state->fail=NULL;
		free(state);
		state=NULL;
}
void destroyRecursion(pm_state_t* state){
//private function - destroy the fsm recursively

	if(slist_size(state->_transitions)==0){
		freeState(state);
		return;
	}
	slist_node_t *curr;
	pm_labeled_edge_t * edgeTemp;
	for(curr = slist_head(state->_transitions); curr!=NULL; curr= slist_next(curr)){
		edgeTemp= slist_data(curr);
		destroyRecursion(edgeTemp -> state);
	}

	freeState(state);

}


/* Destroys the fsm, deallocating memory. */
void pm_destroy(pm_t * pm){
	if(pm==NULL || pm->zerostate==NULL){
		return;
	}
	destroyRecursion(pm->zerostate);


}











