//                Copyright Jo Bates 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//     Please report any bugs, typos, or suggestions to
//         https://github.com/Cincinesh/tue/issues

#pragma once

#include <cstdint>
#include <type_traits>

namespace tue
{
	enum bool8 : std::uint8_t;
	enum bool16 : std::uint16_t;
	enum bool32 : std::uint32_t;
	enum bool64 : std::uint64_t;

	template<typename T, int N>
	class simd;

	template<typename T>
	struct is_vec_component
			:
					public std::integral_constant<bool, false>
	{
		using std::integral_constant<bool, false>::integral_constant;
	};

	template<>
	struct is_vec_component<float>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<double>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::int8_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::int16_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::int32_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::int64_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::uint8_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::uint16_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::uint32_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<std::uint64_t>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<bool8>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<bool16>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<bool32>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<>
	struct is_vec_component<bool64>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};

	template<typename T, int N>
	struct is_vec_component<simd<T, N>>
			:
					public std::integral_constant<bool, true>
	{
		using std::integral_constant<bool, true>::integral_constant;
	};
}
