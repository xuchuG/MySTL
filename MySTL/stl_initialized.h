#pragma once
#include <string.h>

//!!!!!!!!!!!!!!���ڿ�������Ϳ�����ֵ��Ч�ʴ��ڲ��죨��ʼ���б���ֵ��Ҫ������ֵ�Ķ���ָ��Ŀռ�������Ȼ���ڹ���һ�����Ը�ֵ�Ķ��󣩣����Ծ�����__uninitialized_fill_n��fill_n��
//!!!    fill�������������ʼ��n����ֵͬ�������copy��֮

//��һ��
template <class _ForwardIter,class _Size,class _Tp>
inline _ForwardIter uninitialized_fill_n(_ForwardIter __first, _Size __n,const _Tp& __x) {
	return __uninitialized_fill_n(__first, __n,__x, __VALUE_TYPE(__x));
}

//�ڶ���,_Tp1Ϊ__x����ʵ����
template <class _ForwardIter,class _Size,class _Tp,class _Tp1>
inline _ForwardIter __uninitialized_fill_n(_ForwardIter __first, _Size __n,const _Tp& __x, _Tp1*) {
	typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
	return __uninitialized_fill_n_aux(__first, __n, __x, _Is_POD());
}

//������
//����POD���ͣ�POD����Ϊ�������ͻ�ͳ��C struct���ͣ�POD�ͱ�ӵ�в���Ҫ�Ĺ��죬��������
template <class _ForwardIter,class _Size,class _Tp>
inline _ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n, const _TP& __x, __true_type){
	//���ø߽׺���
	return fill_n(__first, __n, __x);
}

//������POD����
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n, const _TP& __x, __false_type) {
	_ForwardIter __cur = __first;
	__STL_TRY{
		for (; __n > 0; --n,++__cur) {
			_Construct(&*__cur, __x);
		}
	}
	__STL_UNWIND(_Destroy(__first, __cur));
}



//��һ��  
template <class _InputIter,class _ForwardIter>
inline _ForwardIter uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __rsult) {
	return __uninitialized_copy(__first, __last, __result, __VALUE_TYPE(__result));
}

//�ڶ���
template <class _InputIter,class _ForwardIter,class _Tp>
inline _ForwardIter __uninitlialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result,_Tp*) {
	typedef typename __type_traits<_TP>::is_POD_type _Is_POD;
	return __uninitlialized_copy_aux(__first, __last, __result, _Is_POD());
}

//������,
//����POD����
template <class _InputIter,class _ForwardIter>
inline _ForwardIter __uninitlialized_copy_aux(_InputIter __first, _InputIter __last, _ForwardIter __result, __true_type) {
	return copy(__first, __last, __result);
}
//������POD����
template <class _InputIter, class _ForwardIter>
inline _ForwardIter __uninitlialized_copy_aux(_InputIter __first, _InputIter __last, _ForwardIter __result, __true_type) {
	_ForwardIter __cur = __result;
	__STL_TRY{
		for (; __first != __last; __first++,__cur++) {
			_Construct(&*__cur, *__first);
		}
	return __cur;
	}
	__STL_UNWIND(_Destroy(__result, __cur);
}


//�������� char*��wchar_t
inline char* uninitialized_copy(const char* __first, const char* __last, char* __result) {
	memmove(__result, __first, __last - __first);
	return __result + (__last - __first);
}
inline wchar_t* uninitialized_copy(const wchar_t* __first, const wchar_t* __last, wchar_t* __result) {
	memmove(__result, __first, sizeof(wchar_t)*(__last - __first));
	return __result + (__last - __first);
}


//��һ��
template <class _ForwardIter,class _Tp>
inline void uninitialized_fill(_ForwardIter __first, _ForwardIter __last, const _Tp& __x) {
	__uninitialized_fill(__first, __last, __x, __VALUE_TYPE(__x));
}

//�ڶ���
template <class _ForwardIter,class _Tp,class _Tp1>
inline void __uninitialized_fill(_ForwardIter __first, _ForwardIter __last, const _Tp& __x, _Tp1*) {
	typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
	__uninitialized_fill_aux(__first, __last, __x, _Is_POD());
}

//������
//����POD����
template <class _ForwardIter,class _Tp>
inline void __uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last,const _Tp& __x, _true_type) {
	fill(__first,__last,__x);
}

//������POD����
template <class _ForwardIter,class _Tp>
inline void __uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, const _Tp& __x, _false_type) {
	_ForwardIter __cur = __first;
	__STL_TRY{
		for (; __cur != __last; __cur++,) {
			_Contruct(&*__cur, __x);
		}
	}
	__STL_UNWIND(_Destroy(__first, __cur));
}