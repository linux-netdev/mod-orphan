/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _MOD_ORPHAN_COMPAT_H
#define _MOD_ORPHAN_COMPAT_H

#include <linux/version.h>
#include <linux/slab.h>
#include <linux/overflow.h>

/*
 * Compat shims for kzalloc_obj/kmalloc_obj/kzalloc_objs/kmalloc_objs,
 * with an optional GFP argument (defaulting to GFP_KERNEL) that every
 * call site in this tree relies on.
 *
 * Some vendor kernels (e.g. Raspberry Pi Foundation's 6.18.x) already
 * backport __alloc_objs()/kmalloc_obj()/etc. natively, but with a
 * mandatory GFP argument and no variadic default. Skipping our own
 * definitions in that case (the previous "#ifndef __alloc_objs" guard)
 * left the vendor's incompatible, GFP-mandatory macros in place and
 * broke every 1-argument call site in this tree. Always #undef and
 * redefine our own variadic versions instead of trusting whatever the
 * host kernel provides.
 */
#undef __alloc_objs
#undef kmalloc_obj
#undef kmalloc_objs
#undef kzalloc_obj
#undef kzalloc_objs

#define __default_gfp_compat(a, b, ...) b
#define default_gfp_compat(...) __default_gfp_compat(, ##__VA_ARGS__, GFP_KERNEL)

#define __alloc_objs(KMALLOC, GFP, TYPE, COUNT)				\
({									\
	const size_t __obj_size = size_mul(sizeof(TYPE), COUNT);		\
	(TYPE *)KMALLOC(__obj_size, GFP);				\
})

#define kmalloc_obj(VAR_OR_TYPE, ...) \
	__alloc_objs(kmalloc, default_gfp_compat(__VA_ARGS__), typeof(VAR_OR_TYPE), 1)

#define kmalloc_objs(VAR_OR_TYPE, COUNT, ...) \
	__alloc_objs(kmalloc, default_gfp_compat(__VA_ARGS__), typeof(VAR_OR_TYPE), COUNT)

#define kzalloc_obj(P, ...) \
	__alloc_objs(kzalloc, default_gfp_compat(__VA_ARGS__), typeof(P), 1)

#define kzalloc_objs(P, COUNT, ...) \
	__alloc_objs(kzalloc, default_gfp_compat(__VA_ARGS__), typeof(P), COUNT)

/*
 * struct sockaddr_unsized was introduced in Linux 7.0 to replace struct sockaddr
 * in proto_ops .bind/.connect callbacks. Map it to struct sockaddr on older kernels.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(7, 0, 0)
#define sockaddr_unsized sockaddr
#endif

#endif /* _MOD_ORPHAN_COMPAT_H */
