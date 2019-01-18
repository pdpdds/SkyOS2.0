/*
** Copyright 2002, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#include "windef.h"
#include "memory.h"
//#include <kernel/thread.h>
//#include <kernel/sem.h>
//#include <kernel/lock.h>
//#include <kernel/time.h>
#include "net_timer.h"
#include <string.h>
#include "syserror.h"

#define NET_TIMER_INTERVAL 200000 // 200 ms
typedef int thread_id;      // thread id
typedef struct {
	net_timer_event *next;
	net_timer_event *prev;

//	mutex  lock;
	sem_id wait_sem;

	thread_id runner_thread;
} net_timer_queue;

static net_timer_queue net_q;

static void add_to_queue(net_timer_event *e)
{
	net_timer_event *tmp = (net_timer_event *)net_q.next;
	net_timer_event *last = (net_timer_event *)&net_q;

	while(tmp != (net_timer_event *)&net_q) {
		if(tmp->sched_time > e->sched_time)
			break;
		last = tmp;
		tmp = tmp->next;
	}

	// add it to the list here
	e->next = tmp;
	e->prev = last;
	last->next = e;
	tmp->prev = e;
}

static void remove_from_queue(net_timer_event *e)
{
	e->prev->next = e->next;
	e->next->prev = e->prev;
	e->prev = e->next = NULL;
}

static net_timer_event *peek_queue_head(void)
{
	net_timer_event *e = NULL;

	if(net_q.next != (net_timer_event *)&net_q)
		e = net_q.next;

	return e;
}

int set_net_timer(net_timer_event *e, unsigned int delay_ms, net_timer_callback callback, void *args, int flags)
{
	int err = NO_ERROR;

	//mutex_lock(&net_q.lock);

	if(e->pending) {
		if(flags & NET_TIMER_PENDING_IGNORE) {
			err = ERR_GENERAL;
			goto out;
		}
		cancel_net_timer(e);
	}

	// set up the timer
	e->func = callback;
	e->args = args;
	e->sched_time = system_time() + delay_ms * 1000;
	e->pending = 1;

	add_to_queue(e);

out:
	//mutex_unlock(&net_q.lock);

	return err;
}

int cancel_net_timer(net_timer_event *e)
{
	int err = NO_ERROR;

	//mutex_lock(&net_q.lock);

	if(!e->pending) {
		err = ERR_GENERAL;
		goto out;
	}

	remove_from_queue(e);
	e->pending = 0;

out:
	//mutex_unlock(&net_q.lock);

	return err;
}

static int net_timer_runner(void *arg)
{
	net_timer_event *e;
	bigtime_t now;

	for(;;) {
		//sem_acquire_etc(net_q.wait_sem, 1, SEM_FLAG_TIMEOUT, NET_TIMER_INTERVAL, NULL);

		now = system_time();

retry:
		//mutex_lock(&net_q.lock);

		// pull off the head of the list and run it, if it timed out
		if((e = peek_queue_head()) != NULL && e->sched_time <= now) {

			remove_from_queue(e);
			e->pending = 0;

			//mutex_unlock(&net_q.lock);

			e->func(e->args);

			// Since we ran an event, loop back and check the head of 
			// the list again, because the list may have changed while
			// inside the callback.
			goto retry;
			
		} else {
		//	mutex_unlock(&net_q.lock);
		}
	}

	return 0;
}

int net_timer_init(void)
{
	int err;

	net_q.next = net_q.prev = (net_timer_event *)&net_q;

	//err = mutex_init(&net_q.lock, "net timer mutex");
	if(err < 0)
		return err;

//	net_q.wait_sem = sem_create(0, "net timer wait sem");
	if(net_q.wait_sem < 0) {
	//	mutex_destroy(&net_q.lock);
		return net_q.wait_sem;
	}

	net_q.runner_thread = thread_create_kernel_thread("net timer runner", &net_timer_runner, NULL);
	if(net_q.runner_thread < 0) {
		//sem_delete(net_q.wait_sem);
		//mutex_destroy(&net_q.lock);
		return net_q.runner_thread;
	}
	thread_resume_thread(net_q.runner_thread);

	return 0;
}

