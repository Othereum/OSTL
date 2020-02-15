#pragma once
#include <iterator>

namespace ostl::internal
{
	template <class V, class D, class P, class R>
	class const_iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = V;
		using difference_type = D;
		using pointer = P;
		using reference = R;

		const_iterator() = default;

		explicit const_iterator(pointer data) : ptr_{data}
		{
		}

		[[nodiscard]] explicit operator pointer() const { return ptr_; }

		[[nodiscard]] reference operator*() const { return *ptr_; }
		[[nodiscard]] reference operator[](difference_type n) const { return *(ptr_ + n); }
		[[nodiscard]] pointer operator->() const { return ptr_; }

		const_iterator& operator++()
		{
			++ptr_;
			return *this;
		}

		const_iterator operator++(int)
		{
			const_iterator it = *this;
			++ptr_;
			return it;
		}

		const_iterator& operator--()
		{
			--ptr_;
			return *this;
		}

		const_iterator operator--(int)
		{
			const_iterator it = *this;
			--ptr_;
			return it;
		}

		const_iterator& operator+=(difference_type n)
		{
			ptr_ += n;
			return *this;
		}

		[[nodiscard]] const_iterator operator+(difference_type n) const { return const_iterator{ptr_ + n}; }

		const_iterator& operator-=(difference_type n)
		{
			ptr_ -= n;
			return *this;
		}

		[[nodiscard]] const_iterator operator-(difference_type n) const { return const_iterator{ptr_ - n}; }
		[[nodiscard]] difference_type operator-(const const_iterator& rhs) const { return ptr_ - rhs.ptr_; }
		[[nodiscard]] bool operator==(const const_iterator& rhs) const { return ptr_ == rhs.ptr_; }
		[[nodiscard]] bool operator!=(const const_iterator& rhs) const { return ptr_ != rhs.ptr_; }
		[[nodiscard]] bool operator<(const const_iterator& rhs) const { return ptr_ < rhs.ptr_; }
		[[nodiscard]] bool operator>(const const_iterator& rhs) const { return ptr_ > rhs.ptr_; }
		[[nodiscard]] bool operator>=(const const_iterator& rhs) const { return ptr_ >= rhs.ptr_; }
		[[nodiscard]] bool operator<=(const const_iterator& rhs) const { return ptr_ <= rhs.ptr_; }

	protected:
		pointer ptr_ = nullptr;
	};

	template <class V, class D, class P, class R>
	class iterator : public const_iterator<V, D, P, R>
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = V;
		using difference_type = D;
		using pointer = P;
		using reference = R;

		iterator() = default;

		explicit iterator(pointer data) : const_iterator{data}
		{
		}

		[[nodiscard]] explicit operator pointer() const { return this->ptr_; }

		[[nodiscard]] reference operator*() const { return *this->ptr_; }
		[[nodiscard]] reference operator[](difference_type n) const { return *(this->ptr_ + n); }
		[[nodiscard]] pointer operator->() const { return this->ptr_; }

		iterator& operator++()
		{
			++this->ptr_;
			return *this;
		}

		iterator operator++(int)
		{
			iterator it = *this;
			++this->ptr_;
			return it;
		}

		iterator& operator--()
		{
			--this->ptr_;
			return *this;
		}

		iterator operator--(int)
		{
			iterator it = *this;
			--this->ptr_;
			return it;
		}

		iterator& operator+=(difference_type n)
		{
			this->ptr_ += n;
			return *this;
		}

		[[nodiscard]] iterator operator+(difference_type n) const { return iterator{this->ptr_ + n}; }

		iterator& operator-=(difference_type n)
		{
			this->ptr_ -= n;
			return *this;
		}

		[[nodiscard]] iterator operator-(difference_type n) const { return iterator{this->ptr_ - n}; }
		[[nodiscard]] difference_type operator-(const iterator& rhs) const { return this->ptr_ - rhs.ptr_; }
		[[nodiscard]] bool operator==(const iterator& rhs) const { return this->ptr_ == rhs.ptr_; }
		[[nodiscard]] bool operator!=(const iterator& rhs) const { return this->ptr_ != rhs.ptr_; }
		[[nodiscard]] bool operator<(const iterator& rhs) const { return this->ptr_ < rhs.ptr_; }
		[[nodiscard]] bool operator>(const iterator& rhs) const { return this->ptr_ > rhs.ptr_; }
		[[nodiscard]] bool operator>=(const iterator& rhs) const { return this->ptr_ >= rhs.ptr_; }
		[[nodiscard]] bool operator<=(const iterator& rhs) const { return this->ptr_ <= rhs.ptr_; }
	};
}
