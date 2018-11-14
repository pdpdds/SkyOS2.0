/*
** Copyright 2001-2004, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#include "windef.h"
#include "stdio.h"
#include "memory.h"
#include "net/sem.h"
#include "Net/list.h"
#include "math.h"
//#include <kernel/smp.h>
//#include <kernel/int.h>
#include "net/timer.h"
#include "kheap.h"
//#include <kernel/thread.h>
//#include <kernel/vm.h>
#include "syserror.h"

//#include <boot/stage2.h>

#include <string.h>

#define SYS_MAX_OS_NAME_LEN 32

#define PAGE_SIZE 4096

#define CHECK_BIT(a, b) ((a) & (1 << (b)))
#define SET_BIT(a, b) ((a) | (1 << (b)))
#define CLEAR_BIT(a, b) ((a) & (~(1 << (b))))

#define PAGE_ALIGN(x) (((x) + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1))

typedef unsigned long addr_t;

#define LOCK_RO        0x0
#define LOCK_RW        0x1
#define LOCK_KERNEL    0x2
#define LOCK_MASK      0x3

enum {
	REGION_WIRING_LAZY = 0,
	REGION_WIRING_WIRED,
	REGION_WIRING_WIRED_ALREADY,
	REGION_WIRING_WIRED_CONTIG
};

enum {
	REGION_ADDR_ANY_ADDRESS = 0,
	REGION_ADDR_EXACT_ADDRESS
};

enum {
	INT_NO_RESCHEDULE,
	INT_RESCHEDULE
};

struct sem_entry {
	sem_id    id;
	int       count;
	struct list_node q;
	const char *name;
	int       lock;
	proc_id   owner;		 // if set to -1, means owned by a port
};

#define MAX_SEMS 4096

static struct sem_entry *sems = NULL;
static region_id sem_region = 0;
static bool sems_active = false;

static sem_id next_sem = 0;

static int sem_spinlock = 0;
#define GRAB_SEM_LIST_LOCK() acquire_spinlock(&sem_spinlock)
#define RELEASE_SEM_LIST_LOCK() release_spinlock(&sem_spinlock)
#define GRAB_SEM_LOCK(s) acquire_spinlock(&(s).lock)
#define RELEASE_SEM_LOCK(s) release_spinlock(&(s).lock)

// used in functions that may put a bunch of threads in the run q at once
#define READY_THREAD_CACHE_SIZE 16

static int remove_thread_from_sem(struct thread *t, struct sem_entry *sem, struct list_node *queue, int sem_errcode);

struct sem_timeout_args {
	thread_id blocked_thread;
	sem_id blocked_sem_id;
	int sem_count;
};

static void dump_sem_list(int argc, char **argv)
{
	int i;

	for(i=0; i<MAX_SEMS; i++) {
		if(sems[i].id >= 0) {
//			dprintf("%p\tid: 0x%x\t\tname: '%s'\n", &sems[i], sems[i].id, sems[i].name);
		}
	}
}

static void _dump_sem_info(struct sem_entry *sem)
{
/*	dprintf("SEM:   %p\n", sem);
	dprintf("name:  '%s'\n", sem->name);
	dprintf("owner: 0x%x\n", sem->owner);
	dprintf("count: 0x%x\n", sem->count);
	dprintf("queue: head %p tail %p\n", sem->q.next, sem->q.prev);*/
}

static void dump_sem_info(int argc, char **argv)
{
	int i;

	if(argc < 2) {
		dprintf("sem: not enough arguments\n");
		return;
	}

	// if the argument looks like a hex number, treat it as such
	if(strlen(argv[1]) > 2 && argv[1][0] == '0' && argv[1][1] == 'x') {
		unsigned long num = atoul(argv[1]);

		if(is_kernel_address(num)) {
			// XXX semi-hack
			_dump_sem_info((struct sem_entry *)num);
			return;
		} else {
			unsigned slot = num % MAX_SEMS;
			if(sems[slot].id != (int)num) {
				dprintf("sem 0x%lx doesn't exist!\n", num);
				return;
			}
			_dump_sem_info(&sems[slot]);
			return;
		}
	}

	// walk through the sem list, trying to match name
	for(i=0; i<MAX_SEMS; i++) {
		if (sems[i].name != NULL)
			if(strcmp(argv[1], sems[i].name) == 0) {
				_dump_sem_info(&sems[i]);
				return;
			}
	}
}

int sem_init(kernel_args *ka)
{
	int i;

	dprintf("sem_init: entry\n");

	// create and initialize semaphore table
	sem_region = vm_create_anonymous_region(vm_get_kernel_aspace_id(), "sem_table", (void **)&sems,
		REGION_ADDR_ANY_ADDRESS, sizeof(struct sem_entry) * MAX_SEMS, REGION_WIRING_WIRED, LOCK_RW|LOCK_KERNEL);
	if(sem_region < 0) {
		panic("unable to allocate semaphore table!\n");
	}

	memset(sems, 0, sizeof(struct sem_entry) * MAX_SEMS);
	for(i=0; i<MAX_SEMS; i++)
		sems[i].id = -1;

	// add debugger commands
	dbg_add_command(&dump_sem_list, "sems", "Dump a list of all active semaphores");
	dbg_add_command(&dump_sem_info, "sem", "Dump info about a particular semaphore");

	dprintf("sem_init: exit\n");

	sems_active = true;

	return 0;
}

sem_id sem_create_etc(int count, const char *name, proc_id owner)
{
	int i;
	sem_id retval = ERR_SEM_OUT_OF_SLOTS;
	char *temp_name;
	int name_len;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;

	if(name == NULL)
		name = "unnamed sem";

	name_len = strlen(name) + 1;
	name_len = MIN(name_len, SYS_MAX_OS_NAME_LEN);

	temp_name = (char *)kmalloc(name_len);
	if(temp_name == NULL)
		return ERR_NO_MEMORY;

	strlcpy(temp_name, name, name_len);

	int_disable_interrupts();
	GRAB_SEM_LIST_LOCK();

	// find the first empty spot
	for(i=0; i<MAX_SEMS; i++) {
		if(sems[i].id == -1) {
			// make the sem id be a multiple of the slot it's in
			if(i >= next_sem % MAX_SEMS) {
				next_sem += i - next_sem % MAX_SEMS;
			} else {
				next_sem += MAX_SEMS - (next_sem % MAX_SEMS - i);
			}
			sems[i].id = next_sem++;

			sems[i].lock = 0;
			GRAB_SEM_LOCK(sems[i]);
			RELEASE_SEM_LIST_LOCK();

			list_initialize(&sems[i].q);
			sems[i].count = count;
			sems[i].name = temp_name;
			sems[i].owner = owner;
			retval = sems[i].id;

			RELEASE_SEM_LOCK(sems[i]);
			goto out;
		}
	}

	RELEASE_SEM_LIST_LOCK();
	kfree(temp_name);

out:
	int_restore_interrupts();

	return retval;
}

sem_id sem_create(int count, const char *name)
{
	return sem_create_etc(count, name, proc_get_kernel_proc_id());
}

int sem_delete(sem_id id)
{
	return sem_delete_etc(id, 0);
}

int sem_delete_etc(sem_id id, int return_code)
{
	int slot;
	int err = NO_ERROR;
	struct thread *t;
	int released_threads;
	char *old_name;
	struct list_node release_queue;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;
	if(id < 0)
		return ERR_INVALID_HANDLE;

	slot = id % MAX_SEMS;

	int_disable_interrupts();
	GRAB_SEM_LOCK(sems[slot]);

	if(sems[slot].id != id) {
		RELEASE_SEM_LOCK(sems[slot]);
		int_restore_interrupts();
		dprintf("sem_delete: invalid sem_id %d\n", id);
		return ERR_INVALID_HANDLE;
	}

	released_threads = 0;
	list_initialize(&release_queue);

	// free any threads waiting for this semaphore
	while((t = thread_dequeue(&sems[slot].q)) != NULL) {
		t->state = THREAD_STATE_READY;
		t->sem_errcode = ERR_SEM_DELETED;
		t->sem_deleted_retcode = return_code;
		t->sem_count = 0;
		thread_enqueue(t, &release_queue);
		released_threads++;
	}

	sems[slot].id = -1;
	old_name = (char *)sems[slot].name;
	sems[slot].name = NULL;

	RELEASE_SEM_LOCK(sems[slot]);

	if(released_threads > 0) {
		GRAB_THREAD_LOCK();
		while((t = thread_dequeue(&release_queue)) != NULL) {
			thread_enqueue_run_q(t);
		}
		thread_resched();
		RELEASE_THREAD_LOCK();
	}

	int_restore_interrupts();

	kfree(old_name);

	return err;
}

// Called from a timer handler. Wakes up a semaphore
static int sem_timeout(void *data)
{
	struct sem_timeout_args *args = (struct sem_timeout_args *)data;
	struct thread *t;
	int slot;
	struct list_node wakeup_queue;

	t = thread_get_thread_struct(args->blocked_thread);
	if(t == NULL)
		return INT_NO_RESCHEDULE;
	slot = args->blocked_sem_id % MAX_SEMS;

	int_disable_interrupts();
	GRAB_SEM_LOCK(sems[slot]);

//	dprintf("sem_timeout: called on 0x%x sem %d, tid %d\n", to, to->sem_id, to->thread_id);

	if(sems[slot].id != args->blocked_sem_id) {
		// this thread was not waiting on this semaphore
		panic("sem_timeout: thid %d was trying to wait on sem %d which doesn't exist!\n",
			args->blocked_thread, args->blocked_sem_id);
	}

	list_initialize(&wakeup_queue);
	remove_thread_from_sem(t, &sems[slot], &wakeup_queue, ERR_SEM_TIMED_OUT);

	RELEASE_SEM_LOCK(sems[slot]);

	GRAB_THREAD_LOCK();
	// put the threads in the run q here to make sure we dont deadlock in sem_interrupt_thread
	while((t = thread_dequeue(&wakeup_queue)) != NULL) {
		thread_enqueue_run_q(t);
	}
	RELEASE_THREAD_LOCK();

	int_restore_interrupts();

	return INT_RESCHEDULE;
}

int sem_acquire(sem_id id, int count)
{
	return sem_acquire_etc(id, count, 0, 0, NULL);
}

int sem_acquire_etc(sem_id id, int count, int flags, bigtime_t timeout, int *deleted_retcode)
{
	int slot = id % MAX_SEMS;
	int err = 0;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;

	if(id < 0) {
		dprintf("sem_acquire_etc: invalid sem handle %d\n", id);
		return ERR_INVALID_HANDLE;
	}

	if(count <= 0)
		return ERR_INVALID_ARGS;

//	if(!kernel_startup && !int_are_interrupts_enabled())
		//panic("sem_acquire_etc: sem attempted to be acquired with interrupts disabled\n");

	int_disable_interrupts();
	GRAB_SEM_LOCK(sems[slot]);

	if(sems[slot].id != id) {
		dprintf("sem_acquire_etc: bad sem_id %d\n", id);
		err = ERR_INVALID_HANDLE;
		goto err;
	}

	if(sems[slot].count - count < 0 && (flags & SEM_FLAG_TIMEOUT) != 0 && timeout <= 0) {
		// immediate timeout
		err = ERR_SEM_TIMED_OUT;
		goto err;
	}

	if((sems[slot].count -= count) < 0) {
		// we need to block
		struct thread *t = thread_get_current_thread();
		struct timer_event timer; // stick it on the stack, since we may be blocking here
		struct sem_timeout_args args;

		// do a quick check to see if the thread has any pending kill signals
		// this should catch most of the cases where the thread had a signal
		//if((flags & SEM_FLAG_INTERRUPTABLE) && t->sig_pending) {
		if ((flags & SEM_FLAG_INTERRUPTABLE)) {
			sems[slot].count += count;
			err = ERR_INTERRUPTED;
			goto err;
		}

		t->next_state = THREAD_STATE_WAITING;
		t->sem_flags = flags;
		t->sem_blocking = id;
		t->sem_acquire_count = count;
		t->sem_count = MIN(-sems[slot].count, count); // store the count we need to restore upon release
		t->sem_deleted_retcode = 0;
		t->sem_errcode = NO_ERROR;
		thread_enqueue(t, &sems[slot].q);

		if((flags & SEM_FLAG_TIMEOUT) != 0) {
//			dprintf("sem_acquire_etc: setting timeout sem for %d %d usecs, semid %d, tid %d\n",
//				timeout, sem_id, t->id);
			// set up an event to go off with the thread struct as the data
			args.blocked_sem_id = id;
			args.blocked_thread = t->id;
			args.sem_count = count;
			timer_setup_timer(&sem_timeout, &args, &timer);
			timer_set_event(timeout, TIMER_MODE_ONESHOT, &timer);
		}

		RELEASE_SEM_LOCK(sems[slot]);
		GRAB_THREAD_LOCK();
		// check again to see if a kill signal is pending.
		// it may have been delivered while setting up the sem, though it's pretty unlikely
//		if((flags & SEM_FLAG_INTERRUPTABLE) && t->sig_pending) {
		if ((flags & SEM_FLAG_INTERRUPTABLE)) {
			struct list_node wakeup_queue;
			// ok, so a tiny race happened where a signal was delivered to this thread while
			// it was setting up the sem. We can only be sure a signal wasn't delivered
			// here, since the threadlock is held. The previous check would have found most
			// instances, but there was a race, so we have to handle it. It'll be more messy...
			list_initialize(&wakeup_queue);
			GRAB_SEM_LOCK(sems[slot]);
			if(sems[slot].id == id) {
				remove_thread_from_sem(t, &sems[slot], &wakeup_queue, ERR_INTERRUPTED);
			}
			RELEASE_SEM_LOCK(sems[slot]);
			while((t = thread_dequeue(&wakeup_queue)) != NULL) {
				thread_enqueue_run_q(t);
			}
			// fall through and reschedule since another thread with a higher priority may have been woken up
		}
		thread_resched();
		RELEASE_THREAD_LOCK();

		if((flags & SEM_FLAG_TIMEOUT) != 0) {
			if(t->sem_errcode != ERR_SEM_TIMED_OUT) {
				// cancel the timer event, the sem may have been deleted or interrupted
				// with the timer still active
				timer_cancel_event(&timer);
			}
		}

		int_restore_interrupts();
		if(deleted_retcode != NULL)
			*deleted_retcode = t->sem_deleted_retcode;
		return t->sem_errcode;
	}

err:
	RELEASE_SEM_LOCK(sems[slot]);
	int_restore_interrupts();

	return err;
}

int sem_release(sem_id id, int count)
{
	return sem_release_etc(id, count, 0);
}

int sem_release_etc(sem_id id, int count, int flags)
{
	int slot = id % MAX_SEMS;
	int released_threads = 0;
	int err = 0;
	struct list_node release_queue;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;

	if(id < 0)
		return ERR_INVALID_HANDLE;

	if(count <= 0)
		return ERR_INVALID_ARGS;

	int_disable_interrupts();
	GRAB_SEM_LOCK(sems[slot]);

	if(sems[slot].id != id) {
		dprintf("sem_release_etc: invalid sem_id %d\n", id);
		err = ERR_INVALID_HANDLE;
		goto err;
	}

	// clear out a queue we will use to hold all of the threads that we will have to
	// put back into the run list. This is done so the thread lock wont be held
	// while this sems lock is held since the two locks are grabbed in the other
	// order in sem_interrupt_thread.
	list_initialize(&release_queue);

	while(count > 0) {
		int delta = count;
		if(sems[slot].count < 0) {
			struct thread *t = thread_lookat_queue(&sems[slot].q);

			delta = MIN(count, t->sem_count);
			t->sem_count -= delta;
			if(t->sem_count <= 0) {
				// release this thread
				t = thread_dequeue(&sems[slot].q);
				thread_enqueue(t, &release_queue);
				t->state = THREAD_STATE_READY;
				released_threads++;
				t->sem_count = 0;
				t->sem_deleted_retcode = 0;
			}
		}

		sems[slot].count += delta;
		count -= delta;
	}
	RELEASE_SEM_LOCK(sems[slot]);

	// pull off any items in the release queue and put them in the run queue
	if(released_threads > 0) {
		struct thread *t;
		GRAB_THREAD_LOCK();
		while((t = thread_dequeue(&release_queue)) != NULL) {
			thread_enqueue_run_q(t);
		}
		if((flags & SEM_FLAG_NO_RESCHED) == 0) {
			thread_resched();
		}
		RELEASE_THREAD_LOCK();
	}
	goto outnolock;

err:
	RELEASE_SEM_LOCK(sems[slot]);
outnolock:
	int_restore_interrupts();

	return err;
}

int sem_get_count(sem_id id, int32* thread_count)
{
	int slot;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;
	if(id < 0)
		return ERR_INVALID_HANDLE;
	if (thread_count == NULL)
		return ERR_INVALID_ARGS;

	slot = id % MAX_SEMS;

	int_disable_interrupts();
	GRAB_SEM_LOCK(sems[slot]);

	if(sems[slot].id != id) {
		RELEASE_SEM_LOCK(sems[slot]);
		int_restore_interrupts();
		dprintf("sem_get_count: invalid sem_id %d\n", id);
		return ERR_INVALID_HANDLE;
	}

	*thread_count = sems[slot].count;

	RELEASE_SEM_LOCK(sems[slot]);
	int_restore_interrupts();

	return NO_ERROR;
}

int sem_get_sem_info(sem_id id, struct sem_info *info)
{
	int slot;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;
	if(id < 0)
		return ERR_INVALID_HANDLE;
	if (info == NULL)
		return ERR_INVALID_ARGS;

	slot = id % MAX_SEMS;

	int_disable_interrupts();
	GRAB_SEM_LOCK(sems[slot]);

	if(sems[slot].id != id) {
		RELEASE_SEM_LOCK(sems[slot]);
		int_restore_interrupts();
		dprintf("get_sem_info: invalid sem_id %d\n", id);
		return ERR_INVALID_HANDLE;
	}

	info->sem			= sems[slot].id;
	info->proc			= sems[slot].owner;
	strncpy(info->name, sems[slot].name, SYS_MAX_OS_NAME_LEN-1);
	info->count			= sems[slot].count;
	info->latest_holder	= -1; // XXX fixme

	RELEASE_SEM_LOCK(sems[slot]);
	int_restore_interrupts();

	return NO_ERROR;
}

int sem_get_next_sem_info(proc_id proc, uint32 *cookie, struct sem_info *info)
{
	int slot;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;

	if (cookie == NULL)
		return ERR_INVALID_ARGS;
	if (proc < 0)
		return ERR_INVALID_ARGS;	// prevents sems[].owner == -1 >= means owned by a port

	if (*cookie == NULL) {
		// return first found
		slot = 0;
	} else {
		// start at index cookie, but check cookie against MAX_PORTS
		slot = *cookie;
		if (slot >= MAX_SEMS)
			return ERR_INVALID_HANDLE;
	}
	// spinlock
	int_disable_interrupts();
	GRAB_SEM_LIST_LOCK();

	while (slot < MAX_SEMS) {
		GRAB_SEM_LOCK(sems[slot]);
		if (sems[slot].id != -1)
			if (sems[slot].owner == proc) {
				// found one!
				info->sem			= sems[slot].id;
				info->proc			= sems[slot].owner;
				strncpy(info->name, sems[slot].name, SYS_MAX_OS_NAME_LEN-1);
				info->count			= sems[slot].count;
				info->latest_holder	= -1; // XXX fixme

				RELEASE_SEM_LOCK(sems[slot]);
				slot++;
				break;
			}
		RELEASE_SEM_LOCK(sems[slot]);
		slot++;
	}
	RELEASE_SEM_LIST_LOCK();
	int_restore_interrupts();

	if (slot == MAX_SEMS)
		return ERR_SEM_NOT_FOUND;
	*cookie = slot;
	return NO_ERROR;
}

int set_sem_owner(sem_id id, proc_id proc)
{
	int slot;

	if(sems_active == false)
		return ERR_SEM_NOT_ACTIVE;
	if(id < 0)
		return ERR_INVALID_HANDLE;
	if (proc < NULL)
		return ERR_INVALID_ARGS;

	// XXX: todo check if proc exists
//	if (proc_get_proc_struct(proc) == NULL)
//		return ERR_INVALID_HANDLE; // proc_id doesn't exist right now

	slot = id % MAX_SEMS;

	int_disable_interrupts();
	GRAB_SEM_LOCK(sems[slot]);

	if(sems[slot].id != id) {
		RELEASE_SEM_LOCK(sems[slot]);
		int_restore_interrupts();
		dprintf("set_sem_owner: invalid sem_id %d\n", id);
		return ERR_INVALID_HANDLE;
	}

	sems[slot].owner = proc;

	RELEASE_SEM_LOCK(sems[slot]);
	int_restore_interrupts();

	return NO_ERROR;
}

// Wake up a thread that's blocked on a semaphore
// this function must be entered with interrupts disabled and THREADLOCK held
int sem_interrupt_thread(struct thread *t)
{
	int slot;
	struct list_node wakeup_queue;

//	dprintf("sem_interrupt_thread: called on thread %p (%d), blocked on sem 0x%x\n", t, t->id, t->sem_blocking);

	if(t->state != THREAD_STATE_WAITING)
		return ERR_INVALID_ARGS;
	if(t->sem_blocking < 0)
		return ERR_INVALID_ARGS;
	if((t->sem_flags & SEM_FLAG_INTERRUPTABLE) == 0)
		return ERR_SEM_NOT_INTERRUPTABLE;

	slot = t->sem_blocking % MAX_SEMS;

	GRAB_SEM_LOCK(sems[slot]);

	if(sems[slot].id != t->sem_blocking) {
		panic("sem_interrupt_thread: thread 0x%x sez it's blocking on sem 0x%x, but that sem doesn't exist!\n", t->id, t->sem_blocking);
	}

	list_initialize(&wakeup_queue);
	if(remove_thread_from_sem(t, &sems[slot], &wakeup_queue, ERR_INTERRUPTED) == ERR_NOT_FOUND)
		panic("sem_interrupt_thread: thread 0x%x not found in sem 0x%x's wait queue\n", t->id, t->sem_blocking);

	RELEASE_SEM_LOCK(sems[slot]);

	while((t = thread_dequeue(&wakeup_queue)) != NULL) {
		thread_enqueue_run_q(t);
	}

	return NO_ERROR;
}

// forcibly removes a thread from a semaphores wait q. May have to wake up other threads in the
// process. All threads that need to be woken up are added to the passed in thread_queue.
// must be called with sem lock held
static int remove_thread_from_sem(struct thread *t, struct sem_entry *sem, struct list_node *queue, int sem_errcode)
{
	struct thread *t1;

	// remove the thread from the queue and place it in the supplied queue
	thread_dequeue_thread(t);
	sem->count += t->sem_acquire_count;
	t->state = THREAD_STATE_READY;
	t->sem_errcode = sem_errcode;
	thread_enqueue(t, queue);

	// now see if more threads need to be woken up
	while(sem->count > 0 && (t1 = thread_lookat_queue(&sem->q))) {
		int delta = MIN(t->sem_count, sem->count);

		t->sem_count -= delta;
		if(t->sem_count <= 0) {
			t = thread_dequeue(&sem->q);
			t->state = THREAD_STATE_READY;
			thread_enqueue(t, queue);
		}
		sem->count -= delta;
	}
	return NO_ERROR;
}

/* this function cycles through the sem table, deleting all the sems that are owned by
   the passed proc_id */
int sem_delete_owned_sems(proc_id owner)
{
	int i;
	int count = 0;

	if (owner < 0)
		return ERR_INVALID_HANDLE;

	int_disable_interrupts();
	GRAB_SEM_LIST_LOCK();

	for(i=0; i<MAX_SEMS; i++) {
		if(sems[i].id != -1 && sems[i].owner == owner) {
			sem_id id = sems[i].id;

			RELEASE_SEM_LIST_LOCK();
			int_restore_interrupts();

			sem_delete_etc(id, 0);
			count++;

			int_disable_interrupts();
			GRAB_SEM_LIST_LOCK();
		}
	}

	RELEASE_SEM_LIST_LOCK();
	int_restore_interrupts();

	return count;
}

sem_id user_sem_create(int count, const char *uname)
{
	if(uname != NULL) {
		char name[SYS_MAX_OS_NAME_LEN];
		int rc;

		if(is_kernel_address(uname))
			return ERR_VM_BAD_USER_MEMORY;

		rc = user_strncpy(name, uname, SYS_MAX_OS_NAME_LEN-1);
		if(rc < 0)
			return rc;
		name[SYS_MAX_OS_NAME_LEN-1] = 0;

		return sem_create_etc(count, name, proc_get_current_proc_id());
	} else {
		return sem_create_etc(count, NULL, proc_get_current_proc_id());
	}
}

int user_sem_delete(sem_id id)
{
	return sem_delete(id);
}

int user_sem_delete_etc(sem_id id, int return_code)
{
	return sem_delete_etc(id, return_code);
}

int user_sem_acquire(sem_id id, int count)
{
	return user_sem_acquire_etc(id, count, 0, 0, NULL);
}

int user_sem_acquire_etc(sem_id id, int count, int flags, bigtime_t timeout, int *deleted_retcode)
{
	if(deleted_retcode != NULL && is_kernel_address(deleted_retcode))
		return ERR_VM_BAD_USER_MEMORY;

	flags = flags | SEM_FLAG_INTERRUPTABLE;

	if(deleted_retcode == NULL) {
		return sem_acquire_etc(id, count, flags, timeout, deleted_retcode);
	} else {
		int uretcode;
		int rc, rc2;

		rc = user_memcpy(&uretcode, deleted_retcode, sizeof(uretcode));
		if(rc < 0)
			return rc;

		rc = sem_acquire_etc(id, count, flags, timeout, &uretcode);
		if(rc < 0)
			return rc;

		rc2 = user_memcpy(deleted_retcode, &uretcode, sizeof(uretcode));
		if(rc2 < 0)
			return rc2;

		return rc;
	}
}

int user_sem_release(sem_id id, int count)
{
	return sem_release(id, count);
}

int user_sem_release_etc(sem_id id, int count, int flags)
{
	return sem_release_etc(id, count, flags);
}

int user_sem_get_count(sem_id uid, int32* uthread_count)
{
	int32 thread_count;
	int rc, rc2;
	rc  = sem_get_count(uid, &thread_count);
	rc2 = user_memcpy(uthread_count, &thread_count, sizeof(int32));
	if(rc2 < 0)
		return rc2;
	return rc;
}

int user_sem_get_sem_info(sem_id uid, struct sem_info *uinfo)
{
	struct sem_info info;
	int rc, rc2;

	if(is_kernel_address(uinfo))
		return ERR_VM_BAD_USER_MEMORY;

	rc = sem_get_sem_info(uid, &info);
	rc2 = user_memcpy(uinfo, &info, sizeof(struct sem_info));
	if(rc2 < 0)
		return rc2;
	return rc;
}

int user_sem_get_next_sem_info(proc_id uproc, uint32 *ucookie, struct sem_info *uinfo)
{
	struct sem_info info;
	uint32 cookie;
	int rc, rc2;

	if(is_kernel_address(uinfo))
		return ERR_VM_BAD_USER_MEMORY;

	rc2 = user_memcpy(&cookie, ucookie, sizeof(uint32));
	if(rc2 < 0)
		return rc2;
	rc = sem_get_next_sem_info(uproc, &cookie, &info);
	rc2 = user_memcpy(uinfo, &info, sizeof(struct sem_info));
	if(rc2 < 0)
		return rc2;
	rc2 = user_memcpy(ucookie, &cookie, sizeof(uint32));
	if(rc2 < 0)
		return rc2;
	return rc;
}

int user_set_sem_owner(sem_id uid, proc_id uproc)
{
	return set_sem_owner(uid, uproc);
}

