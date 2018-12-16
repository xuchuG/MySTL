#pragma once

#ifdef __STL_USE_STD_ALLOCATORS
template <class _Tp,class _Allocator,bool _IsStatic>
class _Vector_alloc_base {
public:
	typedef typename _Alloc_traits(_Tp, _Allocator)::allocator_type allocator_type;
	allocator_type get_allocator() const { return _M_data_allocator; }
	_Vector_alloc_base(const allocator_type& __a) :_M_data_allocator(__a), _M_start(0), _M_finish(0),
		_M_end_of_storage(0) {

	}
protected:
	allocator_type _M_data_allocator;
	_Tp* _M_start;
	_Tp* _M_finish;
	_Tp* _M_end_of_storage;

	_Tp* _M_allocate(size_t __n) {
		_M_data_allocator.allocate(__n);
	}
	void _M_deallocate(_Tp* __p, size_t __n) {
		_M_data_allocator.deallocate(__p, __n);
	}
};

//没有_M_data_allocator变量的偏特化
template <class _Tp, class _Allocator>
class _Vector_alloc_base<_Tp, _Allocator, true> {
public:
	typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type
		allocator_type;
	allocator_type get_allocator() const { return allocator_type(); }

	_Vector_alloc_base(const allocator_type&)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0)
	{}

protected:
	_Tp * _M_start;
	_Tp* _M_finish;
	_Tp* _M_end_of_storage;

	typedef typename _Alloc_traits<_Tp, _Allocator>::_Alloc_type _Alloc_type;
	_Tp* _M_allocate(size_t __n)
	{
		return _Alloc_type::allocate(__n);
	}
	void _M_deallocate(_Tp* __p, size_t __n)
	{
		_Alloc_type::deallocate(__p, __n);
	}
};

//从_Vector_alloc_base继承
template <class _Tp,class _Alloc>
struct _Vector_base : public _Vector_alloc_base<_Tp, _Alloc, _Alloc_traits<_Tp, _Alloc>::_S_instanceless> {
	typedef _Vector_alloc_base <_Tp, _Alloc, _Alloc_traits<_Tp, Alloc>::_S_instanceless> _Base;
	typedef typename _Base::allocator_type allocator_type;

	_Vector_base(const alloctor_type& __a):_Base(__a){}
	_Vector_base(size_t __n, const allocator_type& __a) :_Base(__a) {
		_M_start = _M_allocator(__n);
		_M_finish = _M_start;
		_M_end_of_storage = _M_start + __n;
	}

	~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }
};

#else /* __STL_USE_STD_ALLOCATORS */
//将上面三个类统合到了一块
template <class _Tp, class _Alloc>
class _Vector_base {
public:
	typedef _Alloc allocator_type;
	allocator_type get_allocator() const { return allocator_type(); }

	_Vector_base(const _Alloc&)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0) {}
	_Vector_base(size_t __n, const _Alloc&)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0)
	{
		_M_start = _M_allocate(__n);
		_M_finish = _M_start;
		_M_end_of_storage = _M_start + __n;
	}

	~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }

protected:
	_Tp * _M_start;
	_Tp* _M_finish;
	_Tp* _M_end_of_storage;

	typedef simple_alloc<_Tp, _Alloc> _M_data_allocator;
	_Tp* _M_allocate(size_t __n)
	{
		return _M_data_allocator::allocate(__n);
	}
	void _M_deallocate(_Tp* __p, size_t __n)
	{
		_M_data_allocator::deallocate(__p, __n);
	}
};

#endif/* __STL_USE_STD_ALLOCATORS */

//这句的_STL_DEFAULT_ALLOCATOR(_Tp)不是很理解，为什么要在里面加_Tp
//解答，它是一个define
/*# ifndef __STL_DEFAULT_ALLOCATOR
#   ifdef __STL_USE_STD_ALLOCATORS
#     define __STL_DEFAULT_ALLOCATOR(T) allocator< T >
#   else
#     define __STL_DEFAULT_ALLOCATOR(T) alloc
#   endif
# endif
*/

template <class _Tp,class _Alloc = _STL_DEFAULT_ALLOCATOR(_Tp)>
class vector :protected _Vector_base<_Tp, _Alloc> {
	__STL_CLASS_REQUIRES(_Tp, _Assignable);

private:
	typedef _Vector_base<_Tp, _Alloc> _Base;

public:
	//嵌套类型定义
	typedef _Tp value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename _Base::allocator_type allocator_type;
	allocator_type get_allocator() const { return _Base::get_allocator(); }

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
	typedef reverse_iterator<const_iterator> const_reverse_iterator;
	typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
	typedef reverse_iterator<const_iterator, value_type, const_reference,
		difference_type>  const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type, reference, difference_type>
		reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:
#ifdef __STL_HAS_NAMESPACES
	using _Base::_M_allocate;
	using _Base::_M_deallocate;
	using _Base::_M_start;
	using _Base::_M_finish;
	using _Base::_M_end_of_storage;
#endif /* __STL_HAS_NAMESPACES */

protected:
	void _M_insert_aux(iterator __position, const _Tp& __x);
	void _M_insert_aux(iterator __position);

public:
	iterator begin() { return _M_start; }
	const_iterator begin() const { return _M_start; }
	iterator end() { return _M_finish; }
	const iterator end()const { return _M_finish; }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin()const {
		return const_reverse_iterator(end());
	}
	reverse_iterator rend() {
		return reverse_iterator(begin());
	}
	const_reverse_iterator rend()const {
		return const_reverse_iterator(begin());
	}

	size_type size()const {
		return size_type(end() - begin());
	}
	size_type max_size()const {
		return size_type(-1) / sizeof(_Tp);
	}
	size_type capacity()const {
		return size_type(_M_end_of_storage - begin());
	}
	bool empty()const {
		return begin() == end();
	}

	reference operator[](size_type __n) {
		return *(begin()+ __n);	
	}
	const_reference operator[](size_type __n)const {
		return *(begin() + __n);
	}

#ifdef __STL_THROW_RANGE_ERRORS
	void _M_range_check(size_type __n)const {
		if (__n >= this->size()) {
			__stl_throw_range_error("vector");
		}
	}

	reference at(size_type __n) {
		_M_range_check(__n);
		return (*this)[__n];
	}
	const_reference at(size_type __n)const {
		_M_range_check(__n);
		return (*this)[__n];
	}
#endif

	explicit vector(const allocator_type& __a = allocator_type()):_Base(__a) {

	}
	vector(size_type __n, const _Tp& __value,const allocator_type& __a = allocator_type()):_Base(__n,__a)//_Base申请空间
	{
		//构造
		_M_finish = uninitialized_fill_n(_M_start, __n, __value);
	}

	explicit vector(size_type __n):_Base(__n,allocator_type()){
		_M_finish = uninitialized_fill_n(_M_start, __n, _Tp());
	}
	vector(const vector<_Tp, _Alloc>& __x) :_Base(__x.size(),__x.get_allocator()) {
		_M_finish = uninitialized_copy(__x.begin(), __x.end(), _M_start);
	}

#ifdef __STL_MEMBER_TEMPLATES
	//第一层
	template <class _InputIterator>
	vector(_InputIterator __first, _InputIterator __last, const allocator_type& __a = allocator_type()) : _Base(__a) {
		typedef typename _Is_inteeger<_InputIterator>::_Itegral _Integral;
		_M_initialize_aux(__first, __last, _Integral());
	}

	//第二层
	template<class _Integer>
	void _M_initialize_aux(_Integer __n, _Integer __value, __true_type) {
		_M_start = _M_allocate(__n);
		_M_end_of_storage = _M_start + __n;
		_M_finish = uninitialized_fill_n(_M_start, __n, __value);
	}
	template<class _InputIterator>
	void _M_initialize_aux(_InputIterator __first, _InputIterator __last, __false_type) {
		_M_range_initialize(__first, __last, __ITERATOR_CATEGORY(__first));
	}

#else
	vector < const _Tp* __first, const _Tp* __last, const allocator_type& __a = allocator_type()):_Base(__last - __first, __a){
		_M_finish = uninitialized_copy(__first, __last, _M_start);
	}
#endif/* __STL_MEMBER_TEMPLATES */

#ifdef __STL_MEMBER_TEMPLATES
	//第三层
	template<class _InputIterator>
	void _M_range_initialize(_InputIterator __first, _InputIterator __last, input_iterator_tag) {
		for (; __first != __last; __first++) {
			push_back(*__first);
		}
	}
	template <class _ForwardgyIterator>
	void _M_range_initialize(_ForwardIterator __first, _ForwardIterator __last, forward_iterator_tag) {
		size_type __n = 0;
		distance(__first, __last, __n);
		_M_start = _M_allocate(__n);
		_M_end_of_storage = _M_start + __n;
		_M_finish = uninitialized_copy(__first,__last,_M_start);
	}

#endif/* __STL_MEMBER_TEMPLATES */

	~vector() {
		destory(_M_start, _M_finish);
	}

	vector<_Tp, _Alloc>& operator=(const vector<_Tp, _Alloc>& __x);

	void reserve(size_type __n) {
		if (capacity() < __n) {
			const size_type __old_size = size();
			iterator __tmp = _M_allocate_and_copy(__n, _M_start, _M_finish);
			//调用析构函数
			destroy(_M_start, _M_finish);
			_M_deallocate(_M_start, _M_end_of_storage - _M__start);
			_M_start = __tmp;
			_M_finish = __tmp + __old_size;
			_M_end_of_storage = _M_start + __n;
		}
	}

	//第一层
	void assign(size_type __n, const _Tp& __val) {
		_M_fill_assign(__n, __val);
	}
	//第二层,分了大小的三个情况
	void _M_fill_assign(size_type __n, const _Tp& __val) {
		if (__n > capacity()) {
			vector<_Tp, _Alloc> __tmp(__n, __val, get_allocator());
			__tmp.swap(*this);
		}
		else if (__n > size()) {
			//处理被构造过的空间，可能还需要要释放一些空间,存在问题，fill的操作仅仅是迭代器赋值，不知道这中间有没有对迭代器赋值操作的重载，有对迭代器原指向的对象的析构（应该有）
			fill(begin(), end(), __val);
			//处理没有被构造过的空间
			_M_finish = uninitialized_fill_n(end(), __n - size(), __val);

		}
		else {
			earase(fill_n(begin(), __n, __val), end());
		}
	}
#ifdef __STL_MEMBER_TEMPLATES

	template <class _InputIterator>
	void assign(_InputIterator __first, _InputIterator __last) {
		typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
		_M_assign_dispatch(__first, __last, _Integral());
	}

	template <class _Integer>
	void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
	{
		_M_fill_assign((size_type)__n, (_Tp)__val);
	}

	template <class _InputIter>
	void _M_assign_dispatch(_InputIter __first, _InputIter __last, __false_type)
	{
		_M_assign_aux(__first, __last, __ITERATOR_CATEGORY(__first));
	}

	template <class _InputIterator>
	void _M_assign_aux(_InputIterator __first, _InputIterator __last,
		input_iterator_tag);

	template <class _ForwardIterator>
	void _M_assign_aux(_ForwardIterator __first, _ForwardIterator __last,
		forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

	reference front() {
		return *begin();
	}
	const_reference front()const {
		return *begin();
	}
	reference back() {
		return *end();
	}
	const_reference back()const {
		return *end();
	}

	void push_back(const _Tp& __x) {
		if (_M_finish != _M_end_of_storage) {
			construct(_M_finish, __x);
			_M_finish++;
		}
		else {
			_M_insert_aux(end(),__x);
		}
	}
	void push_back() {
		if (_M_finish != _M_end_of_storage) {
			construct(_M_finish);
			_M_finish++;
		}
		else {
			_M_insert_aux(end());
		}
	}

	void swap(vector<_Tp, _Alloc>& __x) {
		__STD::swap(_M_start, __x._M_start);
		__STD::swap(_M_finish, __x._M_finish);
		__STD::swap(_M_end_of_storage, __x._M_end_of_storage);
	}

	//在position的位置插入__x
	iterator insert(iterator __position, const _Tp& __x) {
		size_type __n = __position - begin();
		if (_M_finish != _M_end_of_storage && __position == end()) {
			construct(_M_finish, __x);
			_M_finish++;
		}
		else {
			_M_insert_aux(__position, __x);
		}
		return begin() + __n;
	}
	void vector<_Tp, _Alloc>::_M_insert_aux(iterator __position, const _Tp& __x) {
		if (_M_finish != _M_end_of_storage) {
			//为什么要单独的将最后一个元素先构造，而不是将处理逻辑和后面的统一？
			construct(_M_finish, *(_M_finish - 1));
			++_M_finish;
			//单独声明x_copy的目的？
			_Tp __x_copy = __x;
			copy_backward(__position, _M_finish - 2, _M_finish - 1);
			*position = __x_copy;
		}
		else {
			const size_type __old_size = size();
			const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
			iterator __new_start = _M_allocate(__len);
			iterator __new_finish = __new_start;
			__STL_TRY{//类似于try
				__new_finish = uninitialized_copy(_M_start,__position,__new_start);
				consrtuct(_new_finish, __x);
				++__new_finish;
				__new_finish = uninitialized_copy(__position, _M_finsh, __new_finish);
			}__STL_UNWIND((destroy(__new_start, __new_finish),//类似于catch
				_M_deallocate(__new_start, __len)));
			destroy(begin(), end());
			_M_deallocate(_M_start, _M_end_of_storage - _M_start);
			_M_start = __new_start;
			_M_finish = __new_finish;
			_M_end_of_storage = __new_start + __len;
		}
	}

	iterator insert(iterator __position) {
		size_type __n = __position - begin();
		if (_M_finish != _M_end_of_storage && __position == end()) {
			construct(_M_finish);
			_M_finish++;
		}
		else {
			_M_insert_aux(__position);
		}
		return begin() + __n;
	}
	void vector<_Tp, _Alloc>::_M_insert_aux(iterator __position)
	{
		if (_M_finish != _M_end_of_storage) {
			construct(_M_finish, *(_M_finish - 1));
			++_M_finish;
			copy_backward(__position, _M_finish - 2, _M_finish - 1);
			*__position = _Tp();
		}
		else {
			const size_type __old_size = size();
			const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
			iterator __new_start = _M_allocate(__len);
			iterator __new_finish = __new_start;
			__STL_TRY{
				__new_finish = uninitialized_copy(_M_start, __position, __new_start);
			construct(__new_finish);
			++__new_finish;
			__new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
			}
				__STL_UNWIND((destroy(__new_start, __new_finish),
					_M_deallocate(__new_start, __len)));
			destroy(begin(), end());
			_M_deallocate(_M_start, _M_end_of_storage - _M_start);
			_M_start = __new_start;
			_M_finish = __new_finish;
			_M_end_of_storage = __new_start + __len;
		}
	}

#ifdef __STL_MEMBER_TEMPLATES
	// Check whether it's an integral type.  If so, it's not an iterator.
	template <class _InputIterator>
	void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
		typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
		_M_insert_dispatch(__pos, __first, __last, _Integral());
	}

	template <class _Integer>
	void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __val,
		__true_type)
	{
		_M_fill_insert(__pos, (size_type)__n, (_Tp)__val);
	}

	template <class _InputIterator>
	void _M_insert_dispatch(iterator __pos,
		_InputIterator __first, _InputIterator __last,
		__false_type) {
		_M_range_insert(__pos, __first, __last, __ITERATOR_CATEGORY(__first));
	}
#else /* __STL_MEMBER_TEMPLATES */
	void insert(iterator __position,
		const_iterator __first, const_iterator __last);
#endif /* __STL_MEMBER_TEMPLATES */

	void insert(iterator __pos, size_type __n, const _Tp& __x)
	{
		_M_fill_insert(__pos, __n, __x);
	}

	void _M_fill_insert(iterator __pos, size_type __n, const _Tp& __x)
	{
		if (__n != 0) {
			if (size_type(_M_end_of_storage - _M_finish) >= __n) {
				_Tp __x_copy = __x;
				const size_type __elems_after = _M_finish - __position;
				iterator __old_finish = _M_finish;
				if (__elems_after > __n) {//这个情况是由于构造和赋值操作存在差异
					uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
					_M_finish += __n;
					copy_backward(__position, __old_finish - __n, __old_finish);
					fill(__position, __position + __n, __x_copy);
				}
				else {
					uninitialized_fill_n(_M_finish, __n - __elems_after, __x_copy);
					_M_finish += __n - __elems_after;
					uninitialized_copy(__position, __old_finish, _M_finish);
					_M_finish += __elems_after;
					fill(__position, __old_finish, __x_copy);
				}
			}
			else {
				const size_type __old_size = size();
				const size_type __len = __old_size + max(__old_size, __n);
				iterator __new_start = _M_allocate(__len);
				iterator __new_finish = __new_start;
				__STL_TRY{
					__new_finish = uninitialized_copy(_M_start, __position, __new_start);
				__new_finish = uninitialized_fill_n(__new_finish, __n, __x);
				__new_finish
					= uninitialized_copy(__position, _M_finish, __new_finish);
				}
					__STL_UNWIND((destroy(__new_start, __new_finish),
						_M_deallocate(__new_start, __len)));
				destroy(_M_start, _M_finish);
				_M_deallocate(_M_start, _M_end_of_storage - _M_start);
				_M_start = __new_start;
				_M_finish = __new_finish;
				_M_end_of_storage = __new_start + __len;
			}
		}
	}

	void pop_back() {
		_M_finish--;
		destroy(_M_finish);
	}
	iterator erase(iterator __position) {
		if (__position + 1 != end()) {
			copy(__position + 1, _M_finish, __position);
		}
		--_M_finish;
		destroy(_M_finish);
		return __position;
	}
	iterator erase(iterator __first, iterator __last) {
		iterator __i = copy(__last, _M_finish, __first);
		destroy(__i, _M_finish);
		_M_finish = _M_finish - (__last - __first);
		return __first;
	}
	void resize(size_type __new_size, const _Tp& __x) {
		if (__new_size < size())
			erase(begin() + __new_size, end());
		else
			insert(end(), __new_size - size(), __x);
	}
	void resize(size_type __new_size) { resize(__new_size, _Tp()); }
	void clear() { erase(begin(), end()); }

	//写到stl_vector.h的434行(后面相当而言不那么重要)，待续......
};

