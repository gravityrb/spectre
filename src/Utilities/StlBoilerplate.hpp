// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include "Utilities/ErrorHandling/Assert.hpp"
#include "Utilities/Gsl.hpp"
#include "Utilities/MakeString.hpp"

/// Classes to generate repetitive code needed for STL compatibility.
namespace stl_boilerplate {
/// Base class to generate methods for a random-access iterator.  This
/// class takes the derived class and the (optionally `const`)
/// `value_type` of the iterator.  The exposed `value_type` alias will
/// always be non-const, as required by the standard, but the template
/// parameter's constness will affect the `reference` and `pointer`
/// aliases.
///
/// The derived class must implement `operator*`, `operator+=`,
/// `operator-` (of two iterators), and `operator==`.
///
/// \snippet Test_StlBoilerplate.cpp RandomAccessIterator
///
/// This class is inspired by Boost's `iterator_facade`, except that
/// that has a lot of special cases, some of which work poorly because
/// they predate C++11.
template <typename Iter, typename ValueType>
class RandomAccessIterator {
 protected:
  RandomAccessIterator() = default;
  RandomAccessIterator(const RandomAccessIterator&) = default;
  RandomAccessIterator(RandomAccessIterator&&) = default;
  RandomAccessIterator& operator=(const RandomAccessIterator&) = default;
  RandomAccessIterator& operator=(RandomAccessIterator&&) = default;
  ~RandomAccessIterator() = default;

 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = std::remove_const_t<ValueType>;
  using reference = ValueType&;
  using pointer = ValueType*;
  using difference_type = std::ptrdiff_t;

  pointer operator->() const { return &**dthis(); }

  Iter& operator++() { return *dthis() += 1; }
  Iter operator++(int) {
    const auto ret = *dthis();
    ++*dthis();
    return ret;
  }
  Iter& operator--() { return *dthis() += -1; }
  Iter operator--(int) {
    const auto ret = *dthis();
    --*dthis();
    return ret;
  }

  Iter& operator-=(const difference_type n) { return *dthis() += -n; }

  reference operator[](const difference_type n) const {
    auto temp = *dthis();
    temp += n;
    return *temp;
  }

 private:
  // derived this
  Iter* dthis() { return static_cast<Iter*>(this); }
  const Iter* dthis() const { return static_cast<const Iter*>(this); }
};

template <typename Iter, typename ValueType>
bool operator!=(const RandomAccessIterator<Iter, ValueType>& a,
                const RandomAccessIterator<Iter, ValueType>& b) {
  return not(static_cast<const Iter&>(a) == static_cast<const Iter&>(b));
}

template <typename Iter, typename ValueType>
bool operator<(const RandomAccessIterator<Iter, ValueType>& a,
               const RandomAccessIterator<Iter, ValueType>& b) {
  return static_cast<const Iter&>(b) - static_cast<const Iter&>(a) > 0;
}

template <typename Iter, typename ValueType>
bool operator>(const RandomAccessIterator<Iter, ValueType>& a,
               const RandomAccessIterator<Iter, ValueType>& b) {
  return b < a;
}

template <typename Iter, typename ValueType>
bool operator<=(const RandomAccessIterator<Iter, ValueType>& a,
                const RandomAccessIterator<Iter, ValueType>& b) {
  return not(a > b);
}

template <typename Iter, typename ValueType>
bool operator>=(const RandomAccessIterator<Iter, ValueType>& a,
                const RandomAccessIterator<Iter, ValueType>& b) {
  return not(a < b);
}

template <typename Iter, typename ValueType>
Iter operator+(
    const RandomAccessIterator<Iter, ValueType>& a,
    const typename RandomAccessIterator<Iter, ValueType>::difference_type& n) {
  auto result = static_cast<const Iter&>(a);
  result += n;
  return result;
}

template <typename Iter, typename ValueType>
Iter operator+(
    const typename RandomAccessIterator<Iter, ValueType>::difference_type& n,
    const RandomAccessIterator<Iter, ValueType>& a) {
  return a + n;
}

template <typename Iter, typename ValueType>
Iter operator-(
    const RandomAccessIterator<Iter, ValueType>& a,
    const typename RandomAccessIterator<Iter, ValueType>::difference_type& n) {
  auto result = static_cast<const Iter&>(a);
  result -= n;
  return result;
}

/// Base class to generate methods for a random-access sequence,
/// similar to a `std::array`.  This class takes the derived class and
/// the `value_type` of the sequence as template parameters.
///
/// The derived class must implement `size` and `operator[]`.
///
/// \snippet Test_StlBoilerplate.cpp RandomAccessSequence
///
/// This class provides methods for accessing and modifying elements
/// of the sequence, such as `front` and `begin`, as well as iterators
/// and reverse iterators.  Methods modifying the sequence itself,
/// such as `insert`, require explicit implementations.
template <typename Sequence, typename ValueType>
class RandomAccessSequence {
 protected:
  RandomAccessSequence() = default;
  RandomAccessSequence(const RandomAccessSequence&) = default;
  RandomAccessSequence(RandomAccessSequence&&) = default;
  RandomAccessSequence& operator=(const RandomAccessSequence&) = default;
  RandomAccessSequence& operator=(RandomAccessSequence&&) = default;
  ~RandomAccessSequence() = default;

 public:
  using value_type = ValueType;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = ValueType*;
  using const_pointer = const ValueType*;

  class const_iterator
      : public RandomAccessIterator<const_iterator, const value_type> {
   public:
    const_iterator() = default;

    const value_type& operator*() const { return (*container_)[offset_]; }
    const_iterator& operator+=(const std::ptrdiff_t n) {
      offset_ += static_cast<size_t>(n);
      return *this;
    }

   private:
    friend bool operator==(const const_iterator& a, const const_iterator& b) {
      return a.container_ == b.container_ and a.offset_ == b.offset_;
    }

    friend std::ptrdiff_t operator-(const const_iterator& a,
                                    const const_iterator& b) {
      ASSERT(a.container_ == b.container_, "Subtracting unrelated iterators");
      return static_cast<std::ptrdiff_t>(a.offset_) -
             static_cast<std::ptrdiff_t>(b.offset_);
    }

    friend RandomAccessSequence;
    const_iterator(const gsl::not_null<const Sequence*> container,
                   const size_t offset)
        : container_(container), offset_(offset) {}

    const Sequence* container_{nullptr};
    size_t offset_{0};
  };

  class iterator : public RandomAccessIterator<iterator, value_type> {
   public:
    iterator() = default;

    value_type& operator*() const { return (*container_)[offset_]; }
    iterator& operator+=(const std::ptrdiff_t n) {
      offset_ += static_cast<size_t>(n);
      return *this;
    }

    operator const_iterator() const {
      return container_->cbegin() + static_cast<std::ptrdiff_t>(offset_);
    }

   private:
    friend bool operator==(const iterator& a, const iterator& b) {
      return a.container_ == b.container_ and a.offset_ == b.offset_;
    }

    friend std::ptrdiff_t operator-(const iterator& a, const iterator& b) {
      ASSERT(a.container_ == b.container_, "Subtracting unrelated iterators");
      return static_cast<std::ptrdiff_t>(a.offset_) -
             static_cast<std::ptrdiff_t>(b.offset_);
    }

    friend RandomAccessSequence;
    iterator(const gsl::not_null<Sequence*> container, const size_t offset)
        : container_(container), offset_(offset) {}

    Sequence* container_{nullptr};
    size_t offset_{0};
  };

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using difference_type =
      typename std::iterator_traits<const_iterator>::difference_type;
  using size_type = size_t;

  iterator begin() { return {dthis(), 0}; }
  const_iterator begin() const { return {dthis(), 0}; }
  const_iterator cbegin() const { return begin(); }
  iterator end() { return {dthis(), dthis()->size()}; }
  const_iterator end() const { return {dthis(), dthis()->size()}; }
  const_iterator cend() const { return end(); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator crbegin() const { return rbegin(); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }
  const_reverse_iterator crend() const { return rend(); }

  size_type max_size() const { return std::numeric_limits<size_type>::max(); }
  bool empty() const { return begin() == end(); }

  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }

  reference at(const size_type n) {
    if (n >= dthis()->size()) {
      throw std::out_of_range(MakeString{} << "RandomAccessSequence::at " << n
                                           << " >= " << dthis()->size());
    }
    return (*dthis())[n];
  }
  const_reference at(const size_type n) const {
    if (n >= dthis()->size()) {
      throw std::out_of_range(MakeString{} << "RandomAccessSequence::at " << n
                                           << " >= " << dthis()->size());
    }
    return (*dthis())[n];
  }

 private:
  // derived this
  Sequence* dthis() { return static_cast<Sequence*>(this); }
  const Sequence* dthis() const { return static_cast<const Sequence*>(this); }
};

template <typename Sequence, typename ValueType>
bool operator==(const RandomAccessSequence<Sequence, ValueType>& a,
                const RandomAccessSequence<Sequence, ValueType>& b) {
  return static_cast<const Sequence&>(a).size() ==
             static_cast<const Sequence&>(b).size() and
         std::equal(a.begin(), a.end(), b.begin());
}

template <typename Sequence, typename ValueType>
bool operator!=(const RandomAccessSequence<Sequence, ValueType>& a,
                const RandomAccessSequence<Sequence, ValueType>& b) {
  return not(a == b);
}

template <typename Sequence, typename ValueType>
bool operator<(const RandomAccessSequence<Sequence, ValueType>& a,
               const RandomAccessSequence<Sequence, ValueType>& b) {
  return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template <typename Sequence, typename ValueType>
bool operator>(const RandomAccessSequence<Sequence, ValueType>& a,
               const RandomAccessSequence<Sequence, ValueType>& b) {
  return b < a;
}

template <typename Sequence, typename ValueType>
bool operator<=(const RandomAccessSequence<Sequence, ValueType>& a,
                const RandomAccessSequence<Sequence, ValueType>& b) {
  return not(a > b);
}

template <typename Sequence, typename ValueType>
bool operator>=(const RandomAccessSequence<Sequence, ValueType>& a,
                const RandomAccessSequence<Sequence, ValueType>& b) {
  return not(a < b);
}
}  // namespace stl_boilerplate
