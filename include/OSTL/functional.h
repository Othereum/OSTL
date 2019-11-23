// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include <memory>
#include "cstddef.h"

namespace ostl
{
	template <class>
	class function;

	template <class R, class... Args>
	class function<R(Args ...)>
	{
		template <class F>
		using enable_if_callable = std::enable_if_t<std::is_convertible_v<decltype(std::declval<F>()()), R>>;

	public:
		using result_type = R;

		function() noexcept = default;

		function(nullptr_t) noexcept
		{
		}

		function(const function& other);
		function(function&&) noexcept = default;

		template <class F, class = enable_if_callable<F>>
		function(F f);

		~function() = default;

		function& operator=(const function& other)
		{
			function{other}.swap(*this);
			return *this;
		}

		function& operator=(function&&) = default;

		function& operator=(nullptr_t) noexcept
		{
			f_.release();
			return *this;
		}

		template <class F, class = enable_if_callable<F>>
		function& operator=(F&& f)
		{
			function{std::forward<F>(f)}.swap(*this);
			return *this;
		}

		template <class F>
		function& operator=(std::reference_wrapper<F> f) noexcept
		{
			function{f}.swap(*this);
			return *this;
		}

		void swap(function& other) noexcept { f_.swap(other.f_); }

		explicit operator bool() const noexcept { return !!f_; }

		R operator()(Args ... args) const;

		[[nodiscard]] const std::type_info& target_type() const noexcept
		{
			return !!*this ? typeid(*f_) : typeid(void);
		}

		template <class T>
		T* target() noexcept { return dynamic_cast<T*>(f_.get()); }

		template <class T>
		const T* target() const noexcept { return dynamic_cast<const T*>(f_.get()); }

	private:
		template <class F>
		struct callable;
		struct callable_base;

		std::unique_ptr<callable_base> f_;
	};

	template <class R, class... Args>
	void swap(function<R(Args ...)>& lhs, function<R(Args ...)>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	template <class R, class... Args>
	bool operator==(const function<R(Args...)>& f, nullptr_t) noexcept { return !f; }

	template <class R, class... Args>
	bool operator==(nullptr_t, const function<R(Args...)>& f) noexcept { return !f; }

	template <class R, class... Args>
	bool operator!=(const function<R(Args...)>& f, nullptr_t) noexcept { return !!f; }

	template <class R, class... Args>
	bool operator!=(nullptr_t, const function<R(Args...)>& f) noexcept { return !!f; }

	class bad_function_call : public std::exception
	{
		bad_function_call() noexcept
		{
		}

		[[nodiscard]] char const* what() const noexcept override { return "bad function call"; }
	};

	template <class R, class... Args>
	struct function<R(Args ...)>::callable_base
	{
		virtual R operator()(Args ...) = 0;
		virtual ~callable_base() = default;
		virtual std::unique_ptr<callable_base> clone() const = 0;
	};

	template <class R, class... Args>
	template <class F>
	struct function<R(Args ...)>::callable final : callable_base
	{
		explicit callable(F&& f) : f_{std::forward<F>(f)}
		{
		}

		R operator()(Args ... args) override { return f_(std::forward<Args>(args)...); }
		std::unique_ptr<callable_base> clone() const override { return std::make_unique<callable>(*this); }

	private:
		F f_;
	};

	template <class R, class... Args>
	function<R(Args ...)>::function(const function& other)
	{
		f_ = other.f_->clone();
	}

	template <class R, class... Args>
	template <class F, class>
	function<R(Args ...)>::function(F f)
		: f_{std::make_unique<callable<F>>(std::move(f))}
	{
	}

	template <class R, class... Args>
	R function<R(Args ...)>::operator()(Args ... args) const
	{
		if (!*this) throw bad_function_call{};
		return (*f_)(std::forward<Args>(args)...);
	}
}
