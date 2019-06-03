#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>

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

		_VecConstIt() = default;
		explicit _VecConstIt(_Tptr Data) :_Ptr{ Data } {}
		explicit operator pointer() const { return _Ptr; }

		reference operator*() const { return *_Ptr; }
		reference operator[](difference_type n) const { return *(_Ptr + n); }
		pointer operator->() const { return _Ptr; }
		_VecConstIt& operator++() { ++_Ptr; return *this; }
		_VecConstIt operator++(int) { _VecConstIt it = *this; ++_Ptr; return it; }
		_VecConstIt& operator--() { --_Ptr; return *this; }
		_VecConstIt operator--(int) { _VecConstIt it = *this; --_Ptr; return it; }
		_VecConstIt& operator+=(difference_type n) { _Ptr += n; return *this; }
		_VecConstIt operator+(difference_type n) const { return _VecConstIt{ _Ptr + n }; }
		_VecConstIt& operator-=(difference_type n) { _Ptr -= n; return *this; }
		_VecConstIt operator-(difference_type n) const { return _VecConstIt{ _Ptr - n }; }
		difference_type operator-(const _VecConstIt& rhs) const { return _Ptr - rhs._Ptr; }
		bool operator==(const _VecConstIt& rhs) const { return _Ptr == rhs._Ptr; }
		bool operator!=(const _VecConstIt& rhs) const { return _Ptr != rhs._Ptr; }
		bool operator<(const _VecConstIt& rhs) const { return _Ptr < rhs._Ptr; }
		bool operator>(const _VecConstIt& rhs) const { return _Ptr > rhs._Ptr; }
		bool operator>=(const _VecConstIt& rhs) const { return _Ptr >= rhs._Ptr; }
		bool operator<=(const _VecConstIt& rhs) const { return _Ptr <= rhs._Ptr; }

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

		_VecIt() = default;
		explicit _VecIt(pointer Data) :_VecConstIt<V>{ Data } {}
		explicit operator pointer() const { return _Ptr; }

		reference operator*() const { return *_Ptr; }
		reference operator[](difference_type n) const { return *(_Ptr + n); }
		pointer operator->() const { return _Ptr; }
		_VecIt& operator++() { ++_Ptr; return *this; }
		_VecIt operator++(int) { _VecIt it = *this; ++_Ptr; return it; }
		_VecIt& operator--() { --_Ptr; return *this }
		_VecIt operator--(int) { _VecIt it = *this; --_Ptr; return it; }
		_VecIt& operator+=(difference_type n) { _Ptr += n; return *this; }
		_VecIt operator+(difference_type n) const { return _VecIt{ _Ptr + n }; }
		_VecIt& operator-=(difference_type n) { _Ptr -= n; return *this; }
		_VecIt operator-(difference_type n) const { return _VecIt{ _Ptr - n }; }
		difference_type operator-(const _VecIt& rhs) const { return _Ptr - rhs._Ptr; }
		bool operator==(const _VecIt& rhs) const { return _Ptr == rhs._Ptr; }
		bool operator!=(const _VecIt& rhs) const { return _Ptr != rhs._Ptr; }
		bool operator<(const _VecIt& rhs) const { return _Ptr < rhs._Ptr; }
		bool operator>(const _VecIt& rhs) const { return _Ptr > rhs._Ptr; }
		bool operator>=(const _VecIt& rhs) const { return _Ptr >= rhs._Ptr; }
		bool operator<=(const _VecIt& rhs) const { return _Ptr <= rhs._Ptr; }
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
				std::allocator_traits<Allocator>::construct(alloc_, firstElemPtr_ + i);
		}

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
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
				std::allocator_traits<Allocator>::construct(alloc_, p++, a);
		}

		vector(const vector& x, const Allocator& alloc)
			:alloc_{ alloc }, firstElemPtr_{ alloc_.allocate(x.size_) }, capacity_{ x.size_ }, size_{ x.size_ }
		{
			pointer p = firstElemPtr_;
			for (const value_type& a : x)
				std::allocator_traits<Allocator>::construct(alloc_, p++, a);
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
				std::allocator_traits<Allocator>::construct(alloc_, p++, *it);
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
				std::allocator_traits<Allocator>::construct(alloc_, p++, a);
			return *this;
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
			return *this;
		}
		
		vector& operator=(std::initializer_list<T> init) {
			clear();
			const size_type s = init.size();
			reserve(s);
			size_ = s;
			pointer p = firstElemPtr_;
			for (auto it = init.begin(); it != init.end(); ++it)
				std::allocator_traits<Allocator>::construct(alloc_, p++, *it);
			return *this;
		}

		void assign(const size_type n, const T& t) {
			clear();
			reserve(n);
			size_ = n;
			for (size_type i = 0; i < n; ++i)
				std::allocator_traits<Allocator>::construct(alloc_, firstElemPtr_ + i, t);
		}

		template <class InputIt, class = std::enable_if_t<
			std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>
			|| std::is_same_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> >>
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
				std::allocator_traits<Allocator>::construct(alloc_, p++, *it);
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

		iterator                begin() noexcept { return iterator{ firstElemPtr_ }; }
		const_iterator          begin() const noexcept { return const_iterator{ firstElemPtr_ }; }
		const_iterator          cbegin() const noexcept { return const_iterator{ firstElemPtr_ }; }

		iterator                end() noexcept { return iterator{ firstElemPtr_ + size_ }; }
		const_iterator          end() const noexcept { return const_iterator{ firstElemPtr_ + size_ }; }
		const_iterator          cend() const noexcept { return const_iterator{ firstElemPtr_ + size_ }; }

		reverse_iterator        rbegin() noexcept { return reverse_iterator{ end() }; }
		const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator{ end() }; }
		const_reverse_iterator  crbegin() const noexcept { return const_reverse_iterator{ cend() }; }

		reverse_iterator        rend() noexcept { return reverse_iterator{ begin() }; }
		const_reverse_iterator  rend() const noexcept { return const_reverse_iterator{ begin() }; }
		const_reverse_iterator  crend() const noexcept { return const_reverse_iterator{ cbegin() }; }

		[[nodiscard]] bool empty() const noexcept { return size_ == 0; }
		size_type size() const noexcept { return size_; }
		size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }

		void reserve(const size_type n) {
			if (n > max_size()) throw std::length_error{ "" };
			if (n <= capacity_) return;

			const pointer p = alloc_.allocate(n);
			if (firstElemPtr_) {
				for (size_type i = 0; i < size_; ++i) {
					std::allocator_traits<Allocator>::construct(alloc_, p + i, std::move(firstElemPtr_[i]));
					std::allocator_traits<Allocator>::destroy(alloc_, firstElemPtr_ + i);
				}
				alloc_.deallocate(firstElemPtr_, capacity_);
			}
			firstElemPtr_ = p;
			capacity_ = n;
		}

		size_type capacity() const noexcept { return capacity_; }

		void shrink_to_fit() {
			if (size_ == capacity_) return;

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

		void clear() noexcept {
			for (size_type i = 0; i < size_; ++i)
				std::allocator_traits<Allocator>::destroy(alloc_, firstElemPtr_ + i);
			size_ = 0;
		}

		iterator insert(const_iterator position, const T& x) { return emplace(position, x); }
		iterator insert(const_iterator position, T&& x) { return emplace(position, std::move(x)); }
		iterator insert(const_iterator position, size_type n, const T& x) {
			const pointer it = shift(position - cbegin(), n);
			for (size_type i = 0; i < n; ++i)
				std::allocator_traits<Allocator>::construct(alloc_, it + i, x);
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
				std::allocator_traits<Allocator>::construct(alloc_, it++, x);
			size_ += list.size();
			return iterator{ first };
		}

		template <class... Args>
		iterator emplace(const_iterator position, Args&& ... args) {
			const pointer new_elem = shift(position - begin(), 1);
			std::allocator_traits<Allocator>::construct(alloc_, new_elem, std::forward<Args>(args)...);
			++size_;
			return iterator{ new_elem };
		}

		iterator erase(const_iterator position) {
			const pointer p = const_cast<pointer>(const_pointer{ position });
			std::allocator_traits<Allocator>::destroy(alloc_, p);
			move(p + 1, p, size_ - (p - firstElemPtr_) - 1);
			--size_;
			return iterator{ p };
		}

		iterator erase(const_iterator first, const_iterator last) {
			const pointer f = const_cast<pointer>(const_pointer{ first });
			const pointer l = const_cast<pointer>(const_pointer{ last });

			for (pointer it = f; it != l; ++it)
				std::allocator_traits<Allocator>::destroy(alloc_, it);

			move(l, f, size_ - (l - firstElemPtr_));
			size_ -= l - f;

			return iterator{ f };
		}

		void push_back(const T& x) { emplace_back(x); }
		void push_back(T&& x) { emplace_back(std::move(x)); }

		template <class... Args>
		void emplace_back(Args&& ... args) { emplace(cend(), std::forward<Args>(args)...); }

		void pop_back() { erase(cend() - 1); }

		void resize(size_type sz);
		void      resize(size_type sz, const T& c);


		void     swap(vector<T, Allocator>&);

	private:
		Allocator alloc_;
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
				std::allocator_traits<Allocator>::construct(alloc_, dest, std::move(*src));
				std::allocator_traits<Allocator>::destroy(alloc_, src);
				op(src); op(dest);
			}
		}

		size_type new_cap(const size_type required) const {
			size_type newCap = capacity_ == 0 && required != 0 ? 1 : capacity_;
			while (newCap < required)
				newCap *= 2;
			return newCap;
		}
	};
}

template <class V>
ostl::_VecConstIt<V> operator+(typename ostl::_VecConstIt<V>::difference_type n, ostl::_VecConstIt<V> it) {
	return it + n;
}
