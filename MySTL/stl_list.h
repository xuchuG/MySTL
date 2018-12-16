#pragma once
#include <stddef.h>
#include "stl_iterator_base.h"

//使用struct而不使用class的原因可能是class会默认给你做很多事，比如默认构造函数
struct _List_node_base {
	_List_node_base* _M_next;
	_List_node_base* _M_prev;
};

template <class _Tp>
struct _List_node : public _List_node_base {
	_Tp _M_data;
};

struct _List_iterator_base {
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef bidirectional_iterator_tag iterator_category;

	_List_node_base* _M_node;

	_List_iterator_base(_List_node_base* __x):_M_node(__x){}
	_List_iterator_base(){}

	void _M_incr() {
		_M_node = _M_node->_M_next;
	}
	void _M_decr() {
		_M_node = _M_node->_M_prev;
	}

	bool operator==(const _List_iterator_base& __x)const {
		return _M_node == __x._M_node;
	}
	bool operator!=(const _List_iterator_base& __x)const {
		return _M_node != __x._M_node;
	}
};

template <class _Tp,class _Ref,class _Ptr>
struct _List_iterator : public _List_iterator_base {
	typedef _List_iterator<_Tp, _Tp&, _Tp*> iterator;//这种定义是否存在递归
	typedef _List_iterator<_Tp, const _Tp&, _Tp*> const_iterator;
	typedef _List_iterator<_Tp, _Ref, _Ptr> _Self;

	typedef _Tp value_type;
	typedef _Ptr pointer;
	typedef _Ref reference;
	typedef _List_node<_Tp> _Node;

	_List_iterator(_Node* __x):_List_iterator_base(__x){}
	_List_iterator(){}
	_List_iterator(const iterator& __x):_List_iterator_base(__x){}

	reference operator*()const {
		return ((_Node*)_M_node)->_M_data;
	}
#ifndef __SGI_STL_NO_ARROW_OPERATOR
	pointer operator->() const { return &(operator*()); }//还是不理解为什么这么写--->解释，对于->操作符，只要它没找到最终的指针，它允许被重复递归的调用 
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

	_Self& operator++() {
		this->_M_incr();
		return *this;
	}
	_Self operator++(int) {
		_Self __tmp = *this;
		this->_M_incr();
		return __tmp;
	}

	_Self& operator--() {
		this->_M_decr();
		return *this;
	}
	_Self operator--(int) {
		_Self __tmp = *this;
		this->_M_decr();
		return __tmp;
	}
};


//考虑到兼容性，兼顾了三种类型的分配器，1.普通的符合标准的分配器，2.符合标准的分配器没有非静态数据的分配器，3.SGI风格的分配器

#ifdef __STL_USE_STD_ALLOCATORS
template <class _Tp,class _Allocator,bool _IsStatic>//第一种分配分配器
class _List_alloc_base {
public:
	typedef typename _Alloc_trait<_Tp, _Allocator>::allocator_type allocator_type;
	allocator_type get_allocator()const {
		return _Node_allocator;
	}
	_List_alloc_base(const allocator_type& __a):_Node_allocator(__a){}

protected:
	_List_node<_Tp>* _M_get_node() {
		return _Node_allocator.allocate(1);
	}
	void _M_put_node(_List_node<_Tp*> __p) {
		return _Node_allocator.deallocate(__p, 1);
	}

protected:
	typname _Alloc_traits<_List_node<_Tp>, _Allocator>::allocator_type _Node_allocator;
	_List_node<_Tp>* _M_node;
};

//特化，第二种分配器，也就是有静态的分配器，不需要创建变量了
template <class _Tp,class _Allocator>
class _List_alloc_base<_Tp, _Allocator, true> {
public:
	typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type allocator_type;
	allocator_type get_allocator()const {
		return allocator_type();
	}
	_List_alloc_base(const allocator_type&){}

protected:
	typedef typename _Alloc_traits<_List_node<_Tp>, _Allocator>::_Alloc_type _Alloc_type;
	_List_node<_Tp*> _M_get_node() {
		return _Alloc_type::allocate(1);
	}
	void _M_put_node(_List_node<_Tp>* __p) {
		_Alloc_type::deallocate(__p, 1);
	}

protected:
	_List_node<_Tp>* _M_node;
};

template <class _Tp,class _Alloc>
class _List_base : public _List_alloc_base<_Tp, _Alloc, _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
public:
	typedef _List_alloc_base<_Tp, _Alloc, _Alloc_tratis<_Tp, _Alloc>::_S_instanceless> _Base;
	typedef typename _Base::allocator allocator_type;

	_List_base(const allocator_type& __a) :_Base(__a) {
		_M_node = _M_get_node();
		_M_node->_M_next = _M_node;
		_M_node->_M_prev = _M_node;
	}
	~_List_base() {
		clear();
		_M_put_node(_M_node);
	}

	void clear();
};
#else/* __STL_USE_STD_ALLOCATORS */
template <class _Tp,class _Alloc>//第三种类型
class _List_base {
public:
	typedef _Alloc allocator_type;
	allocator_type get_allocator()const {
		return allocator_type();
	}

	_List_base(const allocator_type&)//允许外部传入分配器，但不是在这起作用
	{
		_M_node = _M_get_node();//第一个节点不放数据,为了形成前闭后开的区间
		_M_node->_M_next = _M_node;
		_M_node->_M_prev = _M_node;
	}
	~_List_base() {
		clear();
		_M_put_node(_M_node);//由于第一个节点不放数据，所以不需要_Destroy
	}
	void clear();
	
protected:
	typedef simple_alloc<_List_node<_Tp>, _Alloc> _Alloc_type;//在这起作用
	_List_node<_Tp>* _M_get_node() {
		return _Alloc_type::allocator(1);
	}
	void _M_put_node(_List_node<_Tp>* __p) {
		_Alloc_type::deallocate(__p, 1);
	}
protected:
	_List_node<_Tp*> _M_node;
};


#endif/* __STL_USE_STD_ALLOCATORS */

template <class _Tp,class _Alloc>
void _List_base<_Tp, _Alloc>::clear() {
	_List_node<_Tp>* __cur = (_List_node<_Tp>*)_M_node->_M_next;
	while (__cur != _M_node) {
		_List_node<_Tp>* __tmp = __cur;
		__cur = (_List_node<_Tp>*)__cur->_M_next;
		_Destory(&__tmp->_M_data);
		_M_put_node(__tmp);
	}
	_M_node->_M_next = _M_data;
	_M_node->_M_prev = _M_data;
}

//用户使用的list的定义
template <class _Tp,class _Alloc=__STL_DEFAULT_ALLOCATOR(_Tp)>
class list : protected _List_base<_Tp, _Alloc> {
	__STL_CLASS_REQUIRES(_Tp, _Assignable);

	typedef _List_base<_Tp, _Alloc> _Base;
protected:
	typedef void* _Void_Pointer;

public:
	typedef _Tp value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type const_reference;
	typedef _List_node<_Tp> _Node;
	typedef size_t size_type;
	typedef ptrdiff_t difference_typ;

	typedef typename _Base::allocator_type allocator_type;

	allocator_type get_allocator()const {
		return _Base::get_allocator();
	}

public:
	typedef _List_iterator<_Tp, _Tp&, _Tp*> iterator;
	typedef _List_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
	typedef reverse_iterator<const_iterator> const_reverse_iterator;
	typedef reverse_iterator<iterator>       reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
	typedef reverse_bidirectional_iterator<const_iterator, value_type,
		const_reference, difference_type>
		const_reverse_iterator;
	typedef reverse_bidirectional_iterator<iterator, value_type, reference,
		difference_type>
		reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:
#ifdef __STL_HAS_NAMESPACES
	using _Base::_M_node;
	using _Base::_M_put_node;
	using _Base::_M_get_node;

#endif //__STL_HAS_NAMESPACES

protected:
	_Node* _M_create_node(const _Tp& __x) {
		_Node* __p = _M_get_node();
		__STL_TRY{
			_Contruct(&__p->_M_data,__x);
		}
		__STL_UNWIND(_M_put_node(__p));
	}

	_Node* _M_create_node() {
		_Node* __p = _M_get_node();
		__STL_TRY{
			_Contruct(&__p->_M_data);
		}
		__STL_UNWIND(_M_put_node(__p));
	}

public:
	explicit list(const allocator_type __a = allocator_type()):_Base(__a){}
	iterator begin(){//如何应对list为空，然后调用该函数赋值的情况
		return (_Node*)(_M_node->_M_next);
	}
	const_iterator begin()const {
		return (_Node*)(_M_node->_M_next);
	}
	iterator end() {
		return (_Node*)(_M_node);
	}
	const_iterator end()const {
		return (_Node*)_M_node;
	}
	reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	bool empty()const {
		return _M_node->_M_next == _M_node;
	}
	size_type size()const {
		size_type __result = 0;
		distance(begin(), end(), __result);
		return __result;
	}
	size_type max_size()const {
		return size_type(-1);
	}

	reference front()const {
		return *begin();
	}
	const_reference front()const {
		return *begin();
	}
	reference back()const {
		return *(--end());
	}
	const_reference back()const {
		return *(--end());
	}
	void swap(_List<_Tp, _Alloc>& __x) {
		__STD::swap(_M_node, __x._M_node);
	}
	iterator insert(iterator __position, const _Tp& __x) {
		_Node* __tmp = _M_create_node(__x);
		__tmp->_M_next = __position->_M_node;
		__tmp->_M_prev = __position->_M_node->_M_prev;
		__position->_M_node->_M_prev->_M_next = __tmp;
		__position->_M_node->_M_prev = __tmp;
		return __tmp;
	}
	iterator insert(iterator __position) {
		return insert(__position, _Tp());
	}

#ifdef __STL_MEMBER_TEMPLATES
	//第一层,可能是其他类型容（vector,Stack）器的的迭代器
	template <class _InputIterator>
	void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {//在这里并没有判断_InputIterator指向的数据是否是_Tp类型的，这是否存在风险？
		typedef typename _Is_integer<_InputIterator>::_Itergral _Integral;
		_M_insert_dispatch(__pos, __first, __last, _Integral());
	}

	//第二层
	template <class _Integer>
	void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x, true_type) {
		_M_fill_insert(_pos, (size_type)__n, (_Tp)__x);
	}
	template <class _InputIterator>
	void _M_insert_dispatch(iterator __pos, _InputIterator __first, _InputIterator __last, false_type) {
		for (; __first != __last; __first++) {
			insert(__pos, *__first);//插入完成后，所有元素都在_pos之前
		}
	}

#else /* __STL_MEMBER_TEMPLATES */
	//普通指针，也得一个个插入
	void insert(iterator __position, const _Tp* __first, const _Tp* __last) {
		for (; __first != __last; __first++) {
			insert(__position, *__first);
		}
	}
	//为什么不将这两个合并，明明是一样的形式
	void insert(iterator __position, const_iterator __first, const_iterator __last) {
		for (; __first != __last; __first++) {
			insert(__position, *__first);
		}
	}
#endif/* __STL_MEMBER_TEMPLATES */

	void insert(iterator __pos, size_type __n, const _Tp& __x) {
		__M_fill_insert(__pos, __n, __x);
	}


//第338行的第三层
	void _M_fill_insert(iterator __pos, size_type __n, const _Tp& __x) {
		for (; __n > 0; __n--) {
			insert(__pos,__x);
		}
	}

	void push_front(const _Tp& __x) {
		insert(begin(), __x);
	}
	void push_front() {
		insert(begin(),_Tp());
	}
	void push_back(const _Tp& __x) {
		insert(end(), __x);
	}
	void push_back() {
		insert(end(), _Tp());
	}

	iterator erase(iterator __postion) {
		_List_node_base* __next_node = __position->_M_node->_M_next;
		_List_node_base* __prev_node = __position->_M_node->_M_prev;
		_Node* __n = (*_Node)__position->_M_node;
		__prev_node->_M_next = __next_node;
		__next_node->_M_prev = __prev_node;
		_Destroy(&__n->_M_data);
		_M_put_node(__n);
		return iterator((_Node*)__next_node);
	}
	void clear() {
		_Base::clear();
	}

	void resize(size_type __new_size,const _Tp& __x){
		iterator __i = begin();
		size_type __len = 0;
		for (; __i != end() && __len < __new_size; ++__i, __len++) {

		}
		if (__len == __new_size) {
			erase(__i, end());
		}
		else {
			insert(end(), __new_size - __len, __x);
		}
	}
	void resize(size_type __new_size) {
		resize(__new_size, _Tp());
	}

	void pop_front() {
		earse(begin());
	}
	void pop_back() {
		iterator __tmp = end();
		erase(--__tmp);
	}

	list(size_type __n, const _Tp& __value,
		const allocator_type& __a = allocator_type())
		: _Base(__a)
	{
		insert(begin(), __n, __value);
	}
	explicit list(size_type __n)
		: _Base(allocator_type())
	{
		insert(begin(), __n, _Tp());
	}

	//暂时到435行，未完待续
};
