#pragma once
//push_heap�㷨,array�е�һ��Ԫ��Ҳ����ֵ
//���һ

//��һ��,���ʱ����Ԫ���Ѿ��ŵ����һ��λ����
template <class _RandomAccessIterator>
inline void push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last) {
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__STL_REQUIRES(typename iterator_traits<_RandomAccessIterator>::value_type, _LessThanComparable);
	__push_heap_aux(__first, __last, __DISTANCE_TYPE(__first), __VALUE_TYPE(__first));
}

//�ڶ���
template <class _RandomAccessIterator,class _Distance,class _Tp>
inline void __push_heap_aux(_RandomAccessIterator __first, _RandomAccessIterator __last, _Distance*, _Tp*) {
	__push_heap(__first, _Distance((__last - __first) - 1), _Distance(0), _Tp(*(__last - 1)));
}

//�����㣬���ݹ���
template<class _RandomAccessIterator,class _Distance,class _Tp>
void __push_heap(_RandomAccessIterator __first, _Distance __holeIndex, _Distance __topIndex, _Tp __value) {
	_Distance __parent = (__holeIndex - 1) / 2;
	while (__holeIndex > __topIndex && *(__first + __parent) < __value) {
		*(__first + __holeIndex) = *(__first + __parent);
		__holeIndex = __parent;
		__parent = (__holeIndex - 1) / 2;
	}
	*(__first + __holeIndex) = value;
}

//��������Դ��Ƚ�����push_heap
//��һ��
template <class _RandomIterator,class _Comp>
inline void push_heap(_RandomIterator __first, _RandomIterator __last, _Comp __comp) {
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__push_heap_aux(__first, __last, __comp, _DISTANCE_TYPE(__first),_VALUE_TYPE(__first);
}
//�ڶ���
template <class _RandomIterator,class _Comp,class _Distance,class _Tp>
inline void __push_heap_aux(_RandomIterator __first, _RandomIterator __last, _Comp __comp, _Distance*,_Tp*) {
	__push_heap(__first, _Distance(__last - __first - 1),_Distance(0), (_Tp)*(__last - 1), __comp);
}
//������
template <class _RandomIterator,class _Comp,class _Distance,class _Tp>
inline void __push_heap(_RandomIterator __first,  _Distance __holeIndex,_Distance __topIndex _Tp value,_Comp __comp) {
	_Distance __parent = (__holeIndex - 1) / 2;
	while (__holeIndex > __parent && __comp(*(__first + __parent),__value)) {
		*(__first + __holeIndex) = *(__first + __parent);
		__holeIndex = __parent;
		__parent = (__holeIndex - 1) / 2;
	}
	*(__first + __holeIndex) = value;
}


//pop_heap�㷨,����֮��pop��Ԫ��λ��array�����һ��λ��
//���һ
//��һ��
template <class _RandomIterator>
inline void pop_heap(_RandomIterator __first, _RandomIterator __last) {
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__STL_REQUIRES(typename iterator_traits<_RandomAccessIterator>::value_type,
		_LessThanComparable);
	__pop_heap_aux(__first, __last, __VALUE_TYPE(__first));
}

//�ڶ���
template <class _RandomIterator,class _Tp>
inline void pop_heap_aux(_RandomIterator __first, _RandomIterator __last, _Tp*) {
	__pop_heap(__first, __last - 1, __last - 1, _Tp(*(__last - 1)), __DISTANCE_TYPE(__first));
}

//������
template <class _RandomIterator,class _Tp,class _Distance>
inline void pop_heap(_RandomIterator __first, _RandomIterator __last, _RandomIterator __result,
	_Tp __value, _Distance*) {
	*__result = *__first;
	__adjust_heap(__first, _Distance(0), _Distance(__last - __first), __value);
}
//���Ĳ�
template <class _RandomIterator,class _Tp,class _Distance>
inline void __adjust_heap(_RandomIterator __first, _Distance __holeIndex, _Distance __len,
	_Tp value) {
	_Distance __topIndex = __holeIndex;
	_Distance __secondChild = 2 * __holeIndex + 2;
	while (__secondChild < __len) {
		if (*(__first + __secondChild) < *(__first + __secondChild - 1))
			__secondChild--;
		*(first + __holeIndex) = *(first + __secondChild);
		__holeIndex = __secondChild;
		__secondChild = 2 * (__secondChild + 1);
	}
	if (__secondChild == len) {
		*(__first + __holeIndex) = *(__first + secondChild - 1);
		__holeIndex = __secondChild - 1;
	}
	__push_heap(__first, __holeIndex, __topIndex, __value);
}

//�����
//��һ��
template <class _RandomAccessIterator, class _Compare>
inline void
pop_heap(_RandomAccessIterator __first,
	_RandomAccessIterator __last, _Compare __comp)
{
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__pop_heap_aux(__first, __last, __VALUE_TYPE(__first), __comp);
}
//�ڶ���
template <class _RandomAccessIterator, class _Tp, class _Compare>
inline void
__pop_heap_aux(_RandomAccessIterator __first,
	_RandomAccessIterator __last, _Tp*, _Compare __comp)
{
	__pop_heap(__first, __last - 1, __last - 1, _Tp(*(__last - 1)), __comp,
		__DISTANCE_TYPE(__first));
}
//������
template <class _RandomAccessIterator, class _Tp, class _Compare,
	class _Distance>
	inline void
	__pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
		_RandomAccessIterator __result, _Tp __value, _Compare __comp,
		_Distance*)
{
	*__result = *__first;
	__adjust_heap(__first, _Distance(0), _Distance(__last - __first),
		__value, __comp);
}
//���Ĳ�
template <class _RandomAccessIterator, class _Distance,
	class _Tp, class _Compare>
	void
	__adjust_heap(_RandomAccessIterator __first, _Distance __holeIndex,
		_Distance __len, _Tp __value, _Compare __comp)
{
	_Distance __topIndex = __holeIndex;
	_Distance __secondChild = 2 * __holeIndex + 2;
	while (__secondChild < __len) {
		if (__comp(*(__first + __secondChild), *(__first + (__secondChild - 1))))
			__secondChild--;
		*(__first + __holeIndex) = *(__first + __secondChild);
		__holeIndex = __secondChild;
		__secondChild = 2 * (__secondChild + 1);
	}
	if (__secondChild == __len) {
		*(__first + __holeIndex) = *(__first + (__secondChild - 1));
		__holeIndex = __secondChild - 1;
	}
	__push_heap(__first, __holeIndex, __topIndex, __value, __comp);
}

//make_heap�㷨
//���һ
//��һ��
template <class _RandomIterator>
inline void make_heap(_RandomIterator __first, _RandomIterator __last) {
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__STL_REQUIRES(typename iterator_traits<_RandomAccessIterator>::value_type,
		_LessThanComparable);
	__make_heap(__first, __last, __VALUE_TYPE(__first), _DISTANCE_TYPE(__first));
}
//�ڶ���
template <class _RandomIterator,class _Tp,class _Distance>
inline void __make_heap(_RandomIterator __first, _RandomIterator __last, _Tp*, _Distance*) {
	if (__last - __first < 2)
		return;
	_Distance __len = __last - __first;
	_Distance __parent = (__len - 2) / 2;
	while (true) {
		__adjust_heap(__first, __parent, __len, _Tp(*(__first + __parent)));
	}
}

//�����
//��һ��
template <class _RandomAccessIterator, class _Compare>
inline void
make_heap(_RandomAccessIterator __first,
	_RandomAccessIterator __last, _Compare __comp)
{
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__make_heap(__first, __last, __comp,
		__VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
}

//�ڶ���
template <class _RandomAccessIterator, class _Compare,
	class _Tp, class _Distance>
	void
	__make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
		_Compare __comp, _Tp*, _Distance*)
{
	if (__last - __first < 2) return;
	_Distance __len = __last - __first;
	_Distance __parent = (__len - 2) / 2;

	while (true) {
		__adjust_heap(__first, __parent, __len, _Tp(*(__first + __parent)),
			__comp);
		if (__parent == 0) return;
		__parent--;
	}
}

//sort_heap�㷨
//���һ
template <class _RandomAccessIterator>
void sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__STL_REQUIRES(typename iterator_traits<_RandomAccessIterator>::value_type,
		_LessThanComparable);
	while (__last - __first > 1)
		pop_heap(__first, __last--);
}

//�����
template <class _RandomAccessIterator, class _Compare>
void
sort_heap(_RandomAccessIterator __first,
	_RandomAccessIterator __last, _Compare __comp)
{
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	while (__last - __first > 1)
		pop_heap(__first, __last--, __comp);
}

