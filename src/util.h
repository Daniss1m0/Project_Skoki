#pragma once

#include <stdint.h>
#include <stdio.h>

#define ASSERT(x) if (!x) printf("%s:%d %s - %s", __FILE__, __LINE__, __FUNCTION__, " nie udało się.");