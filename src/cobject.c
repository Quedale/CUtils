#include "cobject.h"
#include <stdlib.h>
#include <stdio.h>

void CObject__set_destroyed(CObject * self){
    if(self){
        P_MUTEX_LOCK(self->ref_lock);
        self->destroyed = 1;
        P_MUTEX_UNLOCK(self->ref_lock);
    }
}

void CObject__set_destroy_callback(CObject* self, void (*destroy_callback)(CObject *)){
    self->destroy_callback = destroy_callback;
}

void CObject__init(CObject* self) {
    self->refcount = 1;
    self->destroyed = 0;
    self->allocated = 0;
    self->destroy_callback = NULL;
    P_MUTEX_SETUP(self->ref_lock);
}

void CObject__set_allocated(CObject * self){
    if(self){
        self->allocated = 1;
    }
}

CObject * CObject__create(){
    CObject* result = (CObject*) malloc(sizeof(CObject));
    CObject__init(result);
    result->allocated = 1;
    return result;
}

void CObject__destroy(CObject* self) {
    if (self && CObject__ref_count(self) == 0) {
        if(self->destroy_callback){
            self->destroy_callback(self);
        }
        P_MUTEX_CLEANUP(self->ref_lock);
        if(self->allocated){
            self->allocated = 0;
            free(self);
        }
    } else if(self){
        CObject__set_destroyed(self);
        CObject__unref((CObject*)self);
    }
}

int CObject__addref(CObject *self){
    if(CObject__is_valid((CObject*)self)){
        P_MUTEX_LOCK(self->ref_lock);
        self->refcount++;
        P_MUTEX_UNLOCK(self->ref_lock);
        return 1;
    }

    return 0;
}

void CObject__unref(CObject *self){
    if(self){
        P_MUTEX_LOCK(self->ref_lock);
        self->refcount--;
        if(self->refcount == 0){
            P_MUTEX_UNLOCK(self->ref_lock);
            CObject__destroy(self);
        } else {
            P_MUTEX_UNLOCK(self->ref_lock);
        }
    }
}

int CObject__ref_count(CObject *self){
    int ret = -1;
    if(self){
        P_MUTEX_LOCK(self->ref_lock);
        ret = self->refcount;
        P_MUTEX_UNLOCK(self->ref_lock);
    }
    return ret;
}


int CObject__is_destroyed(CObject * self){
    int ret = -1;
    if(self){
        P_MUTEX_LOCK(self->ref_lock);
        ret = self->destroyed;
        P_MUTEX_UNLOCK(self->ref_lock);

    }
    return ret;
}

int CObject__is_valid(CObject* self){
    if(self && CObject__ref_count(self) > 0 && !CObject__is_destroyed(self)){
        return 1;
    }

    return 0;
}