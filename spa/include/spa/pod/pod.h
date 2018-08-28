/* Simple Plugin API
 * Copyright (C) 2017 Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __SPA_POD_H__
#define __SPA_POD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include <spa/utils/defs.h>
#include <spa/utils/type.h>

#ifndef SPA_POD_MAX_DEPTH
#define SPA_POD_MAX_DEPTH       16
#endif

#define SPA_POD_BODY_SIZE(pod)			(((struct spa_pod*)(pod))->size)
#define SPA_POD_TYPE(pod)			(((struct spa_pod*)(pod))->type)
#define SPA_POD_SIZE(pod)			(sizeof(struct spa_pod) + SPA_POD_BODY_SIZE(pod))
#define SPA_POD_CONTENTS_SIZE(type,pod)		(SPA_POD_SIZE(pod)-sizeof(type))

#define SPA_POD_CONTENTS(type,pod)		SPA_MEMBER((pod),sizeof(type),void)
#define SPA_POD_CONTENTS_CONST(type,pod)	SPA_MEMBER((pod),sizeof(type),const void)
#define SPA_POD_BODY(pod)			SPA_MEMBER((pod),sizeof(struct spa_pod),void)
#define SPA_POD_BODY_CONST(pod)			SPA_MEMBER((pod),sizeof(struct spa_pod),const void)

struct spa_pod {
	uint32_t size;
	uint32_t type;		/* a basic id of enum spa_type */
};

#define SPA_POD_VALUE(type,pod)			(((type*)pod)->value)

struct spa_pod_bool {
	struct spa_pod pod;
	int32_t value;
	int32_t __padding;
};

struct spa_pod_enum {
	struct spa_pod pod;
	uint32_t value;
	int32_t __padding;
};

struct spa_pod_int {
	struct spa_pod pod;
	int32_t value;
	int32_t __padding;
};

struct spa_pod_long {
	struct spa_pod pod;
	int64_t value;
};

struct spa_pod_float {
	struct spa_pod pod;
	float value;
	int32_t __padding;
};

struct spa_pod_double {
	struct spa_pod pod;
	double value;
};

struct spa_pod_string {
	struct spa_pod pod;
	/* value here */
};

struct spa_pod_bytes {
	struct spa_pod pod;
	/* value here */
};

struct spa_pod_rectangle {
	struct spa_pod pod;
	struct spa_rectangle value;
};

struct spa_pod_fraction {
	struct spa_pod pod;
	struct spa_fraction value;
};

struct spa_pod_bitmap {
	struct spa_pod pod;
	/* array of uint8_t follows with the bitmap */
};

struct spa_pod_array_body {
	struct spa_pod child;
	/* array with elements of child.size follows */
};

struct spa_pod_array {
	struct spa_pod pod;
	struct spa_pod_array_body body;
};

struct spa_pod_struct {
	struct spa_pod pod;
	/* one or more spa_pod follow */
};

struct spa_pod_object_body {
	uint32_t type;		/**< one of enum spa_type */
	uint32_t id;		/**< id of the object, depends on the object type */
	/* contents follow, series of spa_pod */
};

struct spa_pod_object {
	struct spa_pod pod;
	struct spa_pod_object_body body;
};

static inline bool spa_pod_is_object_type(const struct spa_pod *pod, uint32_t type)
{
	return (pod && pod->type == SPA_TYPE_Object
		&& ((struct spa_pod_object *) pod)->body.type == type);
}

static inline bool spa_pod_is_object_id(const struct spa_pod *pod, uint32_t id)
{
	return (pod && pod->type == SPA_TYPE_Object
		&& ((struct spa_pod_object *) pod)->body.id == id);
}

struct spa_pod_pointer_body {
	uint32_t type;		/**< pointer id, one of enum spa_type */
	void *value;
};

struct spa_pod_pointer {
	struct spa_pod pod;
	struct spa_pod_pointer_body body;
};

struct spa_pod_fd {
	struct spa_pod pod;
	int value;
};

#define SPA_POD_PROP_N_VALUES(prop)	(((prop)->pod.size - sizeof(struct spa_pod_prop_body)) / (prop)->body.value.size)

struct spa_pod_prop_body {
	uint32_t key;			/**< key of property, list of valid keys depends on the
					  *  object type */
#define SPA_POD_PROP_RANGE_NONE		0	/**< no range */
#define SPA_POD_PROP_RANGE_MIN_MAX	1	/**< property has range */
#define SPA_POD_PROP_RANGE_STEP		2	/**< property has range with step */
#define SPA_POD_PROP_RANGE_ENUM		3	/**< property has enumeration */
#define SPA_POD_PROP_RANGE_FLAGS	4	/**< property has flags */
#define SPA_POD_PROP_RANGE_MASK		0xf	/**< mask to select range type */
#define SPA_POD_PROP_FLAG_UNSET		(1 << 4)	/**< property value is unset */
#define SPA_POD_PROP_FLAG_OPTIONAL	(1 << 5)	/**< property value is optional */
#define SPA_POD_PROP_FLAG_READONLY	(1 << 6)	/**< property is readonly */
#define SPA_POD_PROP_FLAG_DEPRECATED	(1 << 7)	/**< property is deprecated */
#define SPA_POD_PROP_FLAG_INFO		(1 << 8)	/**< property is informational and is not
							  *  used when filtering */
#define SPA_POD_PROP_FLAG_CONTROLLABLE	(1 << 9)	/**< property can be controlled */
	uint32_t flags;
	struct spa_pod value;
	/* array with elements of value.size follows,
	 * first element is value/default, rest are alternatives */
};

struct spa_pod_prop {
	struct spa_pod pod;
	struct spa_pod_prop_body body;
};

#define SPA_POD_CONTROL_SIZE(ev)	(sizeof(struct spa_pod_control) + (ev)->value.size)

/* controls can be inside a sequence and mark timed values */
struct spa_pod_control {
	uint32_t offset;	/**< media offset */
	uint32_t type;		/**< type of control, enum spa_control_type */
	struct spa_pod value;	/**< control value, depends on type */
	/* value contents follow */
};

struct spa_pod_sequence_body {
	uint32_t unit;
	uint32_t pad;
	/* series of struct spa_pod_control follows */
};

/** a sequence of timed controls */
struct spa_pod_sequence {
	struct spa_pod pod;
	struct spa_pod_sequence_body body;
};


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* __SPA_POD_H__ */
