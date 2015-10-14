/*
 * Interface.h
 *
 * Created: 2015-09-26 23:37:50
 *  Author: Andreas
 */ 


#ifndef INTERFACE_H_
#define INTERFACE_H_

typedef void (*interface_fp)(void);

struct view
{
    interface_fp draw_function;
    struct view *parent;
    struct view *child;
    struct view *next;
    struct view *prev;
};

void Interface_Init(void);
void Interface_Update(void);
void Interface_Refresh(void);
void Interface_AddView(struct view *new_view);
void Interface_AddChild(struct view *parent_view, struct view *child_view);

#endif /* INTERFACE_H_ */