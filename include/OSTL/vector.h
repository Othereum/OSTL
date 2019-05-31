#include <initializer_list>
#include <iterator>
#include <limits>

namespace ostl
{
	template <class V>
	class _VecConstIt
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = typename V::value_type;
		using difference_type = typename V::difference_type;
		using reference = typename V::const_reference;
		using pointer = typename V::const_pointer;
		using _Tptr = typename V::pointer;

		constexpr _VecConstIt() = default;
		constexpr _VecConstIt(_Tptr Data) :_Ptr{ Data } {}
		constexpr operator pointer() const { return _Ptr; }

		constexpr reference operator*() const { return *_Ptr; }
		constexpr reference operator[](difference_type n) const { return *(_Ptr + n); }
		constexpr pointer operator->() const { return _Ptr; }
		constexpr _VecConstIt& operator++() { ++_Ptr; return *this; }
		constexpr _VecConstIt operator++(int) { _VecConstIt it = *this; ++_Ptr; return it; }
		constexpr _VecConstIt& operator--() { --_Ptr; return *this }
		constexpr _VecConstIt operator--(int) { _VecConstIt it = *this; --_Ptr; return it; }
		constexpr _VecConstIt& operator+=(difference_type n) { _Ptr += n; return *this; }
		constexpr _VecConstIt operator+(difference_type n) const { return _Ptr + n; }
		constexpr _VecConstIt& operator-=(difference_type n) { _Ptr -= n; return *this; }
		constexpr _VecConstIt operator-(difference_type n) const { return _Ptr - n; }
		constexpr difference_type operator-(const _VecConstIt& rhs) const { return _Ptr - rhs._Ptr; }
		constexpr bool operator==(const _VecConstIt& rhs) const { return _Ptr == rhs._Ptr; }
		constexpr bool operator!=(const _VecConstIt& rhs) const { return _Ptr != rhs._Ptr; }
		constexpr bool operator<(const _VecConstIt& rhs) const { return _Ptr < rhs._Ptr; }
		constexpr bool operator>(const _VecConstIt& rhs) const { return _Ptr > rhs._Ptr; }
		constexpr bool operator>=(const _VecConstIt& rhs) const { return _Ptr >= rhs._Ptr; }
		constexpr bool operator<=(const _VecConstIt& rhs) const { return _Ptr <= rhs._Ptr; }

	protected:
		_Tptr _Ptr = nullptr;
	};

	template <class V>
	class _VecIt : public _VecConstIt<V>
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = typename V::value_type;
		using difference_type = typename V::difference_type;
		using reference = typename V::reference;
		using pointer = typename V::pointer;

		constexpr _VecIt() = default;
		constexpr _VecIt(pointer Data) :_VecConstIt<V>{ Data } {}
		constexpr operator pointer() const { return _Ptr; }

		constexpr reference operator*() const { return *_Ptr; }
		constexpr reference operator[](difference_type n) const { return *(_Ptr + n); }
		constexpr pointer operator->() const { return _Ptr; }
		constexpr _VecIt& operator++() { ++_Ptr; return *this; }
		constexpr _VecIt operator++(int) { _VecIt it = *this; ++_Ptr; return it; }
		constexpr _VecIt& operator--() { --_Ptr; return *this }
		constexpr _VecIt operator--(int) { _VecIt it = *this; --_Ptr; return it; }
		constexpr _VecIt& operator+=(difference_type n) { _Ptr += n; return *this; }
		constexpr _VecIt operator+(difference_type n) const { return _Ptr + n; }
		constexpr _VecIt& operator-=(difference_type n) { _Ptr -= n; return *this; }
		constexpr _VecIt operator-(difference_type n) const { return _Ptr - n; }
		constexpr difference_type operator-(const _VecIt& rhs) const { return _Ptr - rhs._Ptr; }
		constexpr bool operator==(const _VecIt& rhs) const { return _Ptr == rhs._Ptr; }
		constexpr bool operator!=(const _VecIt& rhs) const { return _Ptr != rhs._Ptr; }
		constexpr bool operator<(const _VecIt& rhs) const { return _Ptr < rhs._Ptr; }
		constexpr bool operator>(const _VecIt& rhs) const { return _Ptr > rhs._Ptr; }
		constexpr bool operator>=(const _VecIt& rhs) const { return _Ptr >= rhs._Ptr; }
		constexpr bool operator<=(const _VecIt& rhs) const { return _Ptr <= rhs._Ptr; }
	};

	template <class T, class Allocator = std::allocator<T> >
	class vector
	{
	public:
		using value_type = T;
		using allocator_type = Allocator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type &;
		using const_reference = const value_type &;
		using pointer = typename std::allocator_traits<allocator_type>::pointer;
		using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
		using iterator = _VecIt<vector>;
		using const_iterator = _VecConstIt<vector>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		vector() noexcept(noexcept(Allocator{})) {}
		explicit vector(const Allocator& alloc) noexcept :alloc_{ alloc } {}

		vector(size_type n, const T& value, const Allocator& alloc = Allocator{})
			:alloc_{ alloc }, firstElemPtr_{ alloc_.allocate(n) }, capacity_{ n }, size_{ n }
		{
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Allocator>::construct(alloc_, firstElemPtr_ + i, value);
		}

		explicit vector(size_type n, const Allocator& alloc = Allocator{})
			:alloc_{ alloc }, firstElemPtr_{ alloc_.allocate(n) }, capacity_{ n }, size_{ n }
		{
			for (size_type i = 0; i < size_; ++i)
				alloc_.construct(firstElemPtr_ + i);
		}

		template <class InputIt>
		vector(InputIt first, InputIt last, const Allocator& alloc = Allocator{})
			: alloc_{ alloc }
		{
			for (InputIt it = first; it != last; ++it)
				emplace_back(*it);
		}

		vector(const vector& x)
			:alloc_{ x.alloc_ }, firstElemPtr_{ alloc_.allocate(x.size_) }, capacity_{ x.size_ }, size_{ x.size_ }
		{
			pointer p = firstElemPtr_;
			for (const value_type& a : x)
				alloc_.construct(p++, a);
		}

		vector(const vector& x, const Allocator& alloc)
			:alloc_{ alloc }, firstElemPtr_{ alloc_.allocate(x.size_) }, capacity_{ x.size_ }, size_{ x.size_ }
		{
			pointer p = firstElemPtr_;
			for (const value_type& a : x)
				alloc_.construct(p++, a);
		}

		vector(vector&& x) noexcept
			:alloc_{ x.alloc_ }, firstElemPtr_{ x.firstElemPtr_ }, capacity_{ x.capacity_ }, size_{ x.size_ }
		{
			x.firstElemPtr_ = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
		}

		vector(vector&& x, const Allocator& alloc)
			:alloc_{ alloc }, firstElemPtr_{ x.firstElemPtr_ }, capacity_{ x.capacity_ }, size_{ x.size_ }
		{
			x.firstElemPtr_ = nullptr;
			x.capacity_ = 0;
			x.size_ = 0;
		}

		vector(std::initializer_list<T> init, const Allocator& alloc = Allocator{})
			:alloc_{ alloc }
		{
			const size_type s = init.size();
			reserve(s);
			size_ = s;
			pointer p = firstElemPtr_;
			for (auto it = init.begin(); it != init.end(); ++it)
				alloc_.construct(p++, *it);
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
				alloc_.construct(p++, a);
		}
		
		vector& operator=(vector&& x)
			noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value
				|| std::allocator_traits<Allocator>::is_always_equal::value)
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
		}
		
		vector& operator=(std::initializer_list<T> init) {
			clear();
			const size_type s = init.size();
			reserve(s);
			size_ = s;
			pointer p = firstElemPtr_;
			for (auto it = init.begin(); it != init.end(); ++it)
				alloc_.construct(p++, *it);
		}

		void assign(const size_type n, const T& t) {
			clear();
			reserve(n);
			size_ = n;
			for (size_type i = 0; i < n; ++i)
				alloc_.construct(firstElemPtr_ + i, t);
		}

		template <class InputIt>
		void assign(const InputIt first, const InputIt last) {
			clear();
			for (InputIt it = first; it != last; ++it)
				emplace_back(*it);
		}

		void assign(std::initializer_list<T> init) {
			clear();
			const size_type s = init.size();
			reserve(s);
			size_ = s;
			pointer p = firstElemPtr_;
			for (auto it = init.begin(); it != init.end(); ++it)
				alloc_.construct(p++, *it);
		}

		allocator_type get_allocator() const noexcept { return alloc_; }

		reference       at(size_type n) { return const_cast<reference>(static_cast<const vector&>(*this).at(n)); }
		const_reference at(size_type n) const {
			if (n >= size_) throw std::out_of_range{ "" };
			return firstElemPtr_[n];
		}
		reference       operator[](size_type n) { return const_cast<reference>(static_cast<const vector&>(*this)[n]); }
		const_reference operator[](size_type n) const { return firstElemPtr_[n]; }
		reference       front() { return const_cast<reference>(static_cast<const vector&>(*this).front()); }
		const_reference front() const { return *firstElemPtr_; }
		reference       back() { return const_cast<reference>(static_cast<const vector&>(*this).back()); }
		const_reference back() const { return firstElemPtr_[size_ - 1]; }

		pointer data() noexcept { return const_cast<pointer>(static_cast<const vector&>(*this).data()); }
		const_pointer data() const noexcept { return firstElemPtr_; }

		iterator                begin() noexcept { return firstElemPtr_; }
		const_iterator          begin() const noexcept { return firstElemPtr_; }
		const_iterator          cbegin() const noexcept { return firstElemPtr_; }

		iterator                end() noexcept { return firstElemPtr_ + size_; }
		const_iterator          end() const noexcept { return firstElemPtr_ + size_; }
		const_iterator          cend() const noexcept { return firstElemPtr_ + size_; }

		reverse_iterator        rbegin() noexcept { return reverse_iterator{ firstElemPtr_ + size_ - 1 }; }
		const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator{ firstElemPtr_ + size_ - 1 }; }
		const_reverse_iterator  crbegin() const noexcept { return const_reverse_iterator{ firstElemPtr_ + size_ - 1 }; }

		reverse_iterator        rend() noexcept { return reverse_iterator{ firstElemPtr_ - 1 }; }
		const_reverse_iterator  rend() const noexcept { return const_reverse_iterator{ firstElemPtr_ - 1 }; }
		const_reverse_iterator  crend() const noexcept { return const_reverse_iterator{ firstElemPtr_ - 1 }; }

		[[nodiscard]] bool empty() const noexcept { return size_ == 0; }
		size_type size() const noexcept { return size_; }
		size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }
		void      reserve(const size_type n) {
			if (n > max_size()) throw std::length_error{ "" };
			if (n > capacity_) {
				const pointer p = alloc_.allocate(n);
				if (firstElemPtr_) {
					for (size_type i = 0; i < size_; ++i) {
						alloc_.construct(p + i, std::move(firstElemPtr_[i]));
						alloc_.destroy(firstElemPtr_ + i);
					}
					alloc_.deallocate(firstElemPtr_, capacity_);
				}
				firstElemPtr_ = p;
				capacity_ = n;
			}
		}
		size_type capacity() const noexcept { return capacity_; }
		void      shrink_to_fit() {
			if (size_ < capacity_) {
				if (size_ == 0) {
					alloc_.deallocate(firstElemPtr_, capacity_);
					firstElemPtr_ = nullptr;
					capacity_ = 0;
				} else {
					const pointer n = alloc_.allocate(size_);
					for (size_type i = 0; i < size_; ++i) {
						std::allocator_traits<Allocator>::construct(alloc_, n + i, std::move(firstElemPtr_[i]));
						std::allocator_traits<Allocator>::destroy(alloc_, firstElemPtr_ + i);
					}
					alloc_.deallocate(firstElemPtr_, capacity_);
					firstElemPtr_ = n;
					capacity_ = size_;
				}
			}
		}

		void     clear() noexcept {
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Allocator>::destroy(alloc_, firstElemPtr_ + i);
			size_ = 0;
		}
		iterator insert(const_iterator position, const T& x) {
			const pointer src = firstElemPtr_ + size_ - 1, dest = (size_ + 1 > capacity_ ? alloc_.allocate(capacity_ * 2) : firstElemPtr_) + size_;
			const size_type moveCnt = end() - position, newElemOffset = position - begin();
			for (size_type i = 0; i < moveCnt; ++i) {
				std::allocator_traits<Allocator>::construct(alloc_, dest - i, std::move(*(src - i)));
				std::allocator_traits<Allocator>::destroy(alloc_, src - i);
			}
			alloc_.deallocate(firstElemPtr_, capacity_);
			if (firstElemPtr_ != dest - size_)
			{
				firstElemPtr_ = dest - size_;
				capacity_ *= 2;
			}
			const pointer new_elem = firstElemPtr_ + newElemOffset;
			std::allocator_traits<Allocator>::construct(alloc_, new_elem, x);
			++size_;
			return new_elem;
		}
		iterator insert(const_iterator pos, T&& x);
		iterator insert(const_iterator pos, size_type n, const T& x);
		template <class InputIt>
		iterator insert(const_iterator pos, InputIt first,
			InputIt last);
		iterator insert(const_iterator pos, std::initializer_list<T>);
		void      resize(size_type sz);
		void      resize(size_type sz, const T& c);
		template <class... Args> void emplace_back(Args&& ... args);
		void push_back(const T& x);
		void push_back(T&& x);
		void pop_back();

		template <class... Args> iterator emplace(const_iterator position, Args&& ... args);

		iterator erase(const_iterator position);
		iterator erase(const_iterator first, const_iterator last);
		void     swap(vector<T, Allocator>&);

	private:
		Allocator alloc_;
		pointer firstElemPtr_ = nullptr;
		size_type capacity_ = 0;
		size_type size_ = 0;
	};
}

template <class V>
ostl::_VecConstIt<V> operator+(typename ostl::_VecConstIt<V>::difference_type n, ostl::_VecConstIt<V> it) {
	return it + n;
}
