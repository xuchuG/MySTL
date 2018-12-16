#pragma once


struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag : public input_iterator_tag{};
struct bidirectional_iterator_tag : public forward_iterator_tag{};
struct random_access_iterator_tag : public bidirectional_iterator_tag{};

#ifdef __STL_USE_NAMESPACES
template <class _Category,class _Tp,class _Distance=ptrdiff_t,class _Pointer=_Tp*,class _Reference=_Tp&>
struct iterator {
	typedef _Category iterator_category;
	typedef _Tp value_type;
	typedef _Distance difference_type;
	typedef _Pointer pointer;
	typedef _Reference reference;
};

#endif

template<class _Iterator>
struct iterator_traits {
	typedef typename _Iterator::iterator_category iterator_category;
	typedef typename _Iterator::value_type value_type;
	typedef typename _Iterator::difference_type difference_type;
	typedef typename _Iterator::pointer pointer;
	typedef typename _Iterator::reference reference;
};

//两个指针的特化版
template<class _Tp*>
struct iterator_traits {
	typedef random_access_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef ptrdiff_t difference_type;
	typedef _Tp* pointer;
	typedef _Tp& reference;
};

template<class const _Tp*>
struct iterator_traits {
	typedef random_access_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef ptrdiff_t difference_type;
	typedef const _Tp* pointer;
	typedef const _Tp& reference;
};


//第一层
template <class _Iter>
inline typename iterator_traits<_Iter>::iterator_category iterator_category(const _Iter& __i) {
	return __iterator_category(__i);
}
//第二层
//这些目的是把typedef的语句包装起来
template <class _Iter>
inline typename iterator_traits<_Iter>::iterator_category __iterator_category(const _Iter&) {
	typedef typename iterator_tratis<_Iter>::iterator_category _Category;
	return _Category();
}

//第一层
template <class _Iter>
inline typename iterator_traits<_Iter>::difference_type* iterator_difference(const _Iter& __i) {
	return __iterator_difference(__i);
}
//第二层
template <class _Iter>
inline typename iterator_traits<_Iter>::difference_type* __iterator_difference(const _Iter&) {
	return static_cast<typename iterator_traits<_Iter>::difference_type*>(0);
}

//第一层
template<class _Iter>
inline typename iterator_traits<_Iter>::value_type* value_type(const _Tp* __i) {
	return __value_type(__i);
}
//第二层
template <class _Iter>
inline typename iterator_traits<_Iter>::value_type* __value_type(const _Iter&) {
	return static_cast<typename iterator_traits<_Iter>::value_type*>(0);
}


//第一层,之所以参数命名为_InputIterator，以算法接受之最初级级别来为其迭代器类型参数命名
template <class _InputIterator,class _Distance>
inline void distance(_InputIterator __first, _InputIterator __last, _Distance& __n) {
	__STL_REQUIRES(_InputIterator, _InputIterator);
	__distance(__first, __last, __n, iterator_category(__first));
}
//第二层
template <class _InputIterator,class _Distance>
inline void __distance(_InputIterator __first, _InputIterator __last, _Distance& __n, input_iterator_tag) {
	while (__first != __last) {
		++__first;
		++__n;
	}
}

template <class _InputIterator,class _Distance>
inline void __distance(_InputIterator __first, _InputIterator __last, random_access_iterator_tag) {
	__STL_REQUIRES(_RandomAccessIterator, _RandomAccessIterator);
	__n = __last - __first;
}

//第一层
template <class _InputIterator,class _Distance>
inline void advance(_InputIterator& __i, _Distance __n) {
	__STL_REQUIRES(_InputIterator, _InputIterator);
	__advance(__i, __n, iterator_category(__i));
}
//第二层
template <class _InputIterator,class _Distance>
inline void  __advance(_InputIterator __i, _Distance __n, input_iterator_tag) {
	while (n--) {
		++__i;
	}
}

template <class _InputIterator,class _Distance>
inline void __advance(_InputIterator __i, _Distance __n, bidirectional_iterator_tag) {
	if (__n >= 0) {
		while (__n--) {
			++__i;
		}
	}
	else {
		while (__n++) {
			--_i;
		}
	}
}

template <class _InputIterator,class _Distance>
inline void __advance(_InputIterator __i, _Distance __n, random_access_iterator_tag) {
	__i += __n;
}