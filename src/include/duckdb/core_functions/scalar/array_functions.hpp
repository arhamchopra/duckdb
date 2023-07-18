//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/core_functions/scalar/array_functions.hpp
//
//
//===----------------------------------------------------------------------===//
// This file is automatically generated by scripts/generate_functions.py
// Do not edit this file manually, your changes will be overwritten
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/function/function_set.hpp"

namespace duckdb {

struct ArrayValueFun {
	static constexpr const char *Name = "array_value";
	static constexpr const char *Parameters = "any,...";
	static constexpr const char *Description = "Create an ARRAY containing the argument values.";
	static constexpr const char *Example = "array_value(4, 5, 6)";

	static ScalarFunction GetFunction();
};

struct ArrayFromListFun {
	static constexpr const char *Name = "array_from_list";
	static constexpr const char *Parameters = "any,...";
	static constexpr const char *Description = "Create an ARRAY from a list, truncating too large lists.";
	static constexpr const char *Example = "array_from_list([4, 5, 6], 2)";

	static ScalarFunction GetFunction();
};

} // namespace duckdb
