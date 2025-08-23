#include "clist_ts.h"
#include "clogger.h"
#include <stdlib.h>
#include <stdio.h>


void priv_CListTS__remove_element_and_shift(CObject **array, int index, int array_length);
void priv_CListTS__destroy(CObject* self);
void priv_CListTS__remove(CListTS* self, int index);
CObject * priv_CListTS__get(CListTS * self, int index);

void priv_CListTS__remove_element_and_shift(CObject **array, int index, int array_length){
    int i;
    for(i = index; i < array_length-1; i++) {
        array[i] = array[i + 1];
    }
}

void priv_CListTS__destroy(CObject * self){
    CListTS * list = (CListTS*) self;
    if(list->destroy_callback){
        list->destroy_callback(list);
    }
    CListTS__clear(list);
    P_MUTEX_CLEANUP(list->lock);
    free(list->data);
    list->data = NULL;
}

void priv_CListTS__remove(CListTS* self, int index){
    //Remove element and shift content
    priv_CListTS__remove_element_and_shift(self->data, index, self->count);  /* First shift the elements, then reallocate */
    //Update counter
    self->count--;
    //Resize array memory
    if(self->count > 0){
        self->data = realloc (self->data,sizeof(void *)*self->count);
    } else {
        free(self->data);
        self->data = NULL;
    }
}

CObject * priv_CListTS__get(CListTS * self, int index){
    if(index >= self->count){
        C_ERROR("Index out of bounds [%d]",index);
        return NULL;
    }
    return self->data[index];
}

CListTS * CListTS__create(){
    CListTS * list = malloc(sizeof(CListTS));
    CListTS__init(list);
    CObject__set_allocated((CObject*)list);
    return list;
}

void CListTS__init(CListTS* self){
    CObject__init((CObject*)self);
    CObject__set_destroy_callback((CObject*)self,priv_CListTS__destroy);

    P_MUTEX_SETUP(self->lock);
    self->destroy_callback = NULL;
    self->count = 0;
    self->data = NULL;
}

void CListTS__add(CListTS* self, CObject * record){
    P_MUTEX_LOCK(self->lock);
    if(!self->data)
        self->data = malloc(sizeof (void *));
    else
        self->data = realloc (self->data,sizeof (void *) * (self->count+1));
    self->data[self->count]=record;
    //Trigger event_count inrrement
    self->count++;
    P_MUTEX_UNLOCK(self->lock);
}

void CListTS__remove(CListTS* self, int index){
    P_MUTEX_LOCK(self->lock);
    priv_CListTS__remove(self,index);
    P_MUTEX_UNLOCK(self->lock);
}

CObject * CListTS__get(CListTS * self, int index){
    CObject * item;
    P_MUTEX_LOCK(self->lock);
    item = priv_CListTS__get(self,index);
    P_MUTEX_UNLOCK(self->lock);
    return item;
}

CObject * CListTS__pop(CListTS * self){
    if(CListTS__get_count(self) == 0){
        return NULL;
    }
    CObject * item;
    P_MUTEX_LOCK(self->lock);
    item = priv_CListTS__get(self, 0);
    priv_CListTS__remove(self, 0);
    P_MUTEX_UNLOCK(self->lock);
    return item;
}

void CListTS__clear(CListTS* self){
    if(self){
        P_MUTEX_LOCK(self->lock);
        int i;
        for(i=0; i < self->count; i++){
            CObject__destroy(self->data[i]);
        }
        self->count = 0;
        free(self->data);
        self->data = NULL;
        P_MUTEX_UNLOCK(self->lock);
    }
}

int CListTS__get_count(CListTS * self){
    int ret;
    P_MUTEX_LOCK(self->lock);
    ret = self->count;
    P_MUTEX_UNLOCK(self->lock);
    return ret;
}

void CListTS__set_destroy_callback(CListTS* self, void (*destroy_callback)(CListTS *)){
    self->destroy_callback = destroy_callback;
}

int CListTS__destroy_record(CListTS * self, CObject * record){
    int found = CListTS__remove_record(self,record);
    if(found < 0){
        C_ERROR("---------------------\n");
        C_ERROR("| ERROR record not found for clean up %i\n", found);
        C_ERROR("---------------------\n");
    } else {
        CObject__destroy(record);
    }
    return found;
}

int CListTS__remove_record(CListTS * self, CObject * record){
    P_MUTEX_LOCK(self->lock);
    int found = -1; 

    int i;
    for(i=0; i < self->count; i++) {
        if(found < 0 && record == self->data[i]){
            found=i;
        }

        if(found > -1){
            if(i == self->count-1){
                break;
            }
            self->data[i] = self->data[i + 1];
        }
    }

    if(found > -1){
        self->count--;
    }

    if(found < 0){
        C_ERROR("---------------------\n");
        C_ERROR("| ERROR record not found %i\n", found);
        C_ERROR("---------------------\n");
    }
    P_MUTEX_UNLOCK(self->lock);
    return found;
}
