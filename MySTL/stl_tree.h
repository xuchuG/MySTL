#pragma once
#include "stl_iterator_base.h"
#include <stddef.h>

typedef bool _Rb_tree_Color_type;
const _Rb_tree_Color_type _S_rb_tree_red = false;
const _Rb_tree_Color_type _S_rb_tree_black = true;

//在base里只有关于指针的信息,节点颜色信息
struct _Rb_tree_node_base {
	typedef _Rb_tree_Color_type _Color_type;
	typedef _Rb_tree_node_base* _Base_ptr;

	_Color_type _M_color;
	_Base_ptr _M_parent;
	_Base_ptr _M_left;
	_Base_ptr _M_right;

	static _Base_ptr _S_minimum(_Base_ptr __x) {
		while (__x->_M_left != 0)
			__x = __x->_M_left;
		return __x;
	}

	static _Base_ptr _S_maximum(_Base_ptr __x) {
		while (__x->_M_right != 0) {
			__x = __x->_M_right;
		}
		return __x;
	}

};
//保存有元素信息
template <class _Value>
struct _Rb_tree_node : public _Rb_tree_node_base {
	typedef _Rb_tree_node<_Value>* _Link_type;
	_Value _M_value_field;
};


//红黑树的迭代器
struct _Rb_tree_base_iterator {
	typedef _Rb_tree_node_base::_Base_ptr _Base_ptr;
	typedef bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;
	_Base_ptr _M_node;

	void _M_increment() {
		if (_M_node->_M_right != 0) {//有右孩子
			_M_node = _M_node->_M_right;
			while (_M_node->_M_left != 0)
				_M_node = _M_node->_M_left;
		}
		else {
			_Base_ptr __y = _M_node->_M_parent;
			while (_M_node == __y->_M_right) {//回溯到当前节点不是左孩子
				_M_node = __y;
				__y = _M_node->_M_parent;
			}
			if (_M_node->_M_right != __y) {//这个是为了处理一个特殊情况，想要找根节点的下一个节点，而根节点没有右孩子的情况
				_M_node = __y;
			}
		}
	}

	void _M_decrement() {
		if (_M_node->_M_color == _S_rb_tree_red &&
			_M_node->_M_parent->_M_parent == _M_node)//为了处理当前节点是header节点(即end()所指的位置，类似于list要多一个空白的节点)的时候，所以它的前一个节点应该是树中值最大的节点
			_M_node = _M_node->_M_right;
		else if (_M_node->_M_left != 0) {//若有左孩子
			_Base_ptr __y = _M_node->_M_left;
			while (__y->_M_right != 0)
				__y = __y->_M_right;
			_M_node = __y;
		}
		else {//没有左孩子
			_Base_ptr __y = _M_node->_M_parent;
			while (_M_node == __y->_M_left) {
				_M_node = __y;
				__y = __y->_M_parent;
			}
			_M_node = __y;
		}
	}

};

template <class _Value,class _Ref,class _Ptr>
struct _Rb_tree_iterator :public _Rb_tree_base_iterator {
	typedef _Value value_type;
	typedef _Ptr pointer;
	typedef _Ref reference;

	typedef _Rb_tree_iterator<_Value, _Value&, _Value*> iterator;
	typedef _Rb_tree_iterator<_Value, const _Value&, const _Value*> const_iterator;

	typedef _Rb_tree_iterator<_Value, _Ref, _Ptr> _Self;
	typedef _Rb_tree_node<_Value>* _Link_type;

	_Rb_tree_iterator(){}
	_Rb_tree_iterator(_Link_type __x) {
		_M_node = __x;
	}

	reference operator*()const {
		return _Link_type(_M_node)->_M_value_field;
	}

#ifndef __SGI_STL_NO_ARROW_OPERATOR
	pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

	_Self& operator++() {
		_M_increment();
		return *this;
	}

	_Self& operator++(int) {
		_Self __tmp = *this;
		_M_increment();
		return __tmp;
	}

	_Self& operator--() {
		_M_decrement();
		return *this;
	}
	_Self& operator--(int) {
		_Self __tmp = *this;
		_M_decrement();
		return __tmp;
	}
};

inline bool operator==(const _Rb_tree_base_iterator& __x,
	const _Rb_tree_base_iterator& __y) {
	return __x._M_node == __y._M_node;
}

inline bool operator!=(const _Rb_tree_base_iterator& __x,
	const _Rb_tree_base_iterator& __y) {
	return __x._M_node != __y._M_node;
}

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

inline bidirectional_iterator_tag
iterator_category(const _Rb_tree_base_iterator&) {
	return bidirectional_iterator_tag();
}

inline _Rb_tree_base_iterator::difference_type*
distance_type(const _Rb_tree_base_iterator&) {
	return (_Rb_tree_base_iterator::difference_type*) 0;
}

template <class _Value, class _Ref, class _Ptr>
inline _Value* value_type(const _Rb_tree_iterator<_Value, _Ref, _Ptr>&) {
	return (_Value*)0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */


//一般情况下，有6个指针发生了变化，即在四个节点之间联系，以旋转点__x为基准，四个节点分别__x，它的右子节点，它的右子节点的左子节点和它的父节点
inline void _Rb_tree_rotate_left(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root) {
	_Rb_tree_node_base* __y = __x->_M_right;
	__x->_M_right = __y->_M_left;
	if (__y->_M_left != 0)
		__y->_M_left->_M_parent = __x;
	if (__x == __root)
		__root = __y;
	else if (__x == __x->_M_parent->_M_left) {
		__x->_M_parent->_M_left = __y;
	}
	else
	{
		__x->_M_parent->_M_right = __y;
	}
	__y->_M_left = __x;
	__x->_M_parent = __y;
}

inline void _Rb_tree_rotate_right(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root) {
	_Rb_tree_node_base* __y = __x->_M_left;
	__x->_M_left = __y->_M_right;
	if (__y->_M_right != 0) {
		__y->_M_left->_M_parent = __x;
	}

	if (__x == __root) {
		__root = __y;
	}
	else if (__x->_M_parent->_M_left == __x) {
		__x->_M_parent->_M_left = __y;
	}
	else {
		__x->_M_parent->_M_right = __y;
	}
	__y->_M_parent = __x->_M_parent;
	__y->_M_right = __x;
	__x->_M_parent = __y;
}

//插入节点后调整节点颜色
inline void _Rb_tree_rebalance(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root) {
	//新插入节点颜色必为红色的
	__x->_M_color = _S_rb_tree_red;

	while (__x != __root && __x->_M_parent->_M_color == _S_rb_tree_red) {
		if (__x->_M_parent == __x->_M_parent->_M_parent->_M_left) {//为了辅助判断旋转方向
			_Rb_tree_node_base* __y = __x->_M_parent->_M_parent->_M_right;
			if (__y && __y->_M_color == _S_rb_tree_red) {//情况三和四，父节点的兄弟节点为红色，改变颜色，递归处理，直到变成其他情况不同于书上的做法
				__x->_M_parent->_M_color = _S_rb_tree_black;
				__y->_M_color = _S_rb_tree_black;
				__x->_M_parent->_M_parent->_M_color = _S_rb_tree_red;
				__x = __x->_M_parent->_M_parent;
			}
			else {//父节点的兄弟节点为黑色
				if (__x == __x->_M_parent->_M_right) {//情况二，为内侧插入，需要先左旋一次
					__x = __x->_M_parent;
					_Rb_tree_rotate_left(__x, __root);
				}
				//情况二的后续操作，先改变父节点和祖父节点的颜色，之后再右旋一次；同时它也是情况一的所有操作。所以情况二旋转一次就变成了情况一
				__x->_M_parent->_M_color = _S_rb_tree_black;
				__x->_M_parent->_M_parent->_M_color = _S_rb_tree_red;
				_Rb_tree_rotate_right(__x->_M_parent->_M_parent, __root);
			}
		}
		else {
			_Rb_tree_node_base* __y = __x->_M_parent->_M_parent->_M_left;
			if (__y && __y->_M_color == _S_rb_tree_red) {//同上
				__x->_M_parent->_M_color = _S_rb_tree_black;
				__y->_M_color = _S_rb_tree_black;
				__x->_M_parent->_M_parent->_M_color = _S_rb_tree_red;
				__x = __x->_M_parent->_M_parent;
			}
			else {
				if (__x == __x->_M_parent->_M_left) {//情况二，为内侧插入，需要先右旋一次
					__x = __x->_M_parent;
					_Rb_tree_rotate_right(__x, __root);
				}
				//情况二的后续操作，先改变父节点和祖父节点的颜色，之后再左旋一次；同时它也是情况一的所有操作。所以情况二旋转一次就变成了情况一
				__x->_M_parent->_M_color = _S_rb_tree_black;
				__x->_M_parent->_M_parent = _S_rb_tree_red;
				_Rb_tree_rotate_right(__x->_M_parent->_M_parent, __root);
			}
		}
	}
	__root->_M_color = _S_rb_tree_black;
}

//在红黑树中删除一个节点, 没看懂，需要以后再补充
inline _Rb_tree_node_base* _Rb_tree_rebalance_for_erase(_Rb_tree_node_base* __z,
	_Rb_tree_node_base*& __root, _Rb_tree_node_base*& __leftmost, _Rb_tree_node_base*& __rightmost) {
	  
}


//下面的定义类似于其他的容器的定义，根据配置器的不同分为了三种情况
#ifdef __STL_USE_STD_ALLOCATORS
template <class _Tp,class _Alloc,bool _S_instanceless>
class _Rb_tree_alloc_base {
public:
	typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;
	allocator_type get_allocator()const {
		return _M_node_allocator;
	}

	_Rb_tree_alloc_base(const allocator_type& __a):_M_node_allocator(__a),_M_header(0){}

protected:
	typename _Alloc_traits<_Rb_tree_node<_Tp>, _Alloc>::allocator_type _M_node_allocator_type;
	_Rb_tree_node<_Tp>* _M_header;

	_Rb_tree_node<_Tp>* _M_get_node() {
		return _M_node_allocator.allocate(1);
	}
	void _M_put_node(_Rb_tree_node<_Tp>* __p) {
		_M_node_allocator.deallocate(__p, 1);
	}
};

template <class _Tp, class _Alloc>
class _Rb_tree_alloc_base <_Tp,_Alloc,true>{
public:
	typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;
	allocator_type get_allocator()const {
		return	allocator_type();
	}

	_Rb_tree_alloc_base(const allocator_type&) : _M_header(0) {}

protected:
	typename _Alloc_traits<_Rb_tree_node<_Tp>, _Alloc>::_Alloc_type __Alloc_type;
	_Rb_tree_node<_Tp>* _M_header;

	_Rb_tree_node<_Tp>* _M_get_node() {
		return _Alloc_type::allocate(1);
	}
	void _M_put_node(_Rb_tree_node<_Tp>* __p) {
		_Alloc_type::deallocate(__p, 1);
	}
};

template <class _Tp, class _Alloc>
struct _Rb_tree_base
	: public _Rb_tree_alloc_base<_Tp, _Alloc,
	_Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
	typedef _Rb_tree_alloc_base<_Tp, _Alloc,
		_Alloc_traits<_Tp, _Alloc>::_S_instanceless>
		_Base;
	typedef typename _Base::allocator_type allocator_type;

	_Rb_tree_base(const allocator_type& __a)
		: _Base(__a) {
		_M_header = _M_get_node();
	}
	~_Rb_tree_base() { _M_put_node(_M_header); }

};

#else /* __STL_USE_STD_ALLOCATORS */

//该基类主要维护了header节点的构建和销毁
template <class _Tp,class _Alloc>
struct  _Rb_tree_base
{
	typedef _Alloc allocator_type;
	allocator_type get_allocator()const {
		return allocator_type();
	}

	_Rb_tree_base(const allocator_type&) :_M_header(0) {
		_M_header = _M_get_node();
	}
	~Rb_tree_base() { _M_put_node(_M_header); }

protected:
	_Rb_tree_node<_Tp>* _M_header;
	typedef simple_alloc<_Rb_tree_node<_Tp>, _Alloc> _Alloc_type;

	_Rb_tree_node<Tp>* _M_get_node() {
		return _Alloc_type::allocate(1);
	}
	void _M_put_node(_Rb_tree_node<Tp>* __p) {
		return _Alloc_type::deallocate(__p, 1);
	}

};

#endif // __STL_USE_STD_ALLOCATORS

template<class _Key,class _Value,class _KeyOfValue,class _Compare,class _Alloc= __STL_DEFAULT_ALLOCATOR(_Value)>
class _Rb_tree : protected _Rb_tree_base<_Value, _Alloc> {
	typedef _Rb_tree_base<_Value, _Alloc> _Base;

protected:
	typedef _Rb_tree_node_base* _Base_ptr;
	typedef _Rb_tree_node<Value> _Rb_tree_node;
	typedef _Rb_tree_Color_type _Color_type;
public:
	typedef _Key key_type;
	typedef _Value value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef _Rb_tree_node* _Link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename _Base::allocator_type allocator_type;
	allocator_type get_allocator() const { return _Base::get_allocator(); }

protected:
#ifdef __STL_USE_NAMESPACES
	using _Base::_M_get_node;
	using _Base::_M_put_node;
	using _Base::_M_header;
#endif // __STL_USE_NAMESPACES

protected:
	_Link_type _M_create_node(const value_type& __x) {
		_Link_type __tmp = _M_get_node();
		__STL_TRY{
			construct(&__tmp->_M_value_field,__x);
		}
		__STL_UNWIND(_M_put_node(__tmp));
		return __tmp;
	}
	_Link_type _M_clone_node(_Link_type __x) {
		_Link_type __tmp = _M_create_node(__x->_M_value_field);
		__tmp->_M_color = __x->_M_color;
		__tmp->_M_left = 0;
		__tmp->_M_right = 0;
		return __tmp;
	}

	void destroy_node(_Link_type __p) {
		destroy(&__p->_M_value_field);
		_M_put_node(__p);
	}

protected:
	size_type _M_node_count;
	_Compare _M_key_compare;

	_Link_type& _M_root()const {
		return (_Link_type&)_M_header->_M_parent;
	}
	_Link_type& _M_leftmost()const {
		return (_Link_type&)_M_header->left;
	}
	_Link_type& _M_rightmost()const {
		return (_Link_type&)_M_header->_M_right;
	}

	static _Link_type& _S_left(_Link_type __x)
	{
		return (_Link_type&)(__x->_M_left);
	}
	static _Link_type& _S_right(_Link_type __x)
	{
		return (_Link_type&)(__x->_M_right);
	}
	static _Link_type& _S_parent(_Link_type __x)
	{
		return (_Link_type&)(__x->_M_parent);
	}

	//这个value不是key-value中的value，而是它是一个pair，里面放在一个键值对
	static reference _S_value(_Link_type __x)
	{
		return __x->_M_value_field;
	}
	//这个比较特别
	static const _Key& _S_key(_Link_type __x)
	{
		return _KeyOfValue()(_S_value(__x));
	}
	static _Color_type& _S_color(_Link_type __x)
	{
		return (_Color_type&)(__x->_M_color);
	}

	static _Link_type& _S_left(_Base_ptr __x)
	{
		return (_Link_type&)(__x->_M_left);
	}
	static _Link_type& _S_right(_Base_ptr __x)
	{
		return (_Link_type&)(__x->_M_right);
	}
	static _Link_type& _S_parent(_Base_ptr __x)
	{
		return (_Link_type&)(__x->_M_parent);
	}
	static reference _S_value(_Base_ptr __x)
	{
		return ((_Link_type)__x)->_M_value_field;
	}
	static const _Key& _S_key(_Base_ptr __x)
	{
		return _KeyOfValue()(_S_value(_Link_type(__x)));
	}
	static _Color_type& _S_color(_Base_ptr __x)
	{
		return (_Color_type&)(_Link_type(__x)->_M_color);
	}

	static _Link_type _S_minimum(_Link_type __x)
	{
		return (_Link_type)_Rb_tree_node_base::_S_minimum(__x);
	}

	static _Link_type _S_maximum(_Link_type __x)
	{
		return (_Link_type)_Rb_tree_node_base::_S_maximum(__x);
	}

	public:
		typedef _Rb_tree_iterator<value_type, reference, pointer> iterator;
		typedef _Rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
		typedef reverse_iterator<const_iterator> const_reverse_iterator;
		typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
		typedef reverse_bidirectional_iterator<iterator, value_type, reference,
			difference_type>
			reverse_iterator;
		typedef reverse_bidirectional_iterator<const_iterator, value_type,
			const_reference, difference_type>
			const_reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */ 

		private:
			//先把插入的节点放在当前红黑树的叶子节点下，作为它的儿子节点，之后再调整红黑树
			iterator _M_insert(_Base_ptr __x_, _Base_ptr __y_, const value_type& __v) {
				_Link_type __x = (_Link_type)__x_;
				_Link_type __y = (_Link_type)__y_;
				_Link_type __z;

				if (__y == _M_header || __x != 0 ||
					_M_key_compare(_KeyOfValue()(__v), _S_key(__y))) {//__y的key大于__x的key
					__z = _M_create_node(__v);
					_S_left(__y) = __z;

					if (__y == _M_header) {
						_M_root() = __z;
						_M_rightmost() = __z;
					}
					else if (__y == _M_leftmost())
						_M_leftmost() = __z;
				}
				else {
					__z = _M_create_node(__v);
					_S_right(__y) = __z;
					if (__y == _M_rightmost()) {
						_M_rightmost() = _z;
					}
				}

				_S_parent(__z) = __y;
				_S_left(__z) = 0;
				_S_right(__z) = 0;
				_Rb_tree_rebalance(__z, _M_header->_M_parent);
				++_M_node_count;
				return iterator(__z);
			}

			_Link_type _M_copy(_Link_type __x, _Link_type __p) {
				_Link_type __top = _M_clone_node(__x);
				__top->_M_parent = __p;

				__STLF_TRY{
					if (__x->_M_right)
					__top->_M_right = _M_copy(_S_right(__x),__top);
					__p = __top;
					__x = _S_left(__x);

					while (__x != 0) {
						_Link_type __y = _M_clone_node(__x);
						__p->_M_left = __y;
						__y->_M_parent = __p;
						if (__x->_M_right)
							__y->_M_right = _M_copy(_S_right(__x), __y);
						__p = __y;
						__x = _S_left(__x);
					}
				}
				__STL_UNWIND(_M_erase(__top));
				return __top;
			}

			void _M_erase(_Link_type __x) {
				while (__x != 0) {
					_M_erase(_S_right(__x));
					_Link_type __y = _S_left(__x);
					destroy_node(__x);
					__x = __y;
				}
			}

			public:
				// allocation/deallocation
				_Rb_tree()
					: _Base(allocator_type()), _M_node_count(0), _M_key_compare()
				{
					_M_empty_initialize();
				}

				_Rb_tree(const _Compare& __comp)
					: _Base(allocator_type()), _M_node_count(0), _M_key_compare(__comp)
				{
					_M_empty_initialize();
				}

				_Rb_tree(const _Compare& __comp, const allocator_type& __a)
					: _Base(__a), _M_node_count(0), _M_key_compare(__comp)
				{
					_M_empty_initialize();
				}

				_Rb_tree(const _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x)
					: _Base(__x.get_allocator()),
					_M_node_count(0), _M_key_compare(__x._M_key_compare)
				{
					if (__x._M_root() == 0)
						_M_empty_initialize();
					else {
						_S_color(_M_header) = _S_rb_tree_red;
						_M_root() = _M_copy(__x._M_root(), _M_header);
						_M_leftmost() = _S_minimum(_M_root());
						_M_rightmost() = _S_maximum(_M_root());
					}
					_M_node_count = __x._M_node_count;
				}
				~_Rb_tree() { clear(); }
				_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>&
					operator=(const _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x);

			private:
				void _M_empty_initialize() {
					_S_color(_M_header) = _S_rb_tree_read;//为了区分__root节点
					_M_root() = 0;
					_M_leftmost() = _M_header;
					_M_rightmost() = _M_header;
				}

				public:
					// accessors:
					_Compare key_comp() const { return _M_key_compare; }
					iterator begin() { return _M_leftmost(); }
					const_iterator begin() const { return _M_leftmost(); }
					iterator end() { return _M_header; }
					const_iterator end() const { return _M_header; }
					reverse_iterator rbegin() { return reverse_iterator(end()); }
					const_reverse_iterator rbegin() const {
						return const_reverse_iterator(end());
					}
					reverse_iterator rend() { return reverse_iterator(begin()); }
					const_reverse_iterator rend() const {
						return const_reverse_iterator(begin());
					}
					bool empty() const { return _M_node_count == 0; }
					size_type size() const { return _M_node_count; }
					size_type max_size() const { return size_type(-1); }

					void swap(_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __t) {
						__STD::swap(_M_header, __t._M_header);
						__STD::swap(_M_node_count, __t._M_node_count);
						__STD::swap(_M_key_compare, __t._M_key_compare);
					}

					public:
						// insert/erase
						//红黑树利用insert_unique，insert_equal插入节点，首先它先遍历找到合适的插入位置的节点（不一定是叶子节点，作为它的父节点），之后再
						//利用_M_insert完成插入过程
						//这个两个函数的差别在于，insert_unique不会插入重复的键，而insert_equal可能会
						pair<iterator, bool> insert_unique(const value_type& __x);
						iterator insert_equal(const value_type& __x);

						//至710行，后面的代码之后再补充

};


