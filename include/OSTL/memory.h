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
			SharedObjBase() = default;
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

		template <class Ptr, class Dx, class Al>
		struct SharedObjPtr final : SharedObjBase
		{
			SharedObjPtr(Ptr ptr, Dx dt, Al ax):
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

			compressed_pair<Al, compressed_pair<Dx, Ptr>> pair;
		};

		template <class T>
		class BasePtr
		{
		protected:
			using element_type = std::remove_extent_t<T>;

			constexpr BasePtr() noexcept = default;
			constexpr BasePtr(nullptr_t) noexcept {}
			
			explicit constexpr BasePtr(SharedObjBase* obj, element_type* ptr) noexcept: obj_{obj}, ptr_{ptr} {}
			SharedObjBase* obj_ = nullptr;
			element_type* ptr_ = nullptr;
		};
	}

	template <class T>
	class weak_ptr;

	template <class T>
	class shared_ptr : internal::BasePtr<T>
	{
		using Base = internal::BasePtr<T>;
		
	public:
		using Base::element_type;
		using weak_type = weak_ptr<T>;
		
		constexpr shared_ptr() noexcept = default;
		constexpr shared_ptr(nullptr_t) noexcept {}

		template <class Y>
		explicit shared_ptr(Y* ptr)
		{
			if constexpr (std::is_array_v<T>)
			{
				Construct(ptr, std::default_delete<Y[]>{}, std::allocator<Y>{});
			}
			else
			{
				Construct(ptr, std::default_delete<Y>{}, std::allocator<Y>{});
			}
		}

		template <class Y, class Deleter, class Alloc = std::allocator<Y>>
		shared_ptr(Y* ptr, Deleter deleter, Alloc alloc = {})
		{
			Construct(ptr, std::move(deleter), std::move(alloc));
		}

		template <class Deleter, class Alloc = std::allocator<T>>
		shared_ptr(nullptr_t ptr, Deleter deleter, Alloc alloc = {})
		{
			Construct(ptr, std::move(deleter), std::move(alloc));
		}

		template <class Y>
		shared_ptr(const shared_ptr<Y>& r, element_type* ptr) noexcept
		{
			if (r.obj_) r.obj_->IncStrong();
			this->obj_ = r.obj_;
			this->ptr_ = ptr;
		}

	private:
		template <class Ptr, class Deleter, class Alloc>
		void Construct(Ptr ptr, Deleter deleter, Alloc alloc)
		{
			using Al = typename std::allocator_traits<Alloc>::template rebind_alloc<internal::SharedObjPtr<Ptr, Deleter, Alloc>>;
			using Tr = std::allocator_traits<Al>;
			Al ax{alloc};
			this->obj_ = Tr::allocate(ax, 1);
			this->ptr_ = ptr;
			Tr::construct(ax, this->obj_, ptr, std::move(deleter), std::move(alloc));
		}
	};
}
