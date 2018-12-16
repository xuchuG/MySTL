#pragma once
//在前面有若干个#ifdef #else #endif直接抄写

#ifdef __SUNPRO_CC
#  define __PRIVATE public
// Extra access restrictions prevent us from really making some things
// private.
#else
#  define __PRIVATE private
#endif

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
#  define __USE_MALLOC
#endif


// This implements some standard node allocators.  These are
// NOT the same as the allocators in the C++ draft standard or in
// in the original STL.  They do not encapsulate different pointer
// types; indeed we assume that there is only one pointer type.
// The allocation primitives are intended to allocate individual objects,
// not larger arenas as with the original STL allocators.

#ifndef __THROW_BAD_ALLOC
#  if defined(__STL_NO_BAD_ALLOC) || !defined(__STL_USE_EXCEPTIONS)
#    include <stdio.h>
#    include <stdlib.h>
#    define __THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#  else /* Standard conforming out-of-memory handling */
#    include <new>
#    define __THROW_BAD_ALLOC throw std::bad_alloc()
#  endif
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef __RESTRICT
#  define __RESTRICT
#endif

#ifdef __STL_THREADS
# include <stl_threads.h>
# define __NODE_ALLOCATOR_THREADS true
# ifdef __STL_SGI_THREADS
// We test whether threads are in use before locking.
// Perhaps this should be moved into stl_threads.h, but that
// probably makes it harder to avoid the procedure call when
// it isn't needed.
extern "C" {
	extern int __us_rsthread_malloc;
}
// The above is copied from malloc.h.  Including <malloc.h>
// would be cleaner but fails with certain levels of standard
// conformance.
#   define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \
                { _S_node_allocator_lock._M_acquire_lock(); }
#   define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \
                { _S_node_allocator_lock._M_release_lock(); }
# else /* !__STL_SGI_THREADS */
#   define __NODE_ALLOCATOR_LOCK \
        { if (threads) _S_node_allocator_lock._M_acquire_lock(); }
#   define __NODE_ALLOCATOR_UNLOCK \
        { if (threads) _S_node_allocator_lock._M_release_lock(); }
# endif
#else
//  Thread-unsafe
#   define __NODE_ALLOCATOR_LOCK
#   define __NODE_ALLOCATOR_UNLOCK
#   define __NODE_ALLOCATOR_THREADS false
#endif


#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

// Malloc-based allocator.  Typically slower than default alloc below.
// Typically thread-safe and more storage efficient.
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
# ifdef __DECLARE_GLOBALS_HERE
void(*__malloc_alloc_oom_handler)() = 0;
// g++ 2.7.2 does not handle static template data members.
# else
extern void(*__malloc_alloc_oom_handler)();
# endif
#endif

template <int __inst>//__inst未使用到
class __malloc_alloc_template {
private:
	//处理内存不足的情况，oom:out of memory
	static void* _S_oom_malloc(size_t);
	static void* _S_oom_realloc(void*, size_t);

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
	static void(*__malloc_alloc_oom_handler)();
#endif

public:

	static void* allocate(size_t __n) {
		void* __result = malloc(__n);
		if (0 == __result) {
			__result = _S_oom_malloc(__n);
		}
		return __result;
	}

	static void deallocate(void* __p, size_t) {
		free(__p);
	}

	static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz) {
		void* __result = realloc(__p, __new_sz);
		if (0 == __result) {
			__result = _S_oom_realloc(__p, __new_sz);
		}
		return __result;
	}

	static void(*__set_malloc_handler(void(*__f)()))()
	{
		void(*__old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = __f;
		return(__old);
	}
};

// malloc_alloc out-of-memory handling

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
template <int __inst>
void(*__malloc_alloc_template<__inst>::__malloc_alloc_oom_handler)() = 0;
#endif

template <int __inst>
void* __malloc_alloc_template::_S_oom_malloc(size_t __n) {
	void(*__my_malloc_handler)();
	void* __result;
	for (;;) {
		__my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == __my_malloc_handler) {
			__THROW_BAD_ALLOC;
		}
		//调用处理例程，企图释放内存
		(*__my_malloc_handler)();
		__result = malloc(__n);
		if (__result)
			return (__result);
	}
}

template <int __inst>
void* __malloc_alloc_template::_S_oom_realloc(void* __p, size_t __n) {
	void(*__my_malloc_handler)();
	void* __result;
	for (;;) {
		__my_malloc_handler  __malloc_alloc_oom_handler;
		if (0 == __my_malloc_handler) {
			__THROW_BAD_ALLOC;
		}
		(*__my_malloc_handler)();
		_result = realloc(__p, __n);
		if (__result)
			return (__result);
	}
}

typedef __malloc_alloc_template<0> malloc_alloc;

template<class _Tp,class _Alloc>
class simple_alloc {
public:
	static _Tp* allocate(size_t __n) {
		return 0 == __n ? 0 : (_Tp*)_Alloc::allocate(__n * sizeof(_Tp));
	}

	static _Tp* allocate(void) {
		return (_Tp*)_Alloc::allocate(sizeof(_Tp));
	}

	static void deallocate(_Tp* __p,size_t __n) {
		if (0 != __n) {
			_Alloc::deallocate(__p, __n * sizeof(_Tp));
		}
	}

	static void deallocate(_Tp* __p) {
		_Alloc::deallocate(__p, sizeof(_Tp));
	}
};

template <bool threads,int inst>
class __default_alloc_template {
private:
#if ! (defined(__SUNPRO_CC) || defined(__GNUC__))
	enum { _ALIGN = 8 };
	enum { _MAX_BYTES = 128 };
	enum { _NFREELISTS = 16 }; // _MAX_BYTES/_ALIGN
# endif

	//将bytes上调至8的倍数
	static size_t _S_round_up(size_t __bytes) {
		return (((bytes)+(size_t)_ALIGN - 1) & ~((size_t)_ALIGN - 1));
	}

__PRIVATE:
	union  _Obj {
		union _Obj* _M_free_list_link;
		char _M__client_data[1];
	};

private:
# if defined(__SUNPRO_CC) || defined(__GNUC__) || defined(__HP_aCC)
	static _Obj* __STL_VOLATILE _S_free_list[];
# else
	//定义16个free_list
	static _Obj* __STL_VOLATILE _S_free_list[_NFREELISTS];
# endif

	//根据要求的区块大小，返回决定使用的free_list所在数组的位置
	static size_t _S_free_list_index(size_t __bytes) {
		return (((__bytes)+(size_t)_ALIGN - 1) / (size_t)_ALIGN - 1);
	}

	static void* _S_refill(size_t __n);
	//分配__nojbjs个size的区块，有可能根据实际情况数目可能会少于__nobjs
	static void* _S_chunk_alloc(size_t __size, int& __nobjs);

	//记录内存池的范围，但是只在_S_chunk_alloc()中起作用
	static char* _S_start_free;
	static char* _S_end_free;
	static size_t _S_heap_size;

# ifdef __STL_THREADS
	static _STL_mutex_lock _S_node_allocator_lock;
# endif

	class _Lock;
	friend class _Lock;
	class _Lock {
	public:
		_Lock() { __NODE_ALLOCATOR_LOCK; }
		~_Lock() { __NODE_ALLOCATOR_UNLOCK; }
	};

public:
	static void* allocate(size_t __n) {
		void* __ret = 0;
		//如果要求大小大于128字节，那么转去用第一级配置器
		if (__n > (size_t)_MAX_BYTES) {
			__ret = malloc_alloc::allocate(__n);
		}
		else {
			_Obj* __STL_VOLATILE* __my_free_list = _S_free_list + _S_free_list_index(__n);

		}
#     ifndef _NOTHREADS
		/*REFERENCED*/
		_Lock __lock_instance;
#     endif
		_Obj* __RESTRICT __result = *__my_free_list;
		//若该free_list没有可用区块，那么就重新填充该free_list
		if (__result == 0)
			__ret = _S_refill(_S_round_up(__n));
		//每次取链表的第一个区块
		else {
			*__my_free_list = __result->_M_free_list_link;
			__ret = __result;
		}
		return __ret;
	}
	


	//p不可能是0
static void deallocate(void* __p, size_t __n) {
	if (__n > (size_t)_MAX_BYTES)
		__ret = malloc_alloc::deallocate(__p, __n);
	//直接将该区块归还free_list
	else {
		_Obj* __STL_VOLATE* __my_free_list = _S_free_list + _S_free_list_index(__n);
		_Obj* __q = (_Obj*)__p;

		// acquire lock
#       ifndef _NOTHREADS
		/*REFERENCED*/
		_Lock __lock_instance;
#       endif /* _NOTHREADS */

		__q->_M_free_list_link = *__my_free_list;
		*__my_free_list = __q;
	}
}
static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz);

};

typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
typedef __default_alloc_template<false, 0> single_client_alloc;

template <bool __threads,int __inst>
void* __default_alloc_template<__threads, __inst>::_S_refill(size_t n) {
	int __nobjs = 20;
	//__nobjs是传引用
	char* __chunk = _S_chunk_alloc(__n, __nobjs);
	_Obj* __STL_VOLATILE* __my_free_list;
	_Obj* __result;
	_Obj* _current_obj;
	_Obj* __next_obj;
	int __i;

	//如果是只从内存池里面获得一个区块，那么直接返回给调用者
	if (1 == __nobjs)
		return __chunk;
	__my_free_list = _S_free_list + _S_free_list_index(__n);

	//利用分配来的chunk来构造free_list
	__reault = (_Obj*)__chunk;
	*__my_free_list = __next_obj = (_Obj*)(__chunk + n);
	for (__i = 1;; __i++) {
		_current_obj = __next_obj;
		__next_obj = (_Obj*)((char*)__next_obj + __n);
		if (__nobjs - 1 == __i) {
			_current_obj->_M_free_list_list = 0;
			break;
		}
		else {
			_current_obj->_M_free_list_list = __next_obj;
		}
	}
	return __result;

}

//__nobjs是传的是引用
template <bool __threads,int __inst>
char* __default_alloc_template<__threads, __inst>::_S_chunk_alloc(size_t __size,
	int& __nobjs) {
	char* __result;
	//所需的空间
	size_t __total_bytes = __size * __nobjs;
	//内存池剩余空间
	size_t __bytes_left = _S_end_free - _S_start_free;

	//内存池剩余空间大于所需空间
	if (__bytes_left >= __total_bytes) {
		__result = _S_start_free;
		_S_start_free += __total_bytes;
		return __result;
	}
	//剩余空间仅能提供一个或以上的区块
	else if (__bytes_left >= __size) {
		__nobjs = (int)(__bytes_left / __size);
		__total_bytes = __size * __nobjs;
		__result = _S_start_free;
		_S_start_free += __total_bytes;
		return __result;
	}else{
		size_t __bytes_to_get = 2 * __total_bytes + _S_round_up(_S_heap_size >> 4);
		//将残余空间利用起来
		if (__bytes_left > 0) {
			_Obj* __STL_VOLATILE* __my_free_list =
				_S_free_list + _S_freelist_index(__bytes_left);
			((_Obj*)_S_start_free)->_M_free_list_link = *__my_free_list;
			*__my_free_list = (_Obj*)_S_start_free;
		}
		//利用malloc为内存池重新构造空间
		_S_start_free = (char*)malloc(__bytes_to_get);
		//heap空间不足，malloc失败
		if (0 == _S_start_free) {
			size_t __i;
			_Obj* __STL_VOLATILE* __my_free_list;
			_Obj* __p;
			for (__i = __size; __i <= (size_t)_MAX_BYTES; __i += (size_t)_ALIGN) {
				__my_free_list = _S_free_list + _S_freelist_index(__i);
				__p = *__my_free_list;
				if (0 != __p) {
					*__my_free_list = __p->_M_free_list_link;
					_S_start_free = (char*)__p;
					_S_end_free = _S_start_free + __i;
					return _S_chunk_alloc(__size, __nobjs);
				}
			}
			_S_end_free = 0;
			_S_start_free = (char*)malloc_alloc::allocate(__bytes_to_get);
		}
		_S_heap_size += __bytes_to_get;
		_S_end_free = _S_start_free + __bytes_to_get;
		return _S_chunk_alloc(__size, ___nobjs);
	}
}
