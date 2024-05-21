#pragma once

#include <pointer/pointer.hpp>

namespace vt {
    CPointer GetMethod(CPointer base, uint32_t index);

    template <typename T, typename... Args>
    inline T CallMethod(CPointer base, uint32_t index, Args... args) {
        return GetMethod(base, index).Call<T (*)(void*, Args...)>(base.Get<void*>(), std::forward<Args>(args)...);
    }
}  // namespace vt

#define VIRTUAL_METHOD(returnType, name, index, args, ...) \
	returnType name args { \
		return vt::CallMethod<returnType>(this, index, __VA_ARGS__); \
	}
