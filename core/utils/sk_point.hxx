#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

struct SK_Point {
	std::variant<int, float> x;
	std::variant<int, float> y;
};

END_SK_NAMESPACE