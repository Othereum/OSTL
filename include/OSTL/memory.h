#pragma once
#include <atomic>
#include <memory>
#include "internal/compressed_pair.h"

namespace ostl
{
	namespace internal
	{
		struct SharedObjBase
		{
			virtual ~SharedObjBase() = default;
			SharedObjBase(const SharedObjBase&) = delete;
			SharedObjBase(SharedObjBase&&) = delete;
			SharedObjBase& operator=(const SharedObjBase&) = delete;
			SharedObjBase& operator=(SharedObjBase&&) = delete;

			void IncStrong() noexcept { ++strong; }
			void IncWeak() noexcept { ++weak; }
			void DecStrong() noexcept
			{
				if (--strong == 0)
				{
					Destroy();
					if (weak == 0) DeleteSelf();
				}
			}
			void DecWeak() noexcept { if (--weak == 0 && strong == 0) DeleteSelf();  }

		private:
			virtual void Destroy() noexcept = 0;
			virtual void DeleteSelf() noexcept = 0;

			std::atomic_ulong strong = 1;
			std::atomic_ulong weak = 0;
		};

		template <class T, class Al>
		struct SharedObjInline final : SharedObjBase
		{
			template <class... Args>
			constexpr explicit SharedObjInline(Al ax, Args&&... args):
				SharedObjBase<T>{},
				pair{OneThen{}, std::move(ax), std::forward<Args>(args)...}
			{
			}
			
			~SharedObjInline() { pair.first.~T(); }

			SharedObjInline(const SharedObjInline&) = delete;
			SharedObjInline(SharedObjInline&&) = delete;
			SharedObjInline& operator=(const SharedObjInline&) = delete;
			SharedObjInline& operator=(SharedObjInline&&) = delete;
			
		private:
			void Destroy() noexcept override
			{
				std::allocator_traits<Al>::destroy(pair.first, &pair.second);
			}
			
			void DeleteSelf() noexcept override
			{
				using Alloc = typename std::allocator_traits<Al>::template rebind_alloc<SharedObjInline>;
				Alloc ax{std::move(pair.first)};
				std::allocator_traits<Alloc>::destroy(ax, this);
				std::allocator_traits<Alloc>::deallocate(ax, this, 1);
			}
			
			union { compressed_pair<Al, T> pair; };
		};

		template <class T, class Dx, class Al>
		struct SharedObjPtr final : SharedObjBase
		{
			SharedObjPtr(T* ptr, Dx dt, Al ax):
				SharedObjBase<T>{},
				pair{OneThen{}, std::move(ax), OneThen{}, std::move(dt), ptr}
			{
			}
			
		private:
			void Destroy() noexcept override
			{
				auto& [deleter, ptr] = pair.second;
				if (ptr) deleter(ptr);
			}
			
			void DeleteSelf() noexcept override
			{
				using Alloc = typename std::allocator_traits<Al>::template rebind_alloc<SharedObjPtr>;
				using Traits = std::allocator_traits<Alloc>;
				Alloc ax{std::move(pair.first)};
				Traits::destroy(ax, this);
				Traits::deallocate(ax, this, 1);
			}

			compressed_pair<Al, compressed_pair<Dx, T*>> pair;
		};

		template <class T>
		class BasePtr
		{
		protected:
			constexpr BasePtr() noexcept = default;
			constexpr BasePtr(nullptr_t) noexcept {}
			
			explicit constexpr BasePtr(SharedObjBase* ptr) noexcept: ptr_{ptr} {}
			SharedObjBase* ptr_ = nullptr;
		};
	}

	template <class T>
	class weak_ptr;

	template <class T>
	class shared_ptr : internal::BasePtr<T>
	{
		using Base = internal::BasePtr<T>;
		
	public:
		using element_type = std::remove_extent_t<T>;
		using weak_type = weak_ptr<T>;
		
		constexpr shared_ptr() noexcept = default;
		constexpr shared_ptr(nullptr_t) noexcept {}

		template <class Y>
		explicit shared_ptr(Y* ptr)
			:shared_ptr{ptr, std::default_delete<Y>{}}
		{
		}

		template <class Y, class Deleter, class Alloc = std::allocator<Y>>
		shared_ptr(Y* ptr, Deleter deleter, Alloc alloc = {})
		{
			using Al = typename std::allocator_traits<Alloc>::template rebind_alloc<internal::SharedObjPtr<Y, Deleter, Alloc>>;
			using Tr = std::allocator_traits<Al>;
			Al ax{alloc};
			this->ptr_ = Tr::allocate(ax, 1);
			Tr::construct(ax, this->ptr_, ptr, std::move(deleter), std::move(alloc));
		}

		template <class Deleter, class Alloc = std::allocator<T>>
		shared_ptr(nullptr_t ptr, Deleter deleter, Alloc alloc = {})
			:shared_ptr{static_cast<T*>(nullptr), std::move(deleter), std::move(alloc)}
		{
		}
	};
}
