#include <initializer_list>

namespace ostl {
	template <class T, class Alloc = std::allocator<T>>
	class vector {
	public:
		using value_type = T;
		using allocator_type = Alloc;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using reference = T &;
		using const_reference = const T &;
		using pointer = typename std::allocator_traits<Alloc>::pointer;
		using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;

		class const_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = value_type;
			using difference_type = difference_type;
			using pointer = const_pointer;
			using reference = const_reference;

			const_iterator() = default;
			explicit const_iterator(pointer Data) :_Ptr{ Data } {}
			[[nodiscard]] explicit operator pointer() const { return _Ptr; }

			[[nodiscard]] reference operator*() const { return *_Ptr; }
			[[nodiscard]] reference operator[](difference_type n) const { return *(_Ptr + n); }
			[[nodiscard]] pointer operator->() const { return _Ptr; }
			const_iterator& operator++() { ++_Ptr; return *this; }
			const_iterator operator++(int) { const_iterator it = *this; ++_Ptr; return it; }
			const_iterator& operator--() { --_Ptr; return *this; }
			const_iterator operator--(int) { const_iterator it = *this; --_Ptr; return it; }
			const_iterator& operator+=(difference_type n) { _Ptr += n; return *this; }
			[[nodiscard]] const_iterator operator+(difference_type n) const { return const_iterator{ _Ptr + n }; }
			const_iterator& operator-=(difference_type n) { _Ptr -= n; return *this; }
			[[nodiscard]] const_iterator operator-(difference_type n) const { return const_iterator{ _Ptr - n }; }
			[[nodiscard]] difference_type operator-(const const_iterator& rhs) const { return _Ptr - rhs._Ptr; }
			[[nodiscard]] bool operator==(const const_iterator& rhs) const { return _Ptr == rhs._Ptr; }
			[[nodiscard]] bool operator!=(const const_iterator& rhs) const { return _Ptr != rhs._Ptr; }
			[[nodiscard]] bool operator<(const const_iterator& rhs) const { return _Ptr < rhs._Ptr; }
			[[nodiscard]] bool operator>(const const_iterator& rhs) const { return _Ptr > rhs._Ptr; }
			[[nodiscard]] bool operator>=(const const_iterator& rhs) const { return _Ptr >= rhs._Ptr; }
			[[nodiscard]] bool operator<=(const const_iterator& rhs) const { return _Ptr <= rhs._Ptr; }

		protected:
			pointer _Ptr = nullptr;
		};

		class iterator : public const_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = value_type;
			using difference_type = difference_type;
			using pointer = pointer;
			using reference = reference;

			iterator() = default;
			explicit iterator(pointer Data) :const_iterator{ Data } {}
			[[nodiscard]] explicit operator pointer() const { return _Ptr; }

			[[nodiscard]] reference operator*() const { return *_Ptr; }
			[[nodiscard]] reference operator[](difference_type n) const { return *(_Ptr + n); }
			[[nodiscard]] pointer operator->() const { return _Ptr; }
			iterator& operator++() { ++_Ptr; return *this; }
			iterator operator++(int) { iterator it = *this; ++_Ptr; return it; }
			iterator& operator--() { --_Ptr; return *this }
			iterator operator--(int) { iterator it = *this; --_Ptr; return it; }
			iterator& operator+=(difference_type n) { _Ptr += n; return *this; }
			[[nodiscard]] iterator operator+(difference_type n) const { return iterator{ _Ptr + n }; }
			iterator& operator-=(difference_type n) { _Ptr -= n; return *this; }
			[[nodiscard]] iterator operator-(difference_type n) const { return iterator{ _Ptr - n }; }
			[[nodiscard]] difference_type operator-(const iterator& rhs) const { return _Ptr - rhs._Ptr; }
			[[nodiscard]] bool operator==(const iterator& rhs) const { return _Ptr == rhs._Ptr; }
			[[nodiscard]] bool operator!=(const iterator& rhs) const { return _Ptr != rhs._Ptr; }
			[[nodiscard]] bool operator<(const iterator& rhs) const { return _Ptr < rhs._Ptr; }
			[[nodiscard]] bool operator>(const iterator& rhs) const { return _Ptr > rhs._Ptr; }
			[[nodiscard]] bool operator>=(const iterator& rhs) const { return _Ptr >= rhs._Ptr; }
			[[nodiscard]] bool operator<=(const iterator& rhs) const { return _Ptr <= rhs._Ptr; }
		};

		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		vector() noexcept(noexcept(Alloc{})) {}
		explicit vector(const Alloc& alloc) noexcept :alloc_{ alloc } {}

		vector(size_type n, const T& value, const Alloc& alloc = Alloc{})
			:alloc_{ alloc }, firstElemPtr_{ alloc_.allocate(n) }, capacity_{ n }, size_{ n }
		{
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Alloc>::construct(alloc_, firstElemPtr_ + i, value);
		}

		explicit vector(size_type n, const Alloc& alloc = Alloc{})
			:alloc_{ alloc }, firstElemPtr_{ alloc_.allocate(n) }, capacity_{ n }, size_{ n }
		{
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Alloc>::construct(alloc_, firstElemPtr_ + i);
		}

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
		vector(InputIt first, InputIt last, const Alloc& alloc = Alloc{})
			: alloc_{ alloc }
		{
			reserve(std::distance(first, last));
			for (InputIt it = first; it != last; ++it)
				emplace_back(*it);
		}

		vector(const vector& x)
			:alloc_{ x.alloc_ }, firstElemPtr_{ alloc_.allocate(x.size_) }, capacity_{ x.size_ }, size_{ x.size_ }
		{
			pointer p = firstElemPtr_;
			for (const value_type& a : x)
				std::allocator_traits<Alloc>::construct(alloc_, p++, a);
		}

		vector(const vector& x, const Alloc& alloc)
			:alloc_{ alloc }, firstElemPtr_{ alloc_.allocate(x.size_) }, capacity_{ x.size_ }, size_{ x.size_ }
		{
			pointer p = firstElemPtr_;
			for (const value_type& a : x)
				std::allocator_traits<Alloc>::construct(alloc_, p++, a);
		}

		vector(vector&& x) noexcept
			:alloc_{ x.alloc_ }, firstElemPtr_{ x.firstElemPtr_ }, capacity_{ x.capacity_ }, size_{ x.size_ }
		{
			x.firstElemPtr_ = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
		}

		vector(vector&& x, const Alloc& alloc)
			:alloc_{ alloc }, firstElemPtr_{ x.firstElemPtr_ }, capacity_{ x.capacity_ }, size_{ x.size_ }
		{
			x.firstElemPtr_ = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
		}

		vector(std::initializer_list<T> init, const Alloc& alloc = Alloc{})
			:alloc_{ alloc }
		{
			const size_type s = init.size();
			reserve(s);
			size_ = s;
			pointer p = firstElemPtr_;
			for (auto it = init.begin(); it != init.end(); ++it)
				std::allocator_traits<Alloc>::construct(alloc_, p++, *it);
		}

		~vector() noexcept {
			clear();
			shrink_to_fit();
		}

		vector& operator=(const vector& x) {
			clear();
			alloc_ = x.alloc_;
			reserve(x.size_);
			size_ = x.size_;
			pointer p = firstElemPtr_;
			for (const value_type& a : x)
				std::allocator_traits<Alloc>::construct(alloc_, p++, a);
			return *this;
		}
		
		vector& operator=(vector&& x)
			noexcept(std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value
				|| std::allocator_traits<Alloc>::is_always_equal::value)
		{
			clear();
			shrink_to_fit();
			alloc_ = x.alloc_;
			firstElemPtr_ = x.firstElemPtr_;
			capacity_ = x.capacity_;
			size_ = x.size_;
			x.firstElemPtr_ = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
			return *this;
		}
		
		vector& operator=(std::initializer_list<T> init) {
			clear();
			const size_type s = init.size();
			inc_cap(s);
			size_ = s;
			pointer p = firstElemPtr_;
			for (auto it = init.begin(); it != init.end(); ++it)
				std::allocator_traits<Alloc>::construct(alloc_, p++, *it);
			return *this;
		}

		void assign(const size_type n, const T& t) {
			clear();
			inc_cap(n);
			size_ = n;
			for (size_type i = 0; i < n; ++i)
				std::allocator_traits<Alloc>::construct(alloc_, firstElemPtr_ + i, t);
		}

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
		void assign(const InputIt first, const InputIt last) {
			clear();
			inc_cap(std::distance(first, last));
			for (InputIt it = first; it != last; ++it)
				emplace_back(*it);
		}

		void assign(std::initializer_list<T> init) {
			clear();
			const size_type s = init.size();
			inc_cap(s);
			size_ = s;
			pointer p = firstElemPtr_;
			for (auto it = init.begin(); it != init.end(); ++it)
				std::allocator_traits<Alloc>::construct(alloc_, p++, *it);
		}

		[[nodiscard]] allocator_type get_allocator() const noexcept { return alloc_; }

		[[nodiscard]] reference       at(size_type n) { return const_cast<reference>(static_cast<const vector&>(*this).at(n)); }
		[[nodiscard]] const_reference at(size_type n) const {
			if (n >= size_) throw std::out_of_range{ "" };
			return firstElemPtr_[n];
		}
		[[nodiscard]] reference       operator[](size_type n) { return const_cast<reference>(static_cast<const vector&>(*this)[n]); }
		[[nodiscard]] const_reference operator[](size_type n) const { return firstElemPtr_[n]; }
		[[nodiscard]] reference       front() { return const_cast<reference>(static_cast<const vector&>(*this).front()); }
		[[nodiscard]] const_reference front() const { return *firstElemPtr_; }
		[[nodiscard]] reference       back() { return const_cast<reference>(static_cast<const vector&>(*this).back()); }
		[[nodiscard]] const_reference back() const { return firstElemPtr_[size_ - 1]; }

		[[nodiscard]] pointer data() noexcept { return const_cast<pointer>(static_cast<const vector&>(*this).data()); }
		[[nodiscard]] const_pointer data() const noexcept { return firstElemPtr_; }

		[[nodiscard]] iterator                begin() noexcept { return iterator{ firstElemPtr_ }; }
		[[nodiscard]] const_iterator          begin() const noexcept { return const_iterator{ firstElemPtr_ }; }
		[[nodiscard]] const_iterator          cbegin() const noexcept { return const_iterator{ firstElemPtr_ }; }

		[[nodiscard]] iterator                end() noexcept { return iterator{ firstElemPtr_ + size_ }; }
		[[nodiscard]] const_iterator          end() const noexcept { return const_iterator{ firstElemPtr_ + size_ }; }
		[[nodiscard]] const_iterator          cend() const noexcept { return const_iterator{ firstElemPtr_ + size_ }; }

		[[nodiscard]] reverse_iterator        rbegin() noexcept { return reverse_iterator{ end() }; }
		[[nodiscard]] const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator{ end() }; }
		[[nodiscard]] const_reverse_iterator  crbegin() const noexcept { return const_reverse_iterator{ cend() }; }

		[[nodiscard]] reverse_iterator        rend() noexcept { return reverse_iterator{ begin() }; }
		[[nodiscard]] const_reverse_iterator  rend() const noexcept { return const_reverse_iterator{ begin() }; }
		[[nodiscard]] const_reverse_iterator  crend() const noexcept { return const_reverse_iterator{ cbegin() }; }

		[[nodiscard]] bool empty() const noexcept { return size_ == 0; }
		[[nodiscard]] size_type size() const noexcept { return size_; }
		[[nodiscard]] size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }

		void reserve(const size_type n) {
			if (n > max_size()) throw std::length_error{ "" };
			if (n <= capacity_) return;

			const pointer p = alloc_.allocate(n);
			if (firstElemPtr_) {
				for (size_type i = 0; i < size_; ++i) {
					std::allocator_traits<Alloc>::construct(alloc_, p + i, std::move(firstElemPtr_[i]));
					std::allocator_traits<Alloc>::destroy(alloc_, firstElemPtr_ + i);
				}
				alloc_.deallocate(firstElemPtr_, capacity_);
			}
			firstElemPtr_ = p;
			capacity_ = n;
		}

		[[nodiscard]] size_type capacity() const noexcept { return capacity_; }

		void shrink_to_fit() {
			if (size_ == capacity_) return;

			if (size_ == 0) {
				alloc_.deallocate(firstElemPtr_, capacity_);
				firstElemPtr_ = nullptr;
				capacity_ = 0;
			}
			else {
				const pointer n = alloc_.allocate(size_);
				for (size_type i = 0; i < size_; ++i) {
					std::allocator_traits<Alloc>::construct(alloc_, n + i, std::move(firstElemPtr_[i]));
					std::allocator_traits<Alloc>::destroy(alloc_, firstElemPtr_ + i);
				}
				alloc_.deallocate(firstElemPtr_, capacity_);
				firstElemPtr_ = n;
				capacity_ = size_;
			}
		}

		void clear() noexcept {
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Alloc>::destroy(alloc_, firstElemPtr_ + i);
			size_ = 0;
		}

		iterator insert(const_iterator position, const T& x) { return emplace(position, x); }
		iterator insert(const_iterator position, T&& x) { return emplace(position, std::move(x)); }
		iterator insert(const_iterator position, size_type n, const T& x) {
			const pointer it = shift(position - cbegin(), n);
			for (size_type i = 0; i < n; ++i)
				std::allocator_traits<Alloc>::construct(alloc_, it + i, x);
			size_ += n;
			return iterator{ it };
		}

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
		iterator insert(const_iterator position, InputIt first, InputIt last) {
			const difference_type offset = position-- - cbegin();
			while (first != last) position = emplace(++position, *first++);
			return begin() + offset;
		}

		iterator insert(const_iterator position, std::initializer_list<T> list) {
			const pointer first = shift(position - cbegin(), list.size());
			pointer it = first;
			for (const T& x : list)
				std::allocator_traits<Alloc>::construct(alloc_, it++, x);
			size_ += list.size();
			return iterator{ first };
		}

		template <class... Args>
		iterator emplace(const_iterator position, Args&& ... args) {
			const pointer new_elem = shift(position - begin(), 1);
			std::allocator_traits<Alloc>::construct(alloc_, new_elem, std::forward<Args>(args)...);
			++size_;
			return iterator{ new_elem };
		}

		iterator erase(const_iterator position) {
			const pointer p = const_cast<pointer>(const_pointer{ position });
			std::allocator_traits<Alloc>::destroy(alloc_, p);
			move(p + 1, p, size_ - (p - firstElemPtr_) - 1);
			--size_;
			return iterator{ p };
		}

		iterator erase(const_iterator first, const_iterator last) {
			const pointer f = const_cast<pointer>(const_pointer{ first });
			const pointer l = const_cast<pointer>(const_pointer{ last });

			for (pointer it = f; it != l; ++it)
				std::allocator_traits<Alloc>::destroy(alloc_, it);

			move(l, f, size_ - (l - firstElemPtr_));
			size_ -= l - f;

			return iterator{ f };
		}

		void push_back(const T& x) { emplace_back(x); }
		void push_back(T&& x) { emplace_back(std::move(x)); }

		template <class... Args>
		void emplace_back(Args&& ... args) { emplace(cend(), std::forward<Args>(args)...); }

		void pop_back() { erase(cend() - 1); }

		void resize(size_type sz) {
			if (size_ < sz) {
				inc_cap(sz);
				pointer it = firstElemPtr_ + sz;
				for (size_type i = sz - size_; i; --i)
					std::allocator_traits<Alloc>::construct(alloc_, it++);
				size_ = sz;
			}
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}
		void resize(size_type sz, const T& c) {
			if (size_ < sz)
				insert(cbegin() + size_, sz - size_, c);
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}

		void swap(vector& other)
			noexcept(std::allocator_traits<Alloc>::propagate_on_container_swap::value
				|| std::allocator_traits<Alloc>::is_always_equal::value)
		{
			vector t{ std::move(other) };
			other = std::move(*this);
			*this = std::move(t);
		}

	private:
		Alloc alloc_;
		pointer firstElemPtr_ = nullptr;
		size_type capacity_ = 0;
		size_type size_ = 0;

		pointer shift(const size_type position, const size_type count) {
			if (capacity_ == 0) {
				capacity_ = new_cap(count);
				firstElemPtr_ = alloc_.allocate(capacity_);
				return firstElemPtr_;
			}

			const size_type minReqCap = size_ + count;
			const size_type recommendedCap = new_cap(minReqCap);
			const pointer newMem = capacity_ < minReqCap ? alloc_.allocate(recommendedCap) : nullptr;

			move(firstElemPtr_ + position, (newMem ? newMem : firstElemPtr_) + position + count, size_ - position);

			if (newMem) {
				move(firstElemPtr_, newMem, position);
				alloc_.deallocate(firstElemPtr_, capacity_);
				firstElemPtr_ = newMem;
				capacity_ = recommendedCap;
			}

			return firstElemPtr_ + position;
		}

		void move(pointer src, pointer dest, size_type count) {
			if (src == dest || count == 0) return;
			static constexpr auto inc = [](pointer& p) {++p; };
			static constexpr auto dec = [](pointer& p) {--p; };
			void(*op)(pointer&) = inc;
			if (src < dest) {
				op = dec;
				src += count - 1;
				dest += count - 1;
			}
			while (count--) {
				std::allocator_traits<Alloc>::construct(alloc_, dest, std::move(*src));
				std::allocator_traits<Alloc>::destroy(alloc_, src);
				op(src); op(dest);
			}
		}

		void inc_cap(const size_type minReqCap) {
			if (capacity_ < minReqCap) {
				const size_type newCap = new_cap(minReqCap);
				const pointer newMem = alloc_.allocate(newCap);
				move(firstElemPtr_, newMem, size_);
				alloc_.deallocate(firstElemPtr_, capacity_);
				firstElemPtr_ = newMem;
				capacity_ = newCap;
			}
		}

		[[nodiscard]] size_type new_cap(const size_type required) const {
			size_type newCap = std::max<size_type>(1, capacity_);
			while (newCap < required)
				newCap = std::max<size_type>(2, newCap * 3 / 2);
			return newCap;
		}
	};

	template <class T, class Alloc>
	[[nodiscard]] typename vector<T, Alloc>::const_iterator operator+
		(typename vector<T, Alloc>::difference_type n, typename vector<T, Alloc>::const_iterator it)
	{ return it + n; }

	template <class T, class Alloc>
	[[nodiscard]] typename vector<T, Alloc>::iterator operator+
		(typename vector<T, Alloc>::difference_type n, typename vector<T, Alloc>::iterator it)
	{ return it + n; }

	namespace pmr {
		template <class T>
		using vector = ::ostl::vector<T, ::std::pmr::polymorphic_allocator<T>>;
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
		if (lhs.size() != rhs.size()) return false;
		const auto end = lhs.end();
		for (auto l = lhs.begin(), r = rhs.begin(); l != end;)
			if (*l++ != *r++) return false;
		return true;
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
		return !(lhs == rhs);
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
		return lhs < rhs || lhs == rhs;
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
		return !(lhs <= rhs);
	}

	template <class T, class Alloc>
	[[nodiscard]] bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
		return !(lhs < rhs);
	}

	template <class T, class Alloc>
	void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

	template <class T, class Alloc, class U>
	void erase(vector<T, Alloc>& c, const U& value) {
		c.erase(std::remove(c.begin(), c.end(), value), c.end());
	}

	template <class T, class Alloc, class Pred>
	void erase(vector<T, Alloc>& c, Pred pred) {
		c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
	}

	// Deduction guide (C++17)
	template <class InputIt, class Alloc = std::allocator<typename std::iterator_traits<InputIt>::value_type>>
	vector(InputIt, InputIt, Alloc = Alloc{})->vector<typename std::iterator_traits<InputIt>::value_type, Alloc>;

	template <class Alloc>
	class vector<bool, Alloc> {
	public:
		using value_type = bool;
		using allocator_type = Alloc;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using const_reference = bool;

	private:
		using _T = unsigned;
		using _NBT = unsigned char;
		static constexpr _NBT _nBit = sizeof _T * 8;

	public:
		class reference {
		public:
			[[nodiscard]] operator bool() const noexcept { return ref >> bitOffset & 1; }
			reference& operator=(const bool x) noexcept {
				const _T b = 1 << bitOffset;
				if (x) ref |= b;
				else ref &= ~b;
				return *this;
			}
			reference& operator=(const reference& x) noexcept { return *this = bool{ x }; }
			void flip() noexcept { *this = !*this; }

		private:
			friend class vector<bool, Alloc>;
			friend class vector<bool, Alloc>::iterator;
			reference(_T& ref, _NBT bitOffset) noexcept :ref{ ref }, bitOffset{ bitOffset } {}
			_T& ref;
			const _NBT bitOffset;
		};

		class const_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = bool;
			using difference_type = difference_type;
			using pointer = bool*;
			using reference = bool;

			const_iterator() = default;
			const_iterator(_T* firstElemPtr, size_type idx) :ptr{ firstElemPtr + idx / _nBit }, bitOffset{ idx % _nBit } {}

			[[nodiscard]] bool operator*() const { return *ptr << bitOffset & 1; }
			[[nodiscard]] bool operator[](difference_type n) const { return ptr[n / _nBit] << (bitOffset + n) % _nBit & 1; }
			const_iterator& operator++() { return *this += 1; }
			const_iterator operator++(int) { const_iterator it = *this; ++*this; return it; }
			const_iterator& operator--() { return *this -= 1; }
			const_iterator operator--(int) { const_iterator it = *this; --*this; return it; }
			const_iterator& operator+=(difference_type n) {
				const difference_type d = bitOffset + n;
				ptr += (d < 0 ? d - (_nBit - 1) : d) / _nBit;
				bitOffset = d & (_nBit - 1);
				return *this;
			}
			[[nodiscard]] const_iterator operator+(difference_type n) const { const_iterator it = *this; it += n; return it; }
			const_iterator& operator-=(difference_type n) { return *this += -n; }
			[[nodiscard]] const_iterator operator-(difference_type n) const { return *this + -n; }
			[[nodiscard]] difference_type operator-(const const_iterator& rhs) const { return (ptr - rhs.ptr) * _nBit + (bitOffset - rhs.bitOffset); }
			[[nodiscard]] bool operator==(const const_iterator& rhs) const { return ptr == rhs.ptr && bitOffset == rhs.bitOffset; }
			[[nodiscard]] bool operator!=(const const_iterator& rhs) const { return !(*this == rhs); }
			[[nodiscard]] bool operator<(const const_iterator& rhs) const { return ptr < rhs.ptr || ptr == rhs.ptr && bitOffset < rhs.bitOffset; }
			[[nodiscard]] bool operator>(const const_iterator& rhs) const { return rhs < *this; }
			[[nodiscard]] bool operator>=(const const_iterator& rhs) const { return !(*this < rhs); }
			[[nodiscard]] bool operator<=(const const_iterator& rhs) const { return !(*this > rhs); }

		protected:
			_T* ptr = nullptr;
			_NBT bitOffset = 0;
		};

		class iterator : public const_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = bool;
			using difference_type = difference_type;
			using pointer = vector::reference*;
			using reference = vector::reference;

			iterator() = default;
			iterator(_T* firstElemPtr, size_type idx) :const_iterator{ firstElemPtr, idx } {}

			[[nodiscard]] reference operator*() const { return { *ptr, bitOffset }; }
			[[nodiscard]] reference operator[](difference_type n) const { return { ptr[n / _nBit], (bitOffset + n) % _nBit }; }
			iterator& operator++() { return *this += 1; }
			iterator operator++(int) { iterator it = *this; ++* this; return it; }
			iterator& operator--() { return *this -= 1; }
			iterator operator--(int) { iterator it = *this; --* this; return it; }
			iterator& operator+=(difference_type n) {
				const difference_type d = bitOffset + n;
				ptr += (d < 0 ? d - (_nBit - 1) : d) / _nBit;
				bitOffset = d & (_nBit - 1);
				return *this;
			}
			[[nodiscard]] iterator operator+(difference_type n) const { iterator it = *this; it += n; return it; }
			iterator& operator-=(difference_type n) { return *this += -n; }
			[[nodiscard]] iterator operator-(difference_type n) const { return *this + -n; }
			[[nodiscard]] difference_type operator-(const iterator& rhs) const { return (ptr - rhs.ptr) * _nBit + (bitOffset - rhs.bitOffset); }
			[[nodiscard]] bool operator==(const iterator& rhs) const { return ptr == rhs.ptr && bitOffset == rhs.bitOffset; }
			[[nodiscard]] bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
			[[nodiscard]] bool operator<(const iterator& rhs) const { return ptr < rhs.ptr || ptr == rhs.ptr && bitOffset < rhs.bitOffset; }
			[[nodiscard]] bool operator>(const iterator& rhs) const { return rhs < *this; }
			[[nodiscard]] bool operator>=(const iterator& rhs) const { return !(*this < rhs); }
			[[nodiscard]] bool operator<=(const iterator& rhs) const { return !(*this > rhs); }
		};

		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using pointer = iterator;
		using const_pointer = const_iterator;

		vector() = default;
		explicit vector(const Alloc& alloc) noexcept :vec_{ alloc } {}

		vector(size_type n, bool value, const Alloc& alloc = Alloc{})
			:vec_{ (n + (_nBit - 1)) / _nBit, 0 - value, alloc }, size_{ n }
		{ vec_.back() = ((_T)value << n - _nBit * (vec_.size() - 1)) - 1; }

		explicit vector(size_type n, const Alloc& alloc = Alloc{})
			:vec_{ (n + (_nBit - 1)) / _nBit, alloc }, size_{ n } { }

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
		vector(InputIt first, InputIt last, const Alloc& alloc = Alloc{})
			:size_{ std::distance(first, last) }
		{
			const size_type s = (size_ + (_nBit - 1)) / _nBit;
			vec_.reserve(s);
			for (size_type i = 0; i < s; ++i) {
				_T bits = 0;
				for (_NBT j = 0; j < _nBit && first != last; ++j)
					bits |= *first++ << j;
				vec_.push_back(bits);
			}
		}

		vector(const vector&) = default;
		vector(const vector& x, const Alloc& alloc) :vec_{ x.vec_, alloc }, size_{ x.size_ } {}
		vector(vector&& x) noexcept :vec_{ std::move(x) }, size_{ x.size_ } { x.size_ = 0; }
		vector(vector&& x, const Alloc& alloc) :vec_{ std::move(x), alloc }, size_{ x.size_ } { x.size_ = 0; }
		vector(std::initializer_list<bool> init, const Alloc& alloc = Alloc{}) :vector{ init.begin(), init.end(), alloc } {}

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

		void assign(const size_type n, bool t) { *this = vector(n, t, vec_.get_allocator()); }

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
		void assign(InputIt first, InputIt last) { *this = vector(first, last, vec_.get_allocator()); }

		void assign(std::initializer_list<T> init) { *this = vector(init, vec_.get_allocator()); }

		[[nodiscard]] allocator_type get_allocator() const noexcept { return vec_.get_allocator(); }

		[[nodiscard]] reference       at(size_type n) { if (n >= size_) throw std::out_of_range{ "" }; return (*this)[n]; }
		[[nodiscard]] const_reference at(size_type n) const { if (n >= size_) throw std::out_of_range{ "" }; return (*this)[n]; }
		[[nodiscard]] reference       operator[](size_type n) { return begin()[n]; }
		[[nodiscard]] const_reference operator[](size_type n) const { return begin()[n]; }
		[[nodiscard]] reference       front() { return *begin(); }
		[[nodiscard]] const_reference front() const { return *begin(); }
		[[nodiscard]] reference       back() { return end()[-1]; }
		[[nodiscard]] const_reference back() const { return end()[-1]; }

		[[nodiscard]] iterator                begin() noexcept { return { vec_.data(), 0 }; }
		[[nodiscard]] const_iterator          begin() const noexcept { return { vec_.data(), 0 }; }
		[[nodiscard]] const_iterator          cbegin() const noexcept { return { vec_.data(), 0 }; }

		[[nodiscard]] iterator                end() noexcept { return { vec_.data(), size_ - 1 }; }
		[[nodiscard]] const_iterator          end() const noexcept { return { vec_.data(), size_ - 1 }; }
		[[nodiscard]] const_iterator          cend() const noexcept { return { vec_.data(), size_ - 1 }; }

		[[nodiscard]] reverse_iterator        rbegin() noexcept { return reverse_iterator{ end() }; }
		[[nodiscard]] const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator{ end() }; }
		[[nodiscard]] const_reverse_iterator  crbegin() const noexcept { return const_reverse_iterator{ cend() }; }

		[[nodiscard]] reverse_iterator        rend() noexcept { return reverse_iterator{ begin() }; }
		[[nodiscard]] const_reverse_iterator  rend() const noexcept { return const_reverse_iterator{ begin() }; }
		[[nodiscard]] const_reverse_iterator  crend() const noexcept { return const_reverse_iterator{ cbegin() }; }

		[[nodiscard]] bool empty() const noexcept { return size_ == 0; }
		[[nodiscard]] size_type size() const noexcept { return size_; }
		[[nodiscard]] constexpr size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }
		void reserve(const size_type n) { vec_.reserve((n + (_nBit - 1)) / _nBit); }
		[[nodiscard]] size_type capacity() const noexcept { return vec_.capacity() * _nBit; }

		void clear() noexcept { vec_.clear(); size_ = 0; }

		iterator insert(const_iterator position, bool x) { return insert(position, 1, x); }
		iterator insert(const_iterator position, size_type n, bool x) {
			iterator it = begin();
			const difference_type d = position - it;
			move(position, position + n, size_ - d);
			it += d;
			const iterator ret = it;
			while (n--) *it++ = x;
			return it;
		}

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
		iterator insert(const_iterator position, InputIt first, InputIt last) {
			iterator it = begin();
			const difference_type d = position - it;
			difference_type n = std::distance(first, last);
			move(position, position + n, size_ - d);
			it += d;
			const iterator ret = it;
			while (n--) *it++ = *first++;
			return it;
		}

		iterator insert(const_iterator position, std::initializer_list<bool> list) {
			insert(position, list.begin(), list.end());
		}

		template <class... Args>
		iterator emplace(const_iterator position, Args&& ... args) {
			return insert(position, bool{ std::forward<Args>(args)... });
		}

		iterator erase(const_iterator position) {
			return erase(position, position + 1);
		}

		iterator erase(const_iterator first, const_iterator last) {
			move(last, first, size_ - (last - begin()));
			size_ -= last - first;
			return begin() + (first - begin());
		}

		void push_back(bool x) { insert(cend(), x); }

		template <class... Args>
		void emplace_back(Args&& ... args) { push_back(bool{ std::forward<Args>(args)... }); }

		void pop_back() { erase(cend() - 1); }

		void resize(size_type sz) {
			if (size_ < sz) {
				inc_cap(sz);
				iterator it = begin() + sz;
				for (size_type i = sz - size_; i; --i)
					* it++ = false;
				size_ = sz;
			}
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}
		void resize(size_type sz, bool c) {
			if (size_ < sz)
				insert(cbegin() + size_, sz - size_, c);
			else if (size_ > sz)
				erase(cbegin() + sz, cend());
		}

		void swap(vector& other)
			noexcept(std::allocator_traits<Alloc>::propagate_on_container_swap::value
				|| std::allocator_traits<Alloc>::is_always_equal::value)
		{
			vector t{ std::move(other) };
			other = std::move(*this);
			*this = std::move(t);
		}

	private:
		void move(const_iterator src, const_iterator dest, size_type cnt) {
			const difference_type d = dest - src;
			if (d == 0 || cnt == 0) return;
			if (d > 0) {
				inc_cap(size_ + d);
				for (size_type i = src - begin() + cnt - 1; cnt--; --i)
					(*this)[i + d] = static_cast<const vector&>(*this)[i];
			}
			else {
				for (size_type i = src - begin(); cnt--; ++i)
					(*this)[i + d] = static_cast<const vector&>(*this)[i];
			}
		}

		void inc_cap(size_type min) {
			if (min > vec_.size() * _nBit)
				vec_.insert(vec_.end(), (min - vec_.size() * _nBit + _nBit - 1) / _nBit, 0);
		}

		vector<_T, std::allocator_traits<Alloc>::rebind_alloc<_T>> vec_;
		size_type size_ = 0;
	};
}
