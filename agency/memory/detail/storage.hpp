#pragma once

#include <agency/detail/config.hpp>
#include <agency/memory/allocator.hpp>
#include <agency/memory/allocator/detail/allocator_traits.hpp>
#include <agency/detail/shape_cast.hpp>

namespace agency
{
namespace detail
{


// storage takes an optional Shape parameter instead of assuming size_t
// so that multidimensional containers need not store their shape in
// addition to what is maintained by storage
template<class T, class Allocator, class Shape = std::size_t>
class storage
{
  public:
    using value_type = T;
    using shape_type = Shape;

    __agency_exec_check_disable__
    __AGENCY_ANNOTATION
    storage(shape_type shape, const Allocator& allocator)
      : data_(nullptr),
        shape_(shape),
        allocator_(allocator)
    {
      if(size() > 0)
      {
        data_ = detail::allocator_traits<Allocator>::allocate(allocator_, size());
        if(data_ == nullptr)
        {
          detail::throw_bad_alloc();
        }
      }
    }

    __agency_exec_check_disable__
    __AGENCY_ANNOTATION
    storage(shape_type shape, Allocator&& allocator)
      : data_(nullptr),
        shape_(shape),
        allocator_(std::move(allocator))
    {
      if(size() > 0)
      {
        data_ = detail::allocator_traits<Allocator>::allocate(allocator_, size());
        if(data_ == nullptr)
        {
          detail::throw_bad_alloc();
        }
      }
    }

    __agency_exec_check_disable__
    __AGENCY_ANNOTATION
    storage(storage&& other)
      : data_(other.data_),
        shape_(other.shape_),
        allocator_(std::move(other.allocator_))
    {
      // leave the other storage in a valid state
      other.data_ = nullptr;
      other.shape_ = shape_type{};
    }

    __AGENCY_ANNOTATION
    storage(const Allocator& allocator)
      : storage(shape_type{}, allocator)
    {}

    __agency_exec_check_disable__
    __AGENCY_ANNOTATION
    storage()
      : storage(Allocator())
    {}

    __agency_exec_check_disable__
    __AGENCY_ANNOTATION
    ~storage()
    {
      if(data() != nullptr)
      {
        detail::allocator_traits<Allocator>::deallocate(allocator_, data(), size());
      }
    }

  private:
    __agency_exec_check_disable__
    __AGENCY_ANNOTATION
    void move_assign_allocator(std::true_type, Allocator& other_allocator)
    {
      // propagate the allocator
      allocator_ = std::move(other_allocator);
    }

    __AGENCY_ANNOTATION
    void move_assign_allocator(std::false_type, Allocator&)
    {
      // do nothing
    }

  public:
    __AGENCY_ANNOTATION
    storage& operator=(storage&& other)
    {
      detail::adl_swap(data_, other.data_);
      detail::adl_swap(shape_, other.shape_);

      move_assign_allocator(typename std::allocator_traits<Allocator>::propagate_on_container_move_assignment(), other.allocator());
      return *this;
    }

    __AGENCY_ANNOTATION
    T* data()
    {
      return data_;
    }

    __AGENCY_ANNOTATION
    const T* data() const
    {
      return data_;
    }

    __AGENCY_ANNOTATION
    shape_type shape() const
    {
      return shape_;
    }

    __AGENCY_ANNOTATION
    std::size_t size() const
    {
      return agency::detail::shape_cast<std::size_t>(shape());
    }

    __AGENCY_ANNOTATION
    const Allocator& allocator() const
    {
      return allocator_;
    }

    __AGENCY_ANNOTATION
    Allocator& allocator()
    {
      return allocator_;
    }

    __AGENCY_ANNOTATION
    void swap(storage& other)
    {
      detail::adl_swap(data_, other.data_);
      detail::adl_swap(shape_, other.shape_);
      detail::adl_swap(allocator_, other.allocator_);
    }

  private:
    T* data_;
    shape_type shape_;
    Allocator allocator_;
};

} // end detail
} // end agency

