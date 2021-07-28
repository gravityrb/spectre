// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include <array>
#include <memory>
#include <pup.h>
#include <vector>

#include "DataStructures/DataVector.hpp"
#include "Parallel/CharmPupable.hpp"

namespace domain {
/// \ingroup ComputationalDomainGroup
/// \brief Contains functions of time to support the dual frame system.
namespace FunctionsOfTime {
/// \ingroup ComputationalDomainGroup
/// \brief Base class for FunctionsOfTime
///
/// A FunctionOfTime is a function that will return the same value for
/// a time `t`, regardless of when that function is called during a run
/// (provided that the time `t` is in the domain of validity of the function).
/// All FunctionsOfTime have members
///   - `func`, that returns a `std::array<DataVector, 1>`
///   - `func_and_deriv`, that returns a `std::array<DataVector, 2>`
///   - `func_and_2_derivs`, that returns a `std::array<DataVector, 3>`
///
/// The DataVectors that are returned can be of any size: e.g. a scalar
/// FunctionOfTime will have DataVectors with one component and a 3-vector
/// FunctionOfTime will have DataVectors with three components.
///
/// The domain of validity of the function is given by the `time_bounds` member
/// function.
class FunctionOfTime : public PUP::able {
 public:
  FunctionOfTime() = default;
  FunctionOfTime(FunctionOfTime&&) noexcept = default;
  FunctionOfTime& operator=(FunctionOfTime&&) noexcept = default;
  FunctionOfTime(const FunctionOfTime&) = default;
  FunctionOfTime& operator=(const FunctionOfTime&) = default;
  ~FunctionOfTime() override = default;

  virtual auto get_clone() const noexcept
      -> std::unique_ptr<FunctionOfTime> = 0;

  /// Returns the domain of validity of the function.
  /// For FunctionsOfTime that allow a small amount of time extrapolation,
  /// `time_bounds` tells you the bounds including the allowed extrapolation
  /// interval.
  virtual std::array<double, 2> time_bounds() const noexcept = 0;

  /// The DataVector can be of any size
  virtual std::array<DataVector, 1> func(double t) const noexcept = 0;
  /// The DataVector can be of any size
  virtual std::array<DataVector, 2> func_and_deriv(double t) const noexcept = 0;
  /// The DataVector can be of any size
  virtual std::array<DataVector, 3> func_and_2_derivs(double t) const
      noexcept = 0;

  WRAPPED_PUPable_abstract(FunctionOfTime);  // NOLINT
};
}  // namespace FunctionsOfTime
}  // namespace domain
