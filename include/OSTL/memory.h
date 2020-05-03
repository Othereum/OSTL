#pragma once
#include <atomic>
#include <memory>
#include "internal/compressed_pair.h"

namespace ostl
{
	namespace internal
	{
		template <class T>
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
		struct SharedObjInline final : SharedObjBase<T>
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
				Alloc ax = std::move(pair.first);
				std::allocator_traits<Alloc>::destroy(ax, this);
				std::allocator_traits<Alloc>::deallocate(ax, this, 1);
			}
			
			union { compressed_pair<Al, T> pair; };
		};

		template <class T, class Dx = std::default_delete<T>, class Al = std::allocator<T>>
		struct SharedObjPtr final : SharedObjBase<T>
		{
			explicit SharedObjPtr(T* ptr, Dx dt = {}, Al ax = {}):
				SharedObjBase<T>{},
				pair{OneThen{}, std::move(ax), OneThen{}, std::move(dt), ptr}
			{
			}
			
		private:
			using Alloc = typename std::allocator_traits<Al>::template rebind_alloc<SharedObjPtr>;
			
			void Destroy() noexcept override { pair.second.first(pair.second.second); }
			void DeleteSelf() noexcept override
			{
				std::allocator_traits<Alloc>::destroy(pair.first, this);
				std::allocator_traits<Alloc>::deallocate(pair.first, this, 1);
			}

			compressed_pair<Alloc, compressed_pair<Dx, T*>> pair;
		};

		template <class T>
		class BasePtr
		{
		protected:
			constexpr BasePtr() noexcept = default;
			constexpr BasePtr(nullptr_t) noexcept {}
			
			explicit constexpr BasePtr(SharedObjBase<T>* ptr) noexcept: ptr_{ptr} {}
			SharedObjBase<T>* ptr_ = nullptr;
		};
	}

	template <class T>
	class shared_ptr : internal::BasePtr<T>
	{
		using Base = internal::BasePtr<T>;
		
	public:
		constexpr shared_ptr() noexcept = default;
		constexpr shared_ptr(nullptr_t) noexcept {}

		explicit shared_ptr(T* ptr):
			Base{new internal::SharedObjPtr<T>{ptr}}
		{
		}

		template <class Deleter, class Alloc = std::allocator<T>>
		shared_ptr(T* ptr, Deleter deleter, Alloc alloc = {}):
			Base{new internal::SharedObjPtr<T, Deleter, Alloc>{ptr, std::move(deleter), std::move(alloc)}}
		{
		}
	};
}