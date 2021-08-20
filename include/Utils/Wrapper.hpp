#pragma once

#include "custom-types/shared/macros.hpp"
#include <type_traits>

// C# wrapper type for any pointer
DECLARE_CLASS_CODEGEN(PinkCore, Wrapper, Il2CppObject,

	public:
		template<typename T>
		void set_data(T data);
		template<typename T>
		T get_data();

	private:
		void* data;
)

template<typename T>
T PinkCore::Wrapper::get_data()
{
	static_assert(std::is_pointer_v<T>);
	return (T)data;
}

template <typename T>
void PinkCore::Wrapper::set_data(T data)
{
	static_assert(std::is_pointer_v<T>);
	this->data = (void*)data;
}