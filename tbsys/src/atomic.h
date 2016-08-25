/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

/**
 * Copyright (C) 2007 Doug Judd (Zvents, Inc.)
 * 
 * This file is part of Hypertable.
 * 
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 * 
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef TBSYS_ATOMIC_H
#define TBSYS_ATOMIC_H

#if defined(__x86_64__) || defined(__x86_64) || defined(__i386__) || defined(__i386)
#define ATOMIC_X86
#endif

#if defined(__ARM_ARCH_8A__) || defined(__aarch64__)
#define ATOMIC_AARCH64
#endif

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */
#ifdef ATOMIC_X86
#define LOCK "lock ; "
#endif

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { volatile int counter; } atomic_t;

#define ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @param v pointer of type atomic_t
 * 
 * Atomically reads the value of v.
 */ 
#define atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @param v pointer of type atomic_t
 * @param i required value
 * 
 * Atomically sets the value of v to i.
 */ 
#define atomic_set(v,i)		(((v)->counter) = (i))

/**
 * atomic_add - add integer to atomic variable
 * @param i integer value to add
 * @param v pointer of type atomic_t
 * 
 * Atomically adds i to v.
 */
static __inline__ void atomic_add(int i, atomic_t *v)
{
	__atomic_add_fetch(&((v)->counter), i, __ATOMIC_SEQ_CST);
}

/**
 * atomic_sub - subtract the atomic variable
 * @param i integer value to subtract
 * @param v pointer of type atomic_t
 * 
 * Atomically subtracts i from v.
 */
static __inline__ void atomic_sub(int i, atomic_t *v)
{
	__atomic_sub_fetch(&((v)->counter), i, __ATOMIC_SEQ_CST);
}


/**
 * atomic_add_return - add and return
 * @param v pointer of type atomic_t
 * @param i integer value to add
 *
 * Atomically adds i to v and returns i + v
 */
static __inline__ int atomic_add_return(int i, atomic_t *v)
{
	return __atomic_add_fetch(&((v)->counter), i, __ATOMIC_SEQ_CST);
}

static __inline__ int atomic_sub_return(int i, atomic_t *v)
{
	return __atomic_sub_fetch(&((v)->counter), i, __ATOMIC_SEQ_CST);
}


/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @param i integer value to subtract
 * @param v pointer of type atomic_t
 * 
 * Atomically subtracts i from v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static __inline__ int atomic_sub_and_test(int i, atomic_t *v)
{
        return (0 == __atomic_sub_fetch(&((v)->counter), i, __ATOMIC_SEQ_CST));
}

/**
 * atomic_inc - increment atomic variable
 * @param v pointer of type atomic_t
 * 
 * Atomically increments v by 1.
 */ 
static __inline__ void atomic_inc(atomic_t *v)
{
	__atomic_add_fetch(&((v)->counter), 1, __ATOMIC_SEQ_CST);
}

/**
 * atomic_dec - decrement atomic variable
 * @param v pointer of type atomic_t
 * 
 * Atomically decrements v by 1.
 */ 
static __inline__ void atomic_dec(atomic_t *v)
{
	__atomic_sub_fetch(&((v)->counter), 1, __ATOMIC_SEQ_CST);
}

/**
 * atomic_dec_and_test - decrement and test
 * @param v pointer of type atomic_t
 * 
 * Atomically decrements v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */ 
static __inline__ int atomic_dec_and_test(atomic_t *v)
{
	return (0 == __atomic_sub_fetch(&((v)->counter), 1, __ATOMIC_SEQ_CST));
}

/**
 * atomic_inc_and_test - increment and test 
 * @param v pointer of type atomic_t
 * 
 * Atomically increments v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */ 
static __inline__ int atomic_inc_and_test(atomic_t *v)
{
	return (0 == __atomic_add_fetch(&((v)->counter), 1, __ATOMIC_SEQ_CST));
}

/**
 * atomic_add_negative - add and test if negative
 * @param v pointer of type atomic_t
 * @param i integer value to add
 * 
 * Atomically adds i to v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */ 
static __inline__ int atomic_add_negative(int i, atomic_t *v)
{

	return (0 > __atomic_add_fetch(&((v)->counter), i, __ATOMIC_SEQ_CST));
}

#ifdef ATOMIC_X86
/* These are x86-specific, used by some header files */
#define atomic_clear_mask(mask, addr) \
	__asm__ __volatile__(LOCK "andl %0,%1" \
				: : "r" (~(mask)),"m" (*addr) : "memory")

#define atomic_set_mask(mask, addr) \
			__asm__ __volatile__(LOCK "orl %0,%1" \
						: : "r" (mask),"m" (*(addr)) : "memory")
#endif

#define atomic_inc_return(v)  (atomic_add_return(1,v))
#define atomic_dec_return(v)  (atomic_sub_return(1,v))

#endif // ATOMIC_H
