/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Main/dtor_callback.h"

#include <QtGlobal>
#include <exception>

namespace d3picko {
DtorCallback::DtorCallback(const std::function<void()>& cb, CallType type)
	: cb_(cb), type_(type) {}

DtorCallback::~DtorCallback() noexcept {
  switch (type_) {
	case CallType::kALWAYS:
	  invoke();
	  break;
	case CallType::kONLY_IN_EXC_CONTEXT:
	  if (std::uncaught_exception()) invoke();
	  break;
	case CallType::kONLY_IN_NO_EXC_CONTEXT:
	  if (!std::uncaught_exception()) invoke();
	  break;
	default:
	  Q_UNREACHABLE();
  }
}

void DtorCallback::invoke() const noexcept {
  try {
	cb_();
  } catch (...) {
  }
}
}  // namespace d3picko
