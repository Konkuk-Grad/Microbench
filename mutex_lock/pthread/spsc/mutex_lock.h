#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


pthread_mutex_t queue_lock;

struct msgbuf{
    long item;
    long measure_time;
};

struct linked_list{
    struct linked_list *next;
    struct msgbuf* buf;
};


struct linked_list* list_head;
struct linked_list* list_tail;
 

struct msgbuf* init_buf(long item){
    struct msgbuf* buf = (struct msgbuf*)malloc(sizeof(struct msgbuf));
    buf->item = item;
    buf->measure_time = 0;
    return buf;
}

void push_queue(struct msgbuf* new){
    struct msgbuf* new;
    struct linked_list* tmp = (struct linked_list*)malloc(sizeof(struct linked_list));
    tmp->buf = new;
    tmp->next = NULL;

    list_tail->next = tmp;
    list_tail = tmp;
}


struct msgbuf* pop_queue(struct linked_list* list_head){
    struct msgbuf* old;
    struct linked_list* tmp;


    if(is_empty()){
        return NULL;
    }
    tmp = list_head->next;
    old = tmp->buf;
    list_head->next = tmp->next;

    free(tmp);
    return old;

}
int is_empty(){
    
    if(list_head == list_tail)
        return 1;
    
    return -1;
}