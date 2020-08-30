/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <functional>

namespace d3picko {
/**
 * @brief Executes a callback in its destructor.
 * Use to execute stuff in exception stack unwinding .
 */
class DtorCallback {
 public:
  enum class CallType {
	kALWAYS,				 // always be called in destructor
	kONLY_IN_EXC_CONTEXT,	 // only called when the destructor is called from
							 // stack unwinding
	kONLY_IN_NO_EXC_CONTEXT	 // only called when the destructor is not called
							 // from stack unwinding
  };

  DtorCallback(std::function<void(void)> const& cb, CallType type);
  ~DtorCallback() noexcept;

 private:
  void invoke() const noexcept;

  std::function<void(void)> const cb_;
  CallType const type_;
};
}  // namespace d3picko
