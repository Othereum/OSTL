// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include <memory>

namespace ostl
{
	template <class>
	class function;

	template <class R, class... Args>
	class function<R(Args...)>
	{
	public:
		using result_type = R;

		function() noexcept = default;
		function(std::nullptr_t) noexcept {}
		function(const function& other);
		function(function&&) noexcept = default;
		
		template <class F>
		function(F f);

		R operator()(Args... args);

	private:
		template <class F>
		struct callable;
		struct callable_base;
		
		std::unique_ptr<callable_base> fn_;
	};

	template <class R, class... Args>
	struct function<R(Args...)>::callable_base
	{
		virtual R operator()(Args...) = 0;
		virtual ~callable_base() = default;
		virtual std::unique_ptr<callable_base> clone() const = 0;
	};
	
	template <class R, class... Args>
	template <class F>
	struct function<R(Args...)>::callable final : callable_base
	{
		explicit callable(F&& f) :f_{std::forward<F>(f)} {}
		R operator()(Args... args) override { return f_(std::forward<Args>(args)...); }
		std::unique_ptr<callable_base> clone() const override { return std::make_unique<callable>(*this); }

	private:
		F f_;
	};
	
	template <class R, class... Args>
	function<R(Args...)>::function(const function& other)
	{
		fn_ = other.fn_->clone();
	}

	template <class R, class ... Args>
	template <class F>
	function<R(Args...)>::function(F f)
	{
	}

	template <class R, class... Args>
	R function<R(Args...)>::operator()(Args... args)
	{
		return (*fn_)(std::forward<Args>(args)...);
	}
}
