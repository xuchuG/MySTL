#pragma once

//链表节点
template <class _Val>
struct _Hashtable_node {
	_Hashtable_node* _M_next; 
	_Val _M_val;
};

//hashtable的迭代器
template <class _Val,class _Key,class _HashFcn,class _Extractkey,
	class _EqualKey,class _Alloc>
struct _Hashtable_iterator {
	typedef hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
		_Hashtable;
	typedef _Hashtable_iterator<_Val, _Key, _HashFcn,
		_ExtractKey, _EqualKey, _Alloc>
		iterator;
	typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn,
		_ExtractKey, _EqualKey, _Alloc>
		const_iterator;
	typedef _Hashtable_node<_Val> _Node;

	typedef forward_iterator_tag iterator_category;
	typedef _Val value_type;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;
	typedef _Val& reference;
	typedef _Val* pointer;

	_Node* _M_cur;
	_Hashtable* _M_ht;

	_Hashtable_iterator(_Node* __n,_Hashtable* __tab):_M_cur(__n),
		_M_ht(__tab){}
	_Hashtable_iterator(){}
	reference operator*()const {
		return _M_cur->_M_val;
	}

#ifndef __SGI_STL_NO_ARROW_OPERATOR
	pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

	//移动迭代器到桶（链表）的下一个节点，若已经到头了，那么就会指向相邻桶（右边第一个有值）的第一个节点
	iterator& operator++() {
		const _Node* __old = _M_cur;
		_M_cur = _M_cur->_M_next;
		if (!_M_cur) {
			size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
			while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size())
				_M_cur = _M_ht->_M_buckets[__bucket];
		}
		return *this;
	}

	iterator& operator++(int) {
		iterator __tmp = *this;
		++*this;
		return __tmp;
	}

	bool operator==(const const_iterator& __it)const {
		return _M_cur == __it._M_cur;
	}
	bool operator!=(const const_iterator& __it)const {
		return _M_cur != __it._M_cur;
	}

	template <class _Val,class _Key,class _HashFcn,class _Extractkey,
		class _EqualKey,class _Alloc>
	class hashtable {
		typedef _Key key_type;
		typedef _Val value_type;
		typedef _HashFcn hasher;
		typedef _EqualKey key_equal;

		typedef size_t            size_type;
		typedef ptrdiff_t         difference_type;
		typedef value_type*       pointer;
		typedef const value_type* const_pointer;
		typedef value_type&       reference;
		typedef const value_type& const_reference;

		hasher hash_funct()const {
			return _M_hash;
		}
		key_equal key_eq()const {
			return _M_equals;
		}
		
	private :
		typedef _Hashtable_node<_Val> _Node;

#ifdef __STL_USE_STD_ALLOCATORS
	public:
		typedef typename _Alloc_traits<_Val, _Alloc>::allocator_type allocator_type;
		allocator_type get_allocator()const {
			return _M_node_allocator;
		}

	private:
		typename _Alloc_traits<_Node, _Alloc>::allocator_type _M_node_allocator;
		_Node* _M_get_node() {
			return _M_node_allocator.allocator(1);
		}
		void _M_put_node(_Node* __p) {
			_M_node_allocator.deallocate(__p, 1);
		}

# define __HASH_ALLOC_INIT(__a) _M_node_allocator(__a), 
#else /* __STL_USE_STD_ALLOCATORS */
	public:
		typedef _Alloc alloctor_type;
		alloctor_type get_allocator()const {
			return alloctor_type();
		}
	private:
		typedef simple_alloc<_Node, _Alloc> _M_node_allocator_type;
		_Node* _M_get_node() {
			return _M_node_allocator_type::allocator(1);
		}
		void _M_put_node(_Node* __p) {
			_M_node_allocator_type::deallocator(__p, 1);
		}
# define __HASH_ALLOC_INIT(__a)

#endif // __STL_USE_STD_ALLOCATORS

	private:
		hasher _M_hash;
		key_equal _M_equals;
		_Extractkey _M_get_key;
		vector<_Node*, _Alloc> _M_buckets;
		size_type _M_num_elements;

	public:
		typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
			iterator;
		typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey,
			_Alloc>
			const_iterator;

		//为了让迭代器访问_M_buckets成员
		friend struct
			_Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>;
		friend struct
			_Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>;

	public:
		hashtable(size_type __n,
			const _HashFcn&    __hf,
			const _EqualKey&   __eql,
			const _ExtractKey& __ext,
			const allocator_type& __a = allocator_type())
			: __HASH_ALLOC_INIT(__a)
			_M_hash(__hf),
			_M_equals(__eql),
			_M_get_key(__ext),
			_M_buckets(__a),
			_M_num_elements(0)
		{
			_M_initialize_buckets(__n);
		}

		hashtable(size_type __n,
			const _HashFcn&    __hf,
			const _EqualKey&   __eql,
			const allocator_type& __a = allocator_type())
			: __HASH_ALLOC_INIT(__a)
			_M_hash(__hf),
			_M_equals(__eql),
			_M_get_key(_ExtractKey()),
			_M_buckets(__a),
			_M_num_elements(0)
		{
			_M_initialize_buckets(__n);
		}

		private:
			//将vector里元素的值(链表的第一个节点放在位置)全部初始化为NULL
			void _M_initialize_buckets(size_type __n) {
				const size_type __n_buckets = _M_next_size(__n);
				_M_buckets.reserve(__n_buckets);
				_M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node*)0);
			}
			size_type _M_next_size(size_type __n)const {
				return __stl_next_prime(__n);
			}

		public:
			hashtable(const hashtable& __ht)
				: __HASH_ALLOC_INIT(__ht.get_allocator())
				_M_hash(__ht._M_hash),
				_M_equals(__ht._M_equals),
				_M_get_key(__ht._M_get_key),
				_M_buckets(__ht.get_allocator()),
				_M_num_elements(0)
			{
				_M_copy_from(__ht);
			}

		private:
			void _M_copy_from(const hashtable& __ht) {
				_M_buckets.clear();
				_M_buckets.reserve(__ht._M_buckets.size());
				_M_buckets.insert(_M_buckets.end(), __ht._M_buckets.size(), (_Node*)0);
				__STL_TRY{
					for (size_type __i = 0; __i < __ht._M_buckets.size(); ++__i) {
						_Node* __cur = __ht._M_buckets[i];
						if (__cur) {
							_Node* __copy = _M_new_node(__cur->_M_val);
							_M_buckets[i] = __copy;
							for (_Node* __next = __cur->next; __next; __cur = __next, __next = __cur->next) {
								__copy->next = _M_new_node(__next->_M_val);
								__copy = __copy->next;
							}
						}

					}
				_M_num_elements = __ht._M_num_element;
				}
				__STL_UNWIND(clear());
			}

			_Node* _M_new_node(const value_type& __obj) {
				_Node* __n = _M_get_node();
				__n->_M_next = 0;
				__STL_TRY{
					construct(&__n->_M_val,__obj);
					return __n;
				}
				__STL_UNWIND(_M_put_node(__n));
			}

#undef __HASH_ALLOC_INIT

			hashtable& operator= (const hashtable& __ht)
			{
				if (&__ht != this) {
					clear();
					_M_hash = __ht._M_hash;
					_M_equals = __ht._M_equals;
					_M_get_key = __ht._M_get_key;
					_M_copy_from(__ht);
				}
				return *this;
			}

			~hashtable() { clear(); }

			size_type size() const { return _M_num_elements; }
			size_type max_size() const { return size_type(-1); }
			bool empty() const { return size() == 0; }

			void swap(hashtable& __ht)
			{
				__STD::swap(_M_hash, __ht._M_hash);
				__STD::swap(_M_equals, __ht._M_equals);
				__STD::swap(_M_get_key, __ht._M_get_key);
				_M_buckets.swap(__ht._M_buckets);
				__STD::swap(_M_num_elements, __ht._M_num_elements);
			}

			//遍历vector，直至找到第一个节点
			iterator begin() {
				for (size_type __n = 0; __n < _M_buckets.size(); __n++) {
					if (_M_buckets[__n]) {
						return iterator(_M_buckets[__n], this);
					}
				}
			}
			//返回一个临时指针值为NULL的节点类似于slist
			iterator end() {
				return iterator(0, this);
			}

			const_iterator begin() const
			{
				for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
					if (_M_buckets[__n])
						return const_iterator(_M_buckets[__n], this);
				return end();
			}

			const_iterator end() const { return const_iterator(0, this); }

			public:

				size_type bucket_count() const { return _M_buckets.size(); }

				size_type max_bucket_count() const
				{
					return __stl_prime_list[(int)__stl_num_primes - 1];
				}

				//统计第__bucket中元素的个数
				size_type elems_in_bucket(size_type __bucket) const
				{
					size_type __result = 0;
					for (_Node* __cur = _M_buckets[__bucket]; __cur; __cur = __cur->_M_next)
						__result += 1;
					return __result;
				}
	};
};


