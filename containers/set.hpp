#ifndef SET_HPP
#define SET_HPP

#include <iostream>

#include <iterator>
#include <cstddef>
#include <memory>

#include "enable_if.hpp"
#include "reverse_iterator.hpp"
#include "iterator_traits.hpp"
#include "comparaison.hpp"

#include "less.hpp"
#include "pair.hpp"
#include "rbt_node.hpp"


namespace ft
{
	template < class T, class Compare = less<T>, class Alloc = std::allocator<T> >
	class set
	{
	public:
		typedef T key_type;
		typedef T value_type;
		typedef Compare key_compare;
		typedef Compare value_compare;
		typedef Alloc allocator_type;
		typedef typename allocator_type::reference reference;
		typedef typename allocator_type::const_reference const_reference;
		typedef typename allocator_type::pointer pointer;
		typedef typename allocator_type::const_pointer const_pointer;
		class const_iterator;
		typedef const_iterator iterator;
		typedef ft::reverse_iterator<iterator> reverse_iterator;
		typedef ft::reverse_iterator<const_iterator> const_reverse_iterator;

	protected:
		typedef rbt_node<value_type> rbt_node;
		typedef typename allocator_type::template rebind<rbt_node>::other
			node_allocator_type;

	public:
		typedef typename ft::iterator_traits<iterator>::difference_type difference_type;
		typedef std::size_t size_type;

		class const_iterator : std::bidirectional_iterator_tag
		{
		public:
			typedef typename ft::iterator_traits<const_pointer>::value_type value_type;
			typedef typename ft::iterator_traits<const_pointer>::difference_type difference_type;
			typedef const_pointer pointer;
			typedef typename ft::iterator_traits<const_pointer>::reference reference;
			typedef std::bidirectional_iterator_tag iterator_category;

			rbt_node* ptr_;

			const_iterator() :
				ptr_(NULL)
			{
				return;
			}

			const_iterator(rbt_node* ptr) :
				ptr_(ptr)
			{
				return;
			}

			const_iterator(const_iterator const& src) :
				ptr_(src.ptr_)
			{
				return;
			}

			virtual ~const_iterator()
			{
				return;
			}

			const_iterator& operator=(const_iterator const& rhs)
			{
				this->ptr_ = rhs.ptr_;
				return *this;
			}

			bool operator==(const_iterator const& rhs) const
			{
				return this->ptr_ == rhs.ptr_;
			}

			bool operator!=(const_iterator const& rhs) const
			{
				return this->ptr_ != rhs.ptr_;
			}

			reference operator*() const
			{
				return this->ptr_->val;
			}

			pointer operator->() const
			{
				return &this->ptr_->val;
			}

			const_iterator& operator++()
			{
				if (this->ptr_->right->right) {
					this->ptr_ = this->ptr_->right;
					if (this->ptr_->left->right != this->ptr_) {
						while (this->ptr_->left->left) {
							this->ptr_ = this->ptr_->left;
						}
					}
				}
				else {
					rbt_node* parent = this->ptr_->parent;
					while (parent->left != this->ptr_) {
						this->ptr_ = parent;
						parent = this->ptr_->parent;
					}
					this->ptr_ = parent;
				}
				return *this;
			}

			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
				++(*this);
				return tmp;
			}

			const_iterator& operator--()
			{
				if (this->ptr_->left->left) {
					this->ptr_ = this->ptr_->left;
					if (this->ptr_->right->left != this->ptr_) {
						while (this->ptr_->right->right) {
							this->ptr_ = this->ptr_->right;
						}
					}
				}
				else {
					rbt_node* parent = this->ptr_->parent;
					while (parent->right != this->ptr_) {
						this->ptr_ = parent;
						parent = this->ptr_->parent;
					}
					this->ptr_ = parent;
				}
				return *this;
			}

			const_iterator operator--(int)
			{
				const_iterator tmp = *this;
				--(*this);
				return tmp;
			}
		};
	
	protected:
		key_compare comp_;
		allocator_type alloc_;
		node_allocator_type node_alloc_;
		rbt_node* ghost_node_;
		rbt_node* nil_node_;
		rbt_node* root_node_;
		size_type size_;

	public:
		explicit set(key_compare const& comp = key_compare(),
				allocator_type const& alloc = allocator_type()) :
			comp_(comp),
			alloc_(alloc),
			node_alloc_(),
			ghost_node_(this->node_alloc_.allocate(1)),
			nil_node_(this->node_alloc_.allocate(1)),
			root_node_(this->nil_node_),
			size_(0)
		{
			this->ghost_node_->parent = this->ghost_node_;
			this->ghost_node_->left = this->ghost_node_;
			this->ghost_node_->right = this->ghost_node_;
			this->ghost_node_->is_red = false;
			this->nil_node_->is_red = false;
			this->nil_node_->parent = NULL;
			this->nil_node_->left = NULL;
			this->nil_node_->right = NULL;
			return;
		}

		template <class InputIterator>
		set(InputIterator first, InputIterator last,
				key_compare const& comp = key_compare(),
				allocator_type const& alloc = allocator_type()) :
			comp_(comp),
			alloc_(alloc),
			node_alloc_(),
			ghost_node_(this->node_alloc_.allocate(1)),
			nil_node_(this->node_alloc_.allocate(1)),
			root_node_(this->nil_node_),
			size_(0)
		{
			this->ghost_node_->parent = this->ghost_node_;
			this->ghost_node_->left = this->ghost_node_;
			this->ghost_node_->right = this->ghost_node_;
			this->ghost_node_->is_red = false;
			this->nil_node_->is_red = false;
			this->nil_node_->parent = NULL;
			this->nil_node_->left = NULL;
			this->nil_node_->right = NULL;
			this->insert(first, last);
			return;
		}

		set(set const& x) :
			comp_(x.comp_),
			alloc_(x.alloc_),
			node_alloc_(x.node_alloc_),
			ghost_node_(this->node_alloc_.allocate(1)),
			nil_node_(this->node_alloc_.allocate(1)),
			root_node_(this->nil_node_),
			size_(0)
		{
			this->ghost_node_->parent = this->ghost_node_;
			this->ghost_node_->left = this->ghost_node_;
			this->ghost_node_->right = this->ghost_node_;
			this->ghost_node_->is_red = false;
			this->nil_node_->is_red = false;
			this->nil_node_->parent = NULL;
			this->nil_node_->left = NULL;
			this->nil_node_->right = NULL;
			*this = x;
			return;
		}

		~set()
		{
			this->clear();
			this->node_alloc_.deallocate(this->ghost_node_, 1);
			this->node_alloc_.deallocate(this->nil_node_, 1);
			return;
		}

		set& operator=(set const& x)
		{
			this->clear();
			this->comp_ = x.comp_;
			for (const_iterator cit = x.begin(), cite = x.end();
					cit != cite; ++cit) {
				this->insert(*cit);
			}
			return *this;
		}

		iterator begin()
		{
			return this->begin_impl();
		}

		const_iterator begin() const
		{
			return const_iterator(this->begin_impl());
		}

		iterator end()
		{
			return this->end_impl();
		}

		const_iterator end() const
		{
			return const_iterator(this->end_impl());
		}

		reverse_iterator rbegin()
		{
			return reverse_iterator(this->ghost_node_);
		}

		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator(this->ghost_node_);
		}

		reverse_iterator rend()
		{
			return reverse_iterator(this->ghost_node_->right);
		}

		const_reverse_iterator rend() const
		{
			return const_reverse_iterator(this->ghost_node_->right);
		}

		bool empty() const
		{
			return this->size_ == 0;
		}

		size_type size() const
		{
			return this->size_;
		}

		size_type max_size() const
		{
			return this->node_alloc_.max_size();
		}

		pair<iterator,bool> insert(value_type const& val)
		{
			if (this->root_node_ == this->nil_node_) {
				return this->insert_to_root(val);
			}
			rbt_node* node = this->root_node_;
			while (1) {
				if (this->comp_(val, node->val) == true) {
					if (node->left == this->nil_node_
							|| node->left == this->ghost_node_) {
						return insert_node(node, &node->left, val);
					}
					node = node->left;
				}
				else if (this->comp_(node->val, val) == true) {
					if (node->right == this->nil_node_
							|| node->right == this->ghost_node_) {
						return insert_node(node, &node->right, val);
					}
					node = node->right;
				}
				else {
					return ft::pair<iterator,bool>(iterator(node), false);
				}
			}
		}

		iterator insert(iterator position, value_type const& val)
		{
			if (this->root_node_ == this->nil_node_) {
				return this->insert_to_root(val).first;
			}
			const_iterator previous = position;
			previous--;
			const_iterator next = position;
			next++;
			if ((previous.ptr_ == this->ghost_node_
						|| this->comp_(*previous, val) == true)
					&& (position.ptr_ == this->ghost_node_
						|| this->comp_(val, *position) == true)) {
				if (position.ptr_ == this->ghost_node_) {
					return this->insert_node(previous.ptr_,
							&previous.ptr_->right, val).first;
				}
				else if (position.ptr_->left == this->nil_node_
							|| position.ptr_->left == this->ghost_node_) {
					return this->insert_node(position.ptr_,
							&position.ptr_->left, val).first;
				}
				else {
					return this->insert(iterator(position.ptr_->left), val);
				}
			}
			else if ((position.ptr_ == this->ghost_node_
						|| this->comp_(*position, val) == true)
					&& (next.ptr_ == this->ghost_node_
						|| this->comp_(val, *next) == true)) {
				if (position.ptr_ == this->ghost_node_) {
					return this->insert_node(next.ptr_,
							&next.ptr_->left, val).first;
				}
				else if (position.ptr_->right == this->nil_node_
						|| position.ptr_->right == this->ghost_node_) {
					return this->insert_node(position.ptr_,
							&position.ptr_->right, val).first;
				}
				else {
					return this->insert(iterator(position.ptr_->right), val);
				}
			}
			else {
				return this->insert(val).first;
			}
		}

		template <class InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			while (first != last) {
				this->insert(*first);
				++first;
			}
			return;
		}

		void erase(iterator position)
		{
			rbt_node* node = position.ptr_;
			rbt_node* new_min = NULL;
			if (node->left == this->ghost_node_) {
				iterator new_min_position = position;
				++new_min_position;
				new_min = new_min_position.ptr_;
			}
			rbt_node* new_max = NULL;
			if (node->right == this->ghost_node_) {
				iterator new_max_position = position;
				--new_max_position;
				new_max = new_max_position.ptr_;
			}
			bool original_is_red = node->is_red;
			rbt_node* violating_node = NULL;
			if (node->left == this->nil_node_
					|| node->left == this->ghost_node_) {
				this->substitute_node(node, node->right);
				violating_node = node->right;
				violating_node->parent = node->parent;

			}
			else if (node->right == this->nil_node_
					|| node->right == this->ghost_node_) {
				this->substitute_node(node, node->left);
				violating_node = node->left;
				violating_node->parent = node->parent;
			}
			else {
				rbt_node* substitute = node->right;
				while (substitute->left != this->nil_node_
						&& substitute->left != this->ghost_node_) {
					substitute = substitute->left;
				}
				original_is_red = substitute->is_red;
				violating_node = substitute->right;
				if (substitute->parent == node) {
					violating_node->parent = substitute;
				}
				else {
					this->substitute_node(substitute, substitute->right);
					substitute->right = node->right;
					substitute->right->parent = substitute;
				}
				this->substitute_node(node, substitute);
				substitute->left = node->left;
				substitute->left->parent = substitute;
				substitute->is_red = node->is_red;
			}
			this->node_alloc_.deallocate(node, 1);
			if (original_is_red == false) {
				fix_deletion(violating_node);
			}
			if (new_min) {
				new_min->left = this->ghost_node_;
				this->ghost_node_->right = new_min;
			}
			if (new_max) {
				new_max->right = this->ghost_node_;
				this->ghost_node_->left = new_max;
			}
			this->size_--;
			return;
		}

		size_type erase(value_type const& val)
		{
			rbt_node* node = this->root_node_;
			while (node != this->nil_node_ && node != this->ghost_node_) {
				if (this->comp_(val, node->val) == true) {
					node = node->left;
				}
				else if (this->comp_(node->val, val) == true) {
					node = node->right;
				}
				else {
					this->erase(iterator(node));
					return 1;
				}
			}
			return 0;
		}

		void erase(iterator first, iterator last)
		{
			while (first != last) {
				iterator to_delete = first;
				first++;
				this->erase(to_delete);
			}
			return;
		}

		void swap(set& x)
		{
			key_compare key_compare_tmp = this->comp_;
			this->comp_ = x.comp_;
			x.comp_ = key_compare_tmp;
			allocator_type allocator_tmp = this->alloc_;
			this->alloc_ = x.alloc_;
			x.alloc_ = allocator_tmp;
			node_allocator_type node_allocator_tmp = this->node_alloc_;
			this->node_alloc_ = x.node_alloc_;
			x.node_alloc_ = node_allocator_tmp;
			rbt_node* node_tmp = this->ghost_node_;
			this->ghost_node_ = x.ghost_node_;
			x.ghost_node_ = node_tmp;
			node_tmp = this->nil_node_;
			this->nil_node_ = x.nil_node_;
			x.nil_node_ = node_tmp;
			node_tmp = this->root_node_;
			this->root_node_ = x.root_node_;
			x.root_node_ = node_tmp;
			size_type size_tmp = this->size_;
			this->size_ = x.size_;
			x.size_ = size_tmp;
		}

		void clear()
		{
			clear_rbt(this->root_node_);
			this->root_node_ = this->nil_node_;
			this->ghost_node_->parent = this->ghost_node_;
			this->ghost_node_->left = this->ghost_node_;
			this->ghost_node_->right = this->ghost_node_;
			this->size_ = 0;
		}

		key_compare key_comp() const
		{
			return this->comp_;
		}

		value_compare value_comp() const
		{
			return this->comp_;
		}

		iterator find(value_type const& val) const
		{
			return this->find_impl(val);
		}

		size_type count(value_type const& val) const
		{
			if (this->find(val) != this->end()) {
				return 1;
			}
			return 0;
		}

		iterator lower_bound(value_type const& val) const
		{
			return this->lower_bound_impl(val);
		}

		iterator upper_bound(value_type const& val) const
		{
			return this->upper_bound_impl(val);
		}

		pair<iterator,iterator> equal_range(value_type const& val) const
		{
			return this->equal_range_impl(val);
		}

		allocator_type get_allocator() const
		{
			return this->alloc_;
		}

	private:
		iterator begin_impl() const
		{
			return iterator(this->ghost_node_->right);
		}

		iterator end_impl() const
		{
			return iterator(this->ghost_node_);
		}

		rbt_node* create_rbt_node(value_type const& val)
		{
			rbt_node* node = this->node_alloc_.allocate(1);
			this->node_alloc_.construct(node, rbt_node());
			node->parent = NULL;
			node->left = this->nil_node_;
			node->right = this->nil_node_;
			this->alloc_.construct(&node->val, val);
			this->size_++;
			return node;
		}

		ft::pair<iterator, bool> insert_to_root(value_type const& val)
		{
				this->root_node_ = this->create_rbt_node(val);
				this->root_node_->is_red = false;
				this->root_node_->left = this->ghost_node_;
				this->root_node_->right = this->ghost_node_;
				this->ghost_node_->left = this->root_node_;
				this->ghost_node_->right = this->root_node_;
				return ft::pair<iterator,bool>(iterator(this->root_node_),
						true);
		}

		ft::pair<iterator, bool> insert_node(rbt_node* parent, rbt_node** node_ptr,
				value_type const& val)
		{
			rbt_node* new_node = this->create_rbt_node(val);
			if (parent == this->ghost_node_->right
					&& &parent->left == node_ptr) {
				this->ghost_node_->right = new_node;
				new_node->left = this->ghost_node_;
			}
			else if (parent == this->ghost_node_->left
					&& &parent->right == node_ptr) {
				this->ghost_node_->left = new_node;
				new_node->right = this->ghost_node_;
			}
			new_node->parent = parent;
			*node_ptr = new_node;
			fix_insertion(new_node);
			return ft::pair<iterator,bool>(iterator(new_node), true);
		}

		void left_rotate_node(rbt_node* node)
		{
			rbt_node* parent = node->parent;
			rbt_node* right = node->right;
			if (right->left != this->nil_node_) {
				right->left->parent = node;
			}
			if (parent) {
				if (parent->right == node) {
					parent->right = right;
				}
				else {
					parent->left = right;
				}
			}
			else {
				this->root_node_ = right;
			}
			node->parent = right;
			node->right = right->left;
			right->parent = parent;
			right->left = node;
		}

		void right_rotate_node(rbt_node* node)
		{
			rbt_node* parent = node->parent;
			rbt_node* left = node->left;
			if (left->right != this->nil_node_) {
				left->right->parent = node;
			}
			if (parent) {
				if (parent->right == node) {
					parent->right = left;
				}
				else {
					parent->left = left;
				}
			}
			else {
				this->root_node_ = left;
			}
			node->parent = left;
			node->left = left->right;
			left->parent = parent;
			left->right = node;
		}

		void fix_insertion(rbt_node* node)
		{
			rbt_node* parent = node->parent;
			while (parent && parent->is_red == true) {
				rbt_node* grand_parent = node->get_grand_parent();
				rbt_node* uncle = node->get_uncle();
				if (uncle != this->nil_node_ && uncle->is_red == true) {
					parent->is_red = false;
					grand_parent->is_red = true;
					uncle->is_red = false;
				}
				else {
					// triangle cases
					if (grand_parent->right == parent && parent->left == node) {
						right_rotate_node(parent);
						node = parent;
						parent = node->parent;
					}
					else if (grand_parent->left == parent
							&& parent->right == node) {
						left_rotate_node(parent);
						node = parent;
						parent = node->parent;
					}
					// line cases
					if (grand_parent->right == parent
							&& parent->right == node) {
						left_rotate_node(grand_parent);
						parent->is_red = false;
						grand_parent->is_red = true;
					}
					else if (grand_parent->left == parent
							&& parent->left == node) {
						right_rotate_node(grand_parent);
						parent->is_red = false;
						grand_parent->is_red = true;
					}
				}
				node = grand_parent;
				parent = node->parent;
			}
			this->root_node_->is_red = false;
			return;
		}

		rbt_node* get_deletion_substitute(rbt_node* node) const
		{
			if (node->left != this->nil_node_
					&& node->left != this->ghost_node_) {
				if (node->right != this->nil_node_
						&& node->right != this->ghost_node_) {
					iterator successor_it(node);
					++successor_it;
					return successor_it.ptr_;
				}
				else {
					return node->left;
				}
			}
			else {
				return node->right;
			}
		}

		void fix_deletion(rbt_node* node)
		{
			rbt_node* sibling = NULL;
			while (node != this->root_node_ && node->is_red == false) {
				if (node->parent->left == node) {
					sibling = node->parent->right;
					if (sibling->is_red == true)
					{
						sibling->is_red = false;
						node->parent->is_red = true;
						left_rotate_node(node->parent);
						sibling = node->parent->right;
					}
					if ((sibling->left == this->nil_node_
								|| sibling->left == this->ghost_node_
								|| sibling->left->is_red == false)
							&& (sibling->right == this->nil_node_
								|| sibling->right == this->ghost_node_
								|| sibling->right->is_red == false)) {
						sibling->is_red = true;
						node = node->parent;
					}
					else {
						if (sibling->right == this->nil_node_
								|| sibling->right == this->ghost_node_
								|| sibling->right->is_red == false) {
							if (sibling->left != this->nil_node_
									&& sibling->left != this->ghost_node_) {
								sibling->left->is_red = false;
							}
							sibling->is_red = true;
							right_rotate_node(sibling);
							sibling = node->parent->right;
						}
						sibling->is_red = node->parent->is_red;
						node->parent->is_red = false;
						if (sibling->right != this->nil_node_
								&& sibling->right != this->ghost_node_) {
							sibling->right->is_red = false;
						}
						left_rotate_node(node->parent);
						node = this->root_node_;
					}
				}
				else {
					sibling = node->parent->left;
					if (sibling->is_red == true)
					{
						sibling->is_red = false;
						node->parent->is_red = true;
						right_rotate_node(node->parent);
						sibling = node->parent->left;
					}
					if ((sibling->right == this->nil_node_
								|| sibling->right == this->ghost_node_
								|| sibling->right->is_red == false)
							&& (sibling->left == this->nil_node_
								|| sibling->left == this->ghost_node_
								|| sibling->left->is_red == false)) {
						sibling->is_red = true;
						node = node->parent;
					}
					else {
						if (sibling->left == this->nil_node_
								|| sibling->left == this->ghost_node_
								|| sibling->left->is_red == false) {
							if (sibling->right != this->nil_node_
									&& sibling->right != this->ghost_node_) {
								sibling->right->is_red = false;
							}
							sibling->is_red = true;
							left_rotate_node(sibling);
							sibling = node->parent->left;
						}
						sibling->is_red = node->parent->is_red;
						node->parent->is_red = false;
						if (sibling->left != this->nil_node_
								&& sibling->left != this->ghost_node_) {
							sibling->left->is_red = false;
						}
						right_rotate_node(node->parent);
						node = this->root_node_;
					}
				}
			}
			node->is_red = false;
			return;
		}

		void substitute_node(rbt_node* node, rbt_node* substitute)
		{
			if (substitute != this->nil_node_
					&& substitute != this->ghost_node_) {
				substitute->parent = node->parent;
			}
			if (node->parent) {
				if (node->parent->left == node) {
					node->parent->left = substitute;
				}
				else {
					node->parent->right = substitute;
				}
			}
			else {
				this->root_node_ = substitute;
			}
			return;
		}

		void clear_rbt(rbt_node* node)
		{
			if (node == this->nil_node_ || node == this->ghost_node_) {
				return;
			}
			clear_rbt(node->left);
			clear_rbt(node->right);
			this->node_alloc_.destroy(node);
			this->node_alloc_.deallocate(node, 1);
			return;
		}

		iterator find_impl(value_type const& val) const
		{
			rbt_node* node = this->root_node_;
			while (node != this->nil_node_ && node != this->ghost_node_) {
				if (this->comp_(val, node->val) == true) {
					node = node->left;
				}
				else if (this->comp_(node->val, val) == true) {
					node = node->right;
				}
				else {
					return iterator(node);
				}
			}
			return this->end_impl();
		}

		iterator lower_bound_impl(value_type const& val) const
		{
			rbt_node* node = this->root_node_;
			while (node != this->ghost_node_
					&& this->comp_(node->val, val) == true) {
				node = node->right;
			}
			if (node == this->ghost_node_) {
				return this->end_impl();
			}
			while (node->left != this->nil_node_
						&& node->left != this->ghost_node_
						&& this->comp_(
							node->left->get_relative_max()->val, val)
						== false) {
				node = node->left;
				while (node != this->nil_node_
						&& this->comp_(node->val, val) == true) {
					node = node->right;
				}
			}
			return iterator(node);
		}

		iterator upper_bound_impl(value_type const& val) const
		{
			rbt_node* node = this->root_node_;
			while (node != this->ghost_node_
					&& this->comp_(val, node->val) == false) {
				node = node->right;
			}
			if (node == this->ghost_node_) {
				return this->end_impl();
			}
			while (node->left != this->nil_node_
						&& node->left != this->ghost_node_
						&& this->comp_(val,
							node->left->get_relative_max()->val)
						== true) {
				node = node->left;
				while (node != this->nil_node_
						&& this->comp_(val, node->val) == false) {
					node = node->right;
				}
			}
			return iterator(node);
		}

		pair<iterator,iterator> equal_range_impl(value_type const& val) const
		{
			iterator it = this->find_impl(val);
			if (it != this->end_impl()) {
				iterator next = it;
				++next;
				return pair<iterator, iterator>(it, next);
			}
			it = this->upper_bound_impl(val);
			return pair<iterator, iterator>(it, it);
		}
	};

	template<class T, class Compare, class Alloc>
	bool operator==(set<T, Compare, Alloc> const& lhs,
			set<T, Compare, Alloc> const& rhs)
	{
		if (lhs.size() != rhs.size()) {
			return false;
		}
		return ft::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template<class T, class Compare, class Alloc>
	bool operator!=(set<T, Compare, Alloc> const& lhs,
			set<T, Compare, Alloc> const& rhs)
	{
		return !(lhs == rhs);
	}

	template<class T, class Compare, class Alloc>
	bool operator<(set<T, Compare, Alloc> const& lhs,
			set<T, Compare, Alloc> const& rhs)
	{
		return ft::lexicographical_compare(lhs.begin(), lhs.end(),
				rhs.begin(), rhs.end());
	}

	template<class T, class Compare, class Alloc>
	bool operator<=(set<T, Compare, Alloc> const& lhs,
			set<T, Compare, Alloc> const& rhs)
	{
		return !(rhs < lhs);
	}

	template<class T, class Compare, class Alloc>
	bool operator>(set<T, Compare, Alloc> const& lhs,
			set<T, Compare, Alloc> const& rhs)
	{
		return rhs < lhs;
	}

	template<class T, class Compare, class Alloc>
	bool operator>=(set<T, Compare, Alloc> const& lhs,
			set<T, Compare, Alloc> const& rhs)
	{
		return !(lhs < rhs);
	}

	template<class T, class Compare, class Alloc>
	void swap(set<T, Compare, Alloc>& lhs, set<T, Compare, Alloc>& rhs)
	{
		lhs.swap(rhs);
		return;
	}
}

#endif
