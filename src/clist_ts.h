#ifndef C_LIST_TS_H_ 
#define C_LIST_TS_H_

#include "cobject.h"
#include "portable_thread.h"

typedef struct _CListTS CListTS;

struct _CListTS {
    CObject parent;
    void (*destroy_callback)(CListTS *);
    P_MUTEX_TYPE lock;
    int count;
    CObject ** data;
};

CListTS * CListTS__create();
void CListTS__init(CListTS* self);

void CListTS__add(CListTS* self, CObject * record);
void CListTS__remove(CListTS* self, int index);
int CListTS__destroy_record(CListTS * self, CObject * record);
int CListTS__remove_record(CListTS * self, CObject * record);
CObject * CListTS__get(CListTS * self, int index);
CObject * CListTS__pop(CListTS * self);
void CListTS__clear(CListTS* self);
int CListTS__get_count(CListTS * self);
void CListTS__set_destroy_callback(CListTS* self, void (*destroy_callback)(CListTS *)); 

#endif