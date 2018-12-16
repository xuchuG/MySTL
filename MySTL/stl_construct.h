#pragma once
#include <new.h>

//第一层，第一层和第二层的函数参数都是一样的，仍要分两层的目的在于第一层作为接口，具体实现从第二层开始，类似于event与eventImpl的联系一样
template <class _T1,class _T2>
inline void construct(_T1* __p, const _T2& __value) {
	_Contruct(__p, __value);
}
template <class _T1>
inline void construct(_T1* __p) {
	_Construct(__p);
}
template <class _Tp>
inline void destroy(_Tp* __pointer) {
	_Destroy(__pointer);
}
template <class _ForwardIterator>
inline void destroy(_ForwardIterator __first, _ForwardIterator __last) {
	_Destroy(__first, __last);
}

//第二层
template <class _T1,class _T2>
inline void  _Construct(_T1* __p, const _T2& __value) {
	new ((void*)__p) _T1(__value);
}
template <class _T1>
inline void _Contruct(_T1* __p) {
	new ((void*)__p) _T1();
}
template <class _Tp>
inline void _Destroy(_Tp* __pointer) {
	__pointer->~_Tp();
}
template <class _ForwardIterator>
inline void _Destroy(_ForwardIterator __first, _ForwardIterator __last) {
	__destroy(__first, __last, __VALUE_TYPE(__first));
}

//在第二层特化 char* int* long* float*,这些基本类型的指针析构并不需要做什么操作
inline void _Destroy(char*,char*){}
inline void _Destroy(int*,int*){}
inline void _Destroy(long*,long*){}
inline void _Destroy(float*,float*){}
inline void _Destroy(double*,double*){}
#ifdef __STL_HAS_WCHAR_T
inline void _Destroy(wchar_t*, wchar_t*) {}
#endif /* __STL_HAS_WCHAR_T */

//第三层
template <class _ForwardIterator,class _Tp>
inline void __destroy(_ForwardIterator __first, _ForwardIterator __last, _Tp*) {
	typedef typename __type_traits<_Tp>::has_trivial_destructor _Trivial_destructor;
	__destory_aux(__first,__last,_Trivial_destructor());
}

//第四层
//1.没有重要的析构函数
template <class _ForwardIterator>
inline void __destroy_aux(_ForwardIterator,_ForwardIterator,__true_type){}

//2.有重要的析构函数
template <class _ForwardIterator>
inline void __destroy_aux(_ForwardIterator __first, _ForwardIterator __last, __false_type) {
	for (; __first != __last; ++__first) {
		destroy(&*__first);
	}
}




