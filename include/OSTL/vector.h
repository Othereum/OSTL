#pragma once

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <memory_resource>
#include "internal/iterator.h"
#include "internal/compressed_pair.h"

namespace ostl
{
	template <class T, class Alloc = std::allocator<T>>
	class vector
	{
	public:
		using value_type = T;
		using allocator_type = Alloc;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using reference = T &;
		using const_reference = const T &;
		using pointer = typename std::allocator_traits<Alloc>::pointer;
		using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;
		using iterator = internal::iterator<value_type, difference_type, pointer, reference>;
		using const_iterator = internal::const_iterator<value_type, difference_type, pointer, reference>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		vector() = default;

		explicit vector(const Alloc& alloc) noexcept : r_{nullptr, alloc}
		{
		}

		vector(size_type n, const T& value, const Alloc& alloc = Alloc{})
			: r_{r_.second.allocate(n), alloc}, capacity_{n}, size_{n}
		{
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Alloc>::construct(r_.second, r_.first + i, value);
		}

		explicit vector(size_type n, const Alloc& alloc = Alloc{})
			: r_{r_.second.allocate(n), alloc}, capacity_{n}, size_{n}
		{
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Alloc>::construct(r_.second, r_.first + i);
		}

		template <class InputIt, class = std::enable_if_t<
			          std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>
			          || std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>>>
		vector(InputIt first, InputIt last, const Alloc& alloc = Alloc{})
			: r_{nullptr, alloc}
		{
			reserve(std::distance(first, last));
			for (InputIt it = first; it != last; ++it)
				emplace_back(*it);
		}

		vector(const vector& x)
			: r_{r_.second.allocate(x.size_), x.r_.second}, capacity_{x.size_}, size_{x.size_}
		{
			pointer p = r_.first;
			for (const value_type& a : x)
				std::allocator_traits<Alloc>::construct(r_.second, p++, a);
		}

		vector(const vector& x, const Alloc& alloc)
			: r_{r_.second.allocate(x.size_), alloc}, capacity_{x.size_}, size_{x.size_}
		{
			pointer p = r_.first;
			for (const value_type& a : x)
				std::allocator_traits<Alloc>::construct(r_.second, p++, a);
		}

		vector(vector&& x) noexcept
			: r_{x.r_.first, std::move(x.r_.second)}, capacity_{x.capacity_}, size_{x.size_}
		{
			x.r_.first = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
		}

		vector(vector&& x, const Alloc& alloc)
			: r_{x.r_.first, alloc}, capacity_{x.capacity_}, size_{x.size_}
		{
			x.r_.first = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
		}

		vector(std::initializer_list<T> init, const Alloc& alloc = Alloc{})
			: r_{nullptr, alloc}
		{
			const size_type s = init.size();
			reserve(s);
			size_ = s;
			pointer p = r_.first;
			for (auto it = init.begin(); it != init.end(); ++it)
				std::allocator_traits<Alloc>::construct(r_.second, p++, *it);
		}

		~vector() noexcept
		{
			clear();
			shrink_to_fit();
		}

		vector& operator=(const vector& x)
		{
			clear();
			r_.second = x.r_.second;
			reserve(x.size_);
			size_ = x.size_;
			pointer p = r_.first;
			for (const value_type& a : x)
				std::allocator_traits<Alloc>::construct(r_.second, p++, a);
			return *this;
		}

		vector& operator=(vector&& x)
		noexcept(std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value
			|| std::allocator_traits<Alloc>::is_always_equal::value)
		{
			clear();
			shrink_to_fit();
			r_.second = x.r_.second;
			r_.first = x.r_.first;
			capacity_ = x.capacity_;
			size_ = x.size_;
			x.r_.first = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
			return *this;
		}

		vector& operator=(std::initializer_list<T> init)
		{
			clear();
			const size_type s = init.size();
			inc_cap(s);
			size_ = s;
			pointer p = r_.first;
			for (auto it = init.begin(); it != init.end(); ++it)
				std::allocator_traits<Alloc>::construct(r_.second, p++, *it);
			return *this;
		}

		void assign(const size_type n, const T& t)
		{
			clear();
			inc_cap(n);
			size_ = n;
			for (size_type i = 0; i < n; ++i)
				std::allocator_traits<Alloc>::construct(r_.second, r_.first + i, t);
		}

		template <class InputIt, class = std::enable_if_t<
			          std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>
			          || std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>>>
		void assign(const InputIt first, const InputIt last)
		{
			clear();
			inc_cap(std::distance(first, last));
			for (InputIt it = first; it != last; ++it)
				emplace_back(*it);
		}

		void assign(std::initializer_list<T> init)
		{
			clear();
			const size_type s = init.size();
			inc_cap(s);
			size_ = s;
			pointer p = r_.first;
			for (auto it = init.begin(); it != init.end(); ++it)
				std::allocator_traits<Alloc>::construct(r_.second, p++, *it);
		}

		[[nodiscard]] allocator_type get_allocator() const noexcept { return r_.second; }

		[[nodiscard]] reference at(size_type n)
		{
			return const_cast<reference>(static_cast<const vector&>(*this).at(n));
		}

		[[nodiscard]] const_reference at(size_type n) const
		{
			if (n >= size_) throw std::out_of_range{""};
			return r_.first[n];
		}

		[[nodiscard]] reference operator[](size_type n)
		{
			return const_cast<reference>(static_cast<const vector&>(*this)[n]);
		}

		[[nodiscard]] const_reference operator[](size_type n) const { return r_.first[n]; }
		[[nodiscard]] reference front() { return const_cast<reference>(static_cast<const vector&>(*this).front()); }
		[[nodiscard]] const_reference front() const { return *r_.first; }
		[[nodiscard]] reference back() { return const_cast<reference>(static_cast<const vector&>(*this).back()); }
		[[nodiscard]] const_reference back() const { return r_.first[size_ - 1]; }

		[[nodiscard]] pointer data() noexcept { return const_cast<pointer>(static_cast<const vector&>(*this).data()); }
		[[nodiscard]] const_pointer data() const noexcept { return r_.first; }

		[[nodiscard]] iterator begin() noexcept { return iterator{r_.first}; }
		[[nodiscard]] const_iterator begin() const noexcept { return const_iterator{r_.first}; }
		[[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator{r_.first}; }

		[[nodiscard]] iterator end() noexcept { return iterator{r_.first + size_}; }
		[[nodiscard]] const_iterator end() const noexcept { return const_iterator{r_.first + size_}; }
		[[nodiscard]] const_iterator cend() const noexcept { return const_iterator{r_.first + size_}; }

		[[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
		[[nodiscard]] const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
		[[nodiscard]] const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{cend()}; }

		[[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
		[[nodiscard]] const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }
		[[nodiscard]] const_reverse_iterator crend() const noexcept { return const_reverse_iterator{cbegin()}; }

		[[nodiscard]] bool empty() const noexcept { return size_ == 0; }
		[[nodiscard]] size_type size() const noexcept { return size_; }
		[[nodiscard]] static size_type max_size() noexcept { return std::numeric_limits<difference_type>::max(); }

		void reserve(const size_type n)
		{
			if (n > max_size()) throw std::length_error{""};
			if (n <= capacity_) return;

			const pointer p = r_.second.allocate(n);
			if (r_.first)
			{
				for (size_type i = 0; i < size_; ++i)
				{
					std::allocator_traits<Alloc>::construct(r_.second, p + i, std::move(r_.first[i]));
					std::allocator_traits<Alloc>::destroy(r_.second, r_.first + i);
				}
				r_.second.deallocate(r_.first, capacity_);
			}
			r_.first = p;
			capacity_ = n;
		}

		[[nodiscard]] size_type capacity() const noexcept { return capacity_; }

		void shrink_to_fit()
		{
			if (size_ == capacity_) return;

			if (size_ == 0)
			{
				r_.second.deallocate(r_.first, capacity_);
				r_.first = nullptr;
				capacity_ = 0;
			}
			else
			{
				const pointer n = r_.second.allocate(size_);
				for (size_type i = 0; i < size_; ++i)
				{
					std::allocator_traits<Alloc>::construct(r_.second, n + i, std::move(r_.first[i]));
					std::allocator_traits<Alloc>::destroy(r_.second, r_.first + i);
				}
				r_.second.deallocate(r_.first, capacity_);
				r_.first = n;
				capacity_ = size_;
			}
		}

		void clear() noexcept
		{
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Alloc>::destroy(r_.second, r_.first + i);
			size_ = 0;
		}

		iterator insert(const_iterator position, const T& x) { return emplace(position, x); }
		iterator insert(const_iterator position, T&& x) { return emplace(position, std::move(x)); }

		iterator insert(const_iterator position, size_type n, const T& x)
		{
			const pointer it = shift(position - cbegin(), n);
			for (size_type i = 0; i < n; ++i)
				std::allocator_traits<Alloc>::construct(r_.second, it + i, x);
			size_ += n;
			return iterator{it};
		}

		template <class InputIt, class = std::enable_if_t<
			          std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>
			          || std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>>>
		iterator insert(const_iterator position, InputIt first, InputIt last)
		{
			const difference_type offset = position-- - cbegin();
			while (first != last) position = emplace(++position, *first++);
			return begin() + offset;
		}

		iterator insert(const_iterator position, std::initializer_list<T> list)
		{
			const pointer first = shift(position - cbegin(), list.size());
			pointer it = first;
			for (const T& x : list)
				std::allocator_traits<Alloc>::construct(r_.second, it++, x);
			size_ += list.size();
			return iterator{first};
		}

		template <class... Args>
		iterator emplace(const_iterator position, Args&& ... args)
		{
			const pointer new_elem = shift(position - begin(), 1);
			std::allocator_traits<Alloc>::construct(r_.second, new_elem, std::forward<Args>(args)...);
			++size_;
			return iterator{new_elem};
		}

		iterator erase(const_iterator position)
		{
			const pointer p = const_cast<pointer>(const_pointer{position});
			std::allocator_traits<Alloc>::destroy(r_.second, p);
			move(p + 1, p, size_ - (p - r_.first) - 1);
			--size_;
			return iterator{p};
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			const pointer f = const_cast<pointer>(const_pointer{first});
			const pointer l = const_cast<pointer>(const_pointer{last});

			for (pointer it = f; it != l; ++it)
				std::allocator_traits<Alloc>::destroy(r_.second, it);

			move(l, f, size_ - (l - r_.first));
			size_ -= l - f;

			return iterator{f};
		}

		void push_back(const T& x) { emplace_back(x); }
		void push_back(T&& x) { emplace_back(std::move(x)); }

		template <class... Args>
		void emplace_back(Args&& ... args) { emplace(cend(), std::forward<Args>(args)...); }

		void pop_back() { erase(cend() - 1); }

		void resize(size_type sz)
		{
			if (size_ < sz)
			{
				inc_cap(sz);
				pointer it = r_.first + sz;
				for (size_type i = sz - size_; i; --i)
					std::allocator_traits<Alloc>::construct(r_.second, it++);
				size_ = sz;
			}
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}

		void resize(size_type sz, const T& c)
		{
			if (size_ < sz)
				insert(cbegin() + size_, sz - size_, c);
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}

		void swap(vector& other)
		noexcept(std::allocator_traits<Alloc>::propagate_on_container_swap::value
			|| std::allocator_traits<Alloc>::is_always_equal::value)
		{
			vector t{std::move(other)};
			other = std::move(*this);
			*this = std::move(t);
		}

	private:
		internal::compressed_pair<pointer, allocator_type> r_;
		size_type capacity_ = 0;
		size_type size_ = 0;

		pointer shift(const size_type position, const size_type count)
		{
			if (capacity_ == 0)
			{
				capacity_ = new_cap(count);
				r_.first = r_.second.allocate(capacity_);
				return r_.first;
			}

			const size_type minReqCap = size_ + count;
			const size_type recommendedCap = new_cap(minReqCap);
			const pointer newMem = capacity_ < minReqCap ? r_.second.allocate(recommendedCap) : nullptr;

			move(r_.first + position, (newMem ? newMem : r_.first) + position + count, size_ - position);

			if (newMem)
			{
				move(r_.first, newMem, position);
				r_.second.deallocate(r_.first, capacity_);
				r_.first = newMem;
				capacity_ = recommendedCap;
			}

			return r_.first + position;
		}

		void move(pointer src, pointer dest, size_type count)
		{
			if (src == dest || count == 0) return;
			static constexpr auto inc = [](pointer& p) { ++p; };
			static constexpr auto dec = [](pointer& p) { --p; };
			void (*op)(pointer&) = inc;
			if (src < dest)
			{
				op = dec;
				src += count - 1;
				dest += count - 1;
			}
			while (count--)
			{
				std::allocator_traits<Alloc>::construct(r_.second, dest, std::move(*src));
				std::allocator_traits<Alloc>::destroy(r_.second, src);
				op(src);
				op(dest);
			}
		}

		void inc_cap(const size_type minReqCap)
		{
			if (capacity_ < minReqCap)
			{
				const size_type newCap = new_cap(minReqCap);
				const pointer newMem = r_.second.allocate(newCap);
				move(r_.first, newMem, size_);
				r_.second.deallocate(r_.first, capacity_);
				r_.first = newMem;
				capacity_ = newCap;
			}
		}

		[[nodiscard]] size_type new_cap(const size_type required) const
		{
			size_type new_cap = std::max<size_type>(1, capacity_);
			while (new_cap < required)
				new_cap = std::max<size_type>(2, new_cap * 3 / 2);
			return new_cap;
		}
	};

	template <class T, class Alloc>
	[[nodiscard]] typename vector<T, Alloc>::const_iterator operator+
	(typename vector<T, Alloc>::difference_type n, typename vector<T, Alloc>::const_iterator it)
	{
		return it + n;
	}

	template <class T, class Alloc>
	[[nodiscard]] typename vector<T, Alloc>::iterator operator+
	(typename vector<T, Alloc>::difference_type n, typename vector<T, Alloc>::iterator it)
	{
		return it + n;
	}

	namespace pmr
	{
		template <class T>
		using vector = ostl::vector<T, std::pmr::polymorphic_allocator<T>>;
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
	{
		if (lhs.size() != rhs.size()) return false;
		const auto end = lhs.end();
		for (auto l = lhs.begin(), r = rhs.begin(); l != end;)
			if (*l++ != *r++) return false;
		return true;
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
	{
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
	{
		return lhs < rhs || lhs == rhs;
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
	{
		return !(lhs <= rhs);
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
	{
		return !(lhs < rhs);
	}

	template <class T, class Alloc>
	void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

	template <class T, class Alloc, class U>
	void erase(vector<T, Alloc>& c, const U& value)
	{
		c.erase(std::remove(c.begin(), c.end(), value), c.end());
	}

	template <class T, class Alloc, class Pred>
	void erase(vector<T, Alloc>& c, Pred pred)
	{
		c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
	}

	// Deduction guide (C++17)
	template <class InputIt, class Alloc = std::allocator<typename std::iterator_traits<InputIt>::value_type>>
	vector(InputIt, InputIt, Alloc = Alloc{}) -> vector<typename std::iterator_traits<InputIt>::value_type, Alloc>;

	template <class Alloc>
	class vector<bool, Alloc>
	{
	public:
		using value_type = bool;
		using allocator_type = Alloc;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using const_reference = bool;

	private:
		using int_type = size_t;
		using num_bit_type = unsigned char;
		static constexpr num_bit_type n_bit = sizeof int_type * 8;

	public:
		class iterator;
		
		class reference
		{
		public:
			[[nodiscard]] operator bool() const noexcept { return ref_ >> bit_offset_ & 1; }

			~reference() = default;
			reference(const reference&) = delete;
			reference(reference&&) = delete;

			reference& operator=(reference&& x) noexcept { return *this = static_cast<bool>(x); }
			reference& operator=(const reference& x) noexcept { return *this = static_cast<bool>(x); }

			reference& operator=(const bool x) noexcept
			{
				const auto b = int_type(1) << bit_offset_;
				if (x) ref_ |= b;
				else ref_ &= ~b;
				return *this;
			}

			void flip() noexcept { *this = !*this; }

		private:
			friend vector;
			friend iterator;

			reference(int_type& ref, const num_bit_type bit_offset) noexcept
				:ref_{ref}, bit_offset_{bit_offset}
			{
			}

			int_type& ref_;
			const num_bit_type bit_offset_;
		};

		class const_iterator
		{
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = bool;
			using difference_type = difference_type;
			using pointer = bool*;
			using reference = bool;

			const_iterator() = default;

			const_iterator(int_type* first_elem_ptr, const size_type idx)
				: ptr_{first_elem_ptr + idx / n_bit}, bit_offset_{idx % n_bit}
			{
			}

			[[nodiscard]] bool operator*() const
			{
				return *ptr_ >> bit_offset_ & 1;
			}

			[[nodiscard]] bool operator[](const difference_type n) const
			{
				return *((*this)+n);
			}

			const_iterator& operator++() { return *this += 1; }

			const_iterator operator++(int)
			{
				const auto it = *this;
				++*this;
				return it;
			}

			const_iterator& operator--() { return *this -= 1; }

			const_iterator operator--(int)
			{
				const auto it = *this;
				--*this;
				return it;
			}

			const_iterator& operator+=(const difference_type n)
			{
				const auto d = bit_offset_ + n;
				ptr_ += (d < 0 ? d - (n_bit - 1) : d) / n_bit;
				bit_offset_ = d & n_bit - 1;
				return *this;
			}

			[[nodiscard]] const_iterator operator+(const difference_type n) const
			{
				const_iterator it = *this;
				it += n;
				return it;
			}

			const_iterator& operator-=(const difference_type n) { return *this += -n; }
			[[nodiscard]] const_iterator operator-(const difference_type n) const { return *this + -n; }

			[[nodiscard]] difference_type operator-(const const_iterator& rhs) const
			{
				return (ptr_ - rhs.ptr_) * n_bit + (bit_offset_ - rhs.bit_offset_);
			}

			[[nodiscard]] bool operator==(const const_iterator& rhs) const
			{
				return ptr_ == rhs.ptr_ && bit_offset_ == rhs.bit_offset_;
			}

			[[nodiscard]] bool operator!=(const const_iterator& rhs) const { return !(*this == rhs); }

			[[nodiscard]] bool operator<(const const_iterator& rhs) const
			{
				return ptr_ < rhs.ptr_ || ptr_ == rhs.ptr_ && bit_offset_ < rhs.bit_offset_;
			}

			[[nodiscard]] bool operator>(const const_iterator& rhs) const { return rhs < *this; }
			[[nodiscard]] bool operator>=(const const_iterator& rhs) const { return !(*this < rhs); }
			[[nodiscard]] bool operator<=(const const_iterator& rhs) const { return !(*this > rhs); }

		protected:
			int_type* ptr_ = nullptr;
			num_bit_type bit_offset_ = 0;
		};

		class iterator : public const_iterator
		{
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = bool;
			using difference_type = difference_type;
			using reference = typename vector::reference;
			using pointer = reference*;

			iterator() = default;

			iterator(int_type* first_elem_ptr, size_type idx) : const_iterator{first_elem_ptr, idx}
			{
			}

			[[nodiscard]] reference operator*() const { return {*this->ptr_, this->bit_offset_}; }

			[[nodiscard]] reference operator[](difference_type n) const
			{
				return reference(this->ptr_[n / n_bit], (this->bit_offset_ + n) % n_bit);
			}

			iterator& operator++() { return *this += 1; }

			iterator operator++(int)
			{
				iterator it = *this;
				++*this;
				return it;
			}

			iterator& operator--() { return *this -= 1; }

			iterator operator--(int)
			{
				iterator it = *this;
				--*this;
				return it;
			}

			iterator& operator+=(difference_type n)
			{
				const difference_type d = this->bit_offset_ + n;
				this->ptr_ += (d < 0 ? d - (n_bit - 1) : d) / n_bit;
				this->bit_offset_ = d & n_bit - 1;
				return *this;
			}

			[[nodiscard]] iterator operator+(difference_type n) const
			{
				iterator it = *this;
				it += n;
				return it;
			}

			iterator& operator-=(const difference_type n) { return *this += -n; }
			[[nodiscard]] iterator operator-(const difference_type n) const { return *this + -n; }

			[[nodiscard]] difference_type operator-(const iterator& rhs) const
			{
				return (this->ptr_ - rhs.ptr_) * n_bit + (this->bit_offset_ - rhs.bit_offset_);
			}

			[[nodiscard]] bool operator==(const iterator& rhs) const
			{
				return this->ptr_ == rhs.ptr_ && this->bit_offset_ == rhs.bit_offset_;
			}

			[[nodiscard]] bool operator!=(const iterator& rhs) const { return !(*this == rhs); }

			[[nodiscard]] bool operator<(const iterator& rhs) const
			{
				return this->ptr_ < rhs.ptr_ || this->ptr_ == rhs.ptr_ && this->bit_offset_ < rhs.bit_offset_;
			}

			[[nodiscard]] bool operator>(const iterator& rhs) const { return rhs < *this; }
			[[nodiscard]] bool operator>=(const iterator& rhs) const { return !(*this < rhs); }
			[[nodiscard]] bool operator<=(const iterator& rhs) const { return !(*this > rhs); }
		};

		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using pointer = iterator;
		using const_pointer = const_iterator;

		vector() = default;

		explicit vector(const Alloc& alloc) noexcept : vec_{alloc}
		{
		}

		vector(const size_type n, const bool value, const Alloc& alloc = Alloc{})
			: vec_((n + (n_bit - 1)) / n_bit, 0 - value, alloc), size_{n}
		{
		}

		explicit vector(const size_type n, const Alloc& alloc = Alloc{})
			: vec_{(n + (n_bit - 1)) / n_bit, alloc}, size_{n}
		{
		}

		template <class InputIt, class = std::enable_if_t<
			          std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>
			          || std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>>>
		vector(InputIt first, InputIt last, const Alloc& alloc = Alloc{})
			: size_{static_cast<size_type>(std::distance(first, last))}
		{
			const auto s = (size_ + (n_bit - 1)) / n_bit;
			vec_.reserve(s);
			for (size_type i = 0; i < s; ++i)
			{
				int_type bits = 0;
				for (num_bit_type j = 0; j < n_bit && first != last; ++j)
					bits |= *first++ << j;
				vec_.push_back(bits);
			}
		}

		vector(const vector&) = default;

		vector(const vector& x, const Alloc& alloc) : vec_{x.vec_, alloc}, size_{x.size_}
		{
		}

		vector(vector&& x) noexcept : vec_{std::move(x.vec_)}, size_{x.size_} { x.size_ = 0; }
		vector(vector&& x, const Alloc& alloc) : vec_{std::move(x.vec_), alloc}, size_{x.size_} { x.size_ = 0; }

		vector(const std::initializer_list<bool> init, const Alloc& alloc = Alloc{})
			:vector{init.begin(), init.end(), alloc}
		{
		}

		~vector() = default;

		vector& operator=(const vector&) = default;

		vector& operator=(vector&& x)
		noexcept(std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value
			|| std::allocator_traits<Alloc>::is_always_equal::value)
		{
			vec_ = std::move(x.vec_);
			size_ = x.size_;
			x.size_ = 0;
			return *this;
		}

		vector& operator=(std::initializer_list<bool> init) { return *this = vector(init, vec_.get_allocator()); }

		void assign(const size_type n, bool int_type) { *this = vector(n, int_type, vec_.get_allocator()); }

		template <class InputIt, class = std::enable_if_t<
			          std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>
			          || std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>>>
		void assign(InputIt first, InputIt last) { *this = vector(first, last, vec_.get_allocator()); }

		void assign(std::initializer_list<bool> init) { *this = vector(init, vec_.get_allocator()); }

		[[nodiscard]] allocator_type get_allocator() const noexcept { return vec_.get_allocator(); }

		[[nodiscard]] reference at(size_type n)
		{
			if (n >= size_) throw std::out_of_range{""};
			return (*this)[n];
		}

		[[nodiscard]] const_reference at(size_type n) const
		{
			if (n >= size_) throw std::out_of_range{""};
			return (*this)[n];
		}

		[[nodiscard]] reference operator[](size_type n) { return begin()[n]; }
		[[nodiscard]] const_reference operator[](size_type n) const { return begin()[n]; }
		[[nodiscard]] reference front() { return *begin(); }
		[[nodiscard]] const_reference front() const { return *begin(); }
		[[nodiscard]] reference back() { return end()[-1]; }
		[[nodiscard]] const_reference back() const { return end()[-1]; }

		[[nodiscard]] iterator begin() noexcept { return {vec_.data(), 0}; }
		[[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }
		[[nodiscard]] const_iterator cbegin() const noexcept { return {const_cast<int_type*>(vec_.data()), 0}; }

		[[nodiscard]] iterator end() noexcept { return {vec_.data(), size_}; }
		[[nodiscard]] const_iterator end() const noexcept { return cend(); }
		[[nodiscard]] const_iterator cend() const noexcept { return {const_cast<int_type*>(vec_.data()), size_}; }

		[[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
		[[nodiscard]] const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
		[[nodiscard]] const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{cend()}; }

		[[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
		[[nodiscard]] const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }
		[[nodiscard]] const_reverse_iterator crend() const noexcept { return const_reverse_iterator{cbegin()}; }

		[[nodiscard]] bool empty() const noexcept { return size_ == 0; }
		[[nodiscard]] size_type size() const noexcept { return size_; }

		[[nodiscard]] constexpr size_type max_size() const noexcept
		{
			return std::numeric_limits<difference_type>::max();
		}

		void reserve(const size_type n) { vec_.reserve((n + (n_bit - 1)) / n_bit); }
		[[nodiscard]] size_type capacity() const noexcept { return vec_.capacity() * n_bit; }

		void clear() noexcept
		{
			vec_.clear();
			size_ = 0;
		}

		iterator insert(const const_iterator position, const bool x) { return insert(position, 1, x); }

		iterator insert(const_iterator position, size_type n, bool x)
		{
			const difference_type d = position - begin();
			move(position, position + n, size_ - d);
			
			size_ += n;
			
			const auto ret = begin() + d;
			for (auto it = ret; n--;) *it++ = x;
			return ret;
		}

		template <class InputIt, class = std::enable_if_t<
			          std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>
			          || std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::
			                            iterator_category>>>
		iterator insert(const_iterator position, InputIt first, InputIt last)
		{
			const difference_type d = position - begin();
			difference_type n = std::distance(first, last);
			move(position, position + n, size_ - d);
			
			size_ += n;
			
			const auto ret = begin() + d;
			for (auto it = ret; n--;) *it++ = *first++;
			return ret;
		}

		iterator insert(const_iterator position, std::initializer_list<bool> list)
		{
			return insert(position, list.begin(), list.end());
		}

		template <class... Args>
		iterator emplace(const_iterator position, Args&& ... args)
		{
			return insert(position, bool{std::forward<Args>(args)...});
		}

		iterator erase(const_iterator position)
		{
			return erase(position, position + 1);
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			move(last, first, size_ - (last - begin()));
			size_ -= last - first;
			return begin() + (first - begin());
		}

		void push_back(bool x) { insert(cend(), x); }

		template <class... Args>
		void emplace_back(Args&& ... args) { push_back(bool{std::forward<Args>(args)...}); }

		void pop_back() { erase(cend() - 1); }

		void resize(size_type sz)
		{
			if (size_ < sz)
			{
				inc_cap(sz);
				iterator it = begin() + sz;
				for (size_type i = sz - size_; i; --i)
					*it++ = false;
				size_ = sz;
			}
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}

		void resize(size_type sz, bool c)
		{
			if (size_ < sz)
				insert(cbegin() + size_, sz - size_, c);
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}

		void swap(vector& other)
		noexcept(std::allocator_traits<Alloc>::propagate_on_container_swap::value
			|| std::allocator_traits<Alloc>::is_always_equal::value)
		{
			vector int_type{std::move(other)};
			other = std::move(*this);
			*this = std::move(int_type);
		}

	private:
		void move(const_iterator src, const_iterator dest, size_type cnt)
		{
			const difference_type d = dest - src;
			if (d > 0) inc_cap(size_ + d);
				
			if (d == 0 || cnt == 0) return;
			
			if (d > 0)
			{
				for (size_type i = src - begin() + cnt - 1; cnt--; --i)
					(*this)[i + d] = (*this)[i];
			}
			else
			{
				for (size_type i = src - begin(); cnt--; ++i)
					(*this)[i + d] = (*this)[i];
			}
		}

		void inc_cap(size_type min)
		{
			if (min > vec_.size() * n_bit)
				vec_.insert(vec_.end(), (min - vec_.size() * n_bit + n_bit - 1) / n_bit, 0);
		}

		vector<int_type, typename std::allocator_traits<Alloc>::template rebind_alloc<int_type>> vec_;
		size_type size_ = 0;
	};
}
