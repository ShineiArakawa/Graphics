#pragma once

#ifndef _COMMON_H_
#include "common.h"
#endif

#include <vector>
#include <array>

class Environ {
public:
	virtual void reboundWithEnviron(void) = 0;
};