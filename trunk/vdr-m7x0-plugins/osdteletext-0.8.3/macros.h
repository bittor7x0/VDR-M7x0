/*
 *  libzvbi - Useful macros
 *
 *  Copyright (C) 2002, 2003, 2004 Michael H. Schimek
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Id: macros.h,v 1.5 2006/03/21 19:01:24 mschimek Exp $ */

#ifndef __ZVBI3_MACROS_H__
#define __ZVBI3_MACROS_H__

#ifdef __cplusplus
#  define VBI3_BEGIN_DECLS extern "C" {
#  define VBI3_END_DECLS }
#else
#  define VBI3_BEGIN_DECLS
#  define VBI3_END_DECLS
#endif

VBI3_BEGIN_DECLS

#if __GNUC__ >= 4
#  define _vbi3_sentinel sentinel(0)
#else
#  define _vbi3_sentinel
#  define __restrict__
#endif

#if (__GNUC__ == 3 && __GNUC_MINOR__ >= 3) || __GNUC__ >= 4
#  define _vbi3_nonnull(args...) nonnull(args)
#else
#  define _vbi3_nonnull(args...)
#endif

#if __GNUC__ >= 3
#  define _vbi3_pure pure
#  define _vbi3_alloc malloc
#else
#  define _vbi3_pure
#  define _vbi3_alloc
#endif

#if __GNUC__ >= 2
#  define vbi3_inline static __inline__
#else
#  define vbi3_inline static
#  define __attribute__(args...)
#endif

/**
 * @ingroup Basic
 * @name Boolean type
 * @{
 */
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

typedef int vbi3_bool;
/** @} */

#ifndef NULL
#  ifdef __cplusplus
#    define NULL (0L)
#  else
#    define NULL ((void *) 0)
#  endif
#endif

/** XXX Document me - for variadic funcs.
  (this is a pointer not int to take advantage of
  __attribute__((sentinel))). */
#define VBI3_END ((void *) 0)

typedef void
vbi3_lock_fn			(void *			user_data);
typedef void
vbi3_unlock_fn			(void *			user_data);

VBI3_END_DECLS

#endif /* __ZVBI3_MACROS_H__ */
