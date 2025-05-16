#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Point {
public:
	SK_Number x;
	SK_Number y;
    
    SK_Point() : x(0), y(0) {}
    SK_Point(SK_Number _x, SK_Number _y) : x(0), y(0) {
        x = _x;
        y = _y;
    }
};

END_SK_NAMESPACE
