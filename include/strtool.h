#ifndef __STR_TOOL_H
#define __STR_TOOL_H

#include <stddef.h>

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif /* ARRAY_SIZE */

/**
 * @brief randomize string
 * @param [out] s output randomized string
 * @param [in] len length of @p s buffer
 */
void strrand(char* s, size_t len);

#endif /* __STR_TOOL_H */
