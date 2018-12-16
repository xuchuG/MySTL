#pragma once

//计算中控控制的一块连续空间放下元素的个数
inline size_t __deque_buf_size(size_t __size) {
	return __size < 512 ? size_t(512 / __size) : size_t(1);
}

//并没有像list一样，iterator有个继承体系
template <class _Tp,class _Ref,class _Ptr>
struct _Deque_iterator {
	typedef _Deque<_Tp, _Tp&, _Tp*> iterator;
	typedef _Deque<_Tp, const _Tp&, _Tp*> const_iterator;
	static size_t _S_buffer_size() {
		return __deque_buf_size(sizeof(_Tp));
	}

	typedef random_access_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef _Ptr pointer;
	typedef _Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef _Tp** _Map_pointer;

	typedef _Deque_iterator self;

	//用来描述迭代器的4个指针,不能用pointer,list只有一个指向当前节点的指针
	_Tp* _M_cur;
	_Tp* _M_first;
	_Tp* _M_last;
	_Map_pointer _M_node;

	_Deque_iterator(_Tp* __x,_Map_pointer __y):_M_cur(__x),_M_first(*__y),
		_M_last(*__y+_S_buffer_size()),_M_node(__y){}
	_Deque_iterator():_M_cur(0),_M_first(0),_M_last(0),_M_node(0){}
	_Deque_iterator(const iterator& __x):
		_M_cur(__x._M_cur),_M_first(__x._M_first),_M_last(__x._M_last),_M_node(__x._M_node){}

	reference operator*() const { return *_M_cur; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
	pointer operator->() const { return _M_cur; }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

	difference_type operator-(const _Self& __x)const {//没有检查两个迭代器的位置关系是否会有错，即__x所在的位置比this更后面
		return difference_type(_S_buffer_size())*(_M_node - __x._M_node - 1) + (_M_cur - _M_first) + (__x._M_last - __x._M_cur);
	}

	_Self& operator++() {
		++_M_cur;
		if (_M_cur == _M_last) {
			_M_set_node(_M_node + 1);
			_M_cur = _M_first;
		}
		return *this;
	}

	void _M_set_node(_Map_pointer __new_node) {
		_M_node = __new_node;
		_M_first = *_M_node;
		_M_last = *_M_first + difference_type(_S_buffer_size());
	}

	_Self& operator++(int) {
		_Self __tmp = *this;
		++*this;
		return __tmp;
	}
	_Self& operator--() {
		if (_M_cur == _M_first) {
			_M_set_node(_M_node - 1);
			_M_cur = _M_last;
		}
		--_M_cur;
		return *this;
	}
	_Self& operator--(int) {
		_Self __tmp = *this;
		--*this;
		return __tmp;
	}

	_Self& operator+=(difference_type __n) {
		difference_type __offset = __n + _M_cur - _M_first;
		if (__offset > 0 && __offset <= _S_buffer_size()) {
			_M_cur += __n;
		}
		else {
			difference_type __node_offset = __offset > 0 ? (__offset / difference_type(_S_buffer_size())) : -difference_type(-__offset - 1) / _S_buffer_size() - 1;
			_M_set_node(_M_node + __node_offset); 
			_M_cur = _M_first + (__offset + __node_offset * difference_type(_S_buffer_size()));
		}
		return *this;
	}

	_Self operator+(difference_type __n) {
		_Self __tmp = *this;
		__tmp += __n;
		return __tmp;
	}

	_Self& operator-=(difference_type __n) {
		return *this += -__n;
	}
	_Self operator-(difference_type __n) {
		Self __tmp = *this;
		return __tmp -= __n;
	}
	
	reference operator[](difference_type __n) {
		return *(*this + __n);
	}

	bool operator==(_Self& __x) {
		return _M_cur == __x._M_cur;
	}
	bool operator!=(_Self& __x) {
		return !(*this == __x);
	}
	bool operator<(_Self& __x) {
		return _M_node == __x._M_node ? (_M_cur < __x._M_cur) : (_M_node < __x._M_node);
	}
	bool operator>(_Self& __x) {
		return __x < *this;
	}
	bool operator<=(_Self& __x) {
		return !(*this > __x);
	}
	bool operator>=(_Self& __x) {
		return !(*this < __x);
	}

};

//加上deuque迭代器位于第二个操作数的情况
template <class _Tp,class _Ref,class _Ptr>
inline _Deque_iterator<_Tp,_Ref,_Ptr>
operator+(ptrdiff_t __n, const _Deque_iterator<_Tp, _Ref, _Ptr>& __x) {
	return __x + __n;
}

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class _Tp, class _Ref, class _Ptr>
inline random_access_iterator_tag
iterator_category(const _Deque_iterator<_Tp, _Ref, _Ptr>&)
{
	return random_access_iterator_tag();
}

template <class _Tp, class _Ref, class _Ptr>
inline _Tp* value_type(const _Deque_iterator<_Tp, _Ref, _Ptr>&) { return 0; }

template <class _Tp, class _Ref, class _Ptr>
inline ptrdiff_t* distance_type(const _Deque_iterator<_Tp, _Ref, _Ptr>&) {
	return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

#ifdef __STL_USE_STD_ALLOCATORS
template <class _Tp,class _Alloc,bool __is_static>
class _Deque_alloc_base {//用于分配空间的基类，
public:
	typedef typename _Alloc_trait<_Tp, _Alloc>::allocator_type alloctor_type;//用于分配一个节点，即n个元素组成的连续空间，传入模板参数为_Tp
	allocator_type get_allocator()const {
		return _M_node_allocator;
	}
	_Deque_alloc_base(const allocator_type& __a):_M_node_allocator(__a),_M_map_allocator(__a),//不是很明白_M_map_allocator(__a)，不同模板类型的变量之间的拷贝构造
	_M_map(0),_M_map_size(0){

	}

protected:
	typedef typename _Alloc_traits<_Tp*, _Alloc>::allocator_type _map_allocator_type;//用于分配中继器节点，里面的每个元素是指向n个元素连续空间的指针

	allocator_type _M_node_allocator;
	_map_allocator_type _M_map_allocator;

	_Tp* _M_allocate_node() {
		return _M_node_allocator.allocate(__deque_buf_size(sizeof(_Tp)));
	}

	void _M_deallocate_node(_Tp* __p) {
		_M_node_allocator.deallocate(__p, __deque_buf_size(sizeof(_Tp)));
	}
	_Tp** _M_allocate_map(size_t __n) {
		return _M_map_allocator.allocate(__n);
	}
	void _M_deallocate_map(_Tp** __p, size_t __n) {
		_M_map_allocator.deallocate(__p, __n);
	}

	_Tp** _M_map;
	size_t _M_map_size;
};

//分配空间基类的特化
template <class _Tp,class _Alloc>
class _Deque_alloc_base<_Tp, _Alloc, true>
{
public:
	typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;
	allocator_type get_allocator()const {
		return allocator_type();
	}
	_Deque_alloc_base(const allocator_type&):_M_map(0),_M_size(0){}

protected:
	typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type _Node_alloc_type;
	typedef typename _Alloc_traits<_Tp*, _Alloc>::allocator_type _Map_alloc_type;

	_Tp* _M_allocate_node() {
		return _Node_alloc_type::allocate(__deque_buf_size(sizeof(_Tp)));
	}
	void _M_deallocate_node(_Tp* __p) {
		_Node_alloc_type::deallocate(__p, __deque_buf_size(sizeof(_Tp));
	}
	_Tp** _M_allocate_map(size_t __n) {
		_Map_alloc_type::allocate(__n);
	}
	void _M_deallocate_map(_Tp** __p,size_t __n) {
		_Map_alloc_type::deallocate(__p, __n);
	}

	_Tp** _M_map;
	size_t _M_map_size;
};

//将中控其和其对应节点的空间分配出来
template<class _Tp,class _Alloc>
class _Deque_base :_Deque_alloc_base<_Tp, _Alloc, _Alloc_traits<_Tp, _Alloc>::_S_intanceless> {
public:
	typedef _Deque_alloc_base<_Tp, _Alloc, _Alloc_traits<_Tp, _Alloc>::_S_intanceless> _Base;
	typedef typename _Base::allocator_type allocator_type;
	typedef _Dque_iterator<_Tp, _Tp&, _Tp*> iterator;
	typedef _Deque_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;

	_Deque_base(const allocator_type& __a, size_t __num_element):_Base(__a),_M_start(),_M_finish() {//传入初始化元素量的数目
		_M_initialize_map(__num_elements);
	}
	_Deque_base(const allocator_type&):_M_map(0),_M_map_size(0),_M_start(),_M_finish(){}
	~Deque_base();

protected:
	void _M_initialize_map(size_t);
	void _M_create_nodes(_Tp** __nstart, _Tp** __nfinish);
	void _M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish);
	enum { _S_initial_map_size = 8 };

protected:
	iterator _M_start;
	iterator _M_finish;
};

#else/*__STL_USE_STD_ALLOCATORS*/
template <class _Tp, class _Alloc>
class _Deque_base {
public:
	typedef _Deque_iterator<_Tp, _Tp&, _Tp*>             iterator;
	typedef _Deque_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;

	typedef _Alloc allocator_type;
	allocator_type get_allocator() const { return allocator_type(); }

	_Deque_base(const allocator_type&, size_t __num_elements)
		: _M_map(0), _M_map_size(0), _M_start(), _M_finish() {
		_M_initialize_map(__num_elements);
	}
	_Deque_base(const allocator_type&)
		: _M_map(0), _M_map_size(0), _M_start(), _M_finish() {}
	~_Deque_base();

protected:
	void _M_initialize_map(size_t);
	void _M_create_nodes(_Tp** __nstart, _Tp** __nfinish);
	void _M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish);
	enum { _S_initial_map_size = 8 };

protected:
	_Tp * * _M_map;
	size_t _M_map_size;
	iterator _M_start;
	iterator _M_finish;

	typedef simple_alloc<_Tp, _Alloc>  _Node_alloc_type;
	typedef simple_alloc<_Tp*, _Alloc> _Map_alloc_type;

	_Tp* _M_allocate_node()
	{
		return _Node_alloc_type::allocate(__deque_buf_size(sizeof(_Tp)));
	}
	void _M_deallocate_node(_Tp* __p)
	{
		_Node_alloc_type::deallocate(__p, __deque_buf_size(sizeof(_Tp)));
	}
	_Tp** _M_allocate_map(size_t __n)
	{
		return _Map_alloc_type::allocate(__n);
	}
	void _M_deallocate_map(_Tp** __p, size_t __n)
	{
		_Map_alloc_type::deallocate(__p, __n);
	}
};

#endif/*__STL_USE_STD_ALLOCATORS*/
template <class _Tp, class _Alloc>
_Deque_base<_Tp, _Alloc>::~_Deque_base() {
	if (_M_map) {
		_M_destroy_nodes(_M_start._M_node, _M_finish._M_node + 1);
		_M_deallocate_map(_M_map, _M_map_size);
	}
}

template<class _Tp,class _Alloc>
void _Deque_base<_Tp, _Alloc>::_M_initialize_map(size_t __num_elements) {
	size_t __num_nodes = __num_elements / __deque_buf_size(sizeof(_Tp)) + 1;
	_M_map_size = max((size_t)_S_initial_map_size, __num_nodes + 2);
	_M_Map = _M_allocate_map(_M_map_size);
	//让前面预留的放map元素空间一样
	_Tp** __nstart = _M_map + (_M_map_size - __num_nodes) / 2;
	_Tp** __nfinish = __nstart + __num_nodes;
	__STL_TRY{
		_M_create_nodes(__nstart, __nfinish);
	}
		__STL_UNWIND((_M_deallocate_map(_M_map, _M_map_size),
			_M_map = 0, _M_map_size = 0));
	_M_start._M_set_node(__nstart);
	_M_finish._M_set_node(__nfinish-1);
	_M_start._M_cur = _M_start._M_first;
	_M_finish._M_cur = _M_finish._M_first + __num_elements % __deque_buf_size(sizeof(_Tp));
}

template<class _Tp,class _Alloc>
void _Deque_base<_Tp, _Alloc>::_M_create_node(_Tp** __nstart, _Tp** _nfinish) {
	_Tp** __cur;
	__STL_TRY{
		for (__cur = __nstart; __cur < __nfinish; __cur++) {
			__cur = _M_allocate_node();
		}
	}
	STL_UNWIND(_M_destory_nodes(__nstart, __cur));
}

template<class _Tp,class _Alloc>
void _Deque_base<_Tp, _Alloc>::_M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish) {
	for (_Tp** __n = __nstart; __n < __nfinish; ++__n)
		_M_deallocate_node(*__n);
}

template<class _Tp, class _Alloc = __STL_DEFAULT_ALLOCTOR(_Tp)>
class deque : protected _Deque_base<_Tp, _Alloc> {
	__STL_CLASS_REQUIRES(_Tp, _Assignable);

	typedef _Deque_base<_Tp, _Alloc> _Base;

public:
	typedef _Tp value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename _Base::allocator_type allocator_type;

	allocator_type get_allocator()const {
		return _Base::get_allocator();
	}

public:
	typedef typename _Base::iterator iterator;
	typedef typename _Base::const_iterator const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
	typedef reverse_iterator<const_iterator> const_reverse_iterator;
	typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
	typedef reverse_iterator<const_iterator, value_type, const_reference,
		difference_type>
		const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type, reference, difference_type>
		reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:
	typedef pointer* _Map_pointer;
	static size_t _S_buffer_size() {
		return __deque_buf_size(sizeof(_Tp));
	}

protected:
#ifdef __STL_USE_NAMESPACES
	using _Base::_M_initialize_map;
	using _Base::_M_create_nodes;
	using _Base::_M_destroy_nodes;
	using _Base::_M_allocate_node;
	using _Base::_M_deallocate_node;
	using _Base::_M_allocate_map;
	using _Base::_M_deallocate_map;

	using _Base::_M_map;
	using _Base::_M_map_size;
	using _Base::_M_start;
	using _Base::_M_finish;
#endif //__STL_USE_NAMESPACES

public:
	iterator begin() {
		return _M_start;
	}
	iterator end() {
		return _M_finish;
	}

	const_iterator begin()const {
		return _M_start;
	}
	const_iterator end()const {
		return _M_finish;
	}
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(_M_finish);
	}
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(_M_start);
	}

	reference operator[](size_type __n) {
		return _M_start[difference_type(__n)];
	}
	const_reference operator[](size_type __n)const {
		return _M_start[difference_type(__n)];
	}

	//由于后面部分和vector和list存在大量重复，所以先停止这部分的书写


};