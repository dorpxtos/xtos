#pragma once
// The latest version of this library is available on GitHub;
// https://github.com/sheredom/utf8.h

// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
#if defined(_MSC_VER)
#pragma warning(push)

// disable 'bytes padding added after construct' warning
#pragma warning(disable : 4820)
#endif

#include <stddef.h>
#include <DeclSpec.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#if defined(_MSC_VER)
typedef __int32 utf8_int32_t;
#else
#include <stdint.h>
typedef int32_t utf8_int32_t;
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wcast-qual"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__clang__) || defined(__GNUC__)
#define utf8_nonnull __attribute__((nonnull))
#define utf8_pure __attribute__((pure))
#define utf8_restrict __restrict__
#define utf8_weak __attribute__((weak))
#elif defined(_MSC_VER)
#define utf8_nonnull
#define utf8_pure
#define utf8_restrict __restrict
#define utf8_weak __inline
#else
#error Non clang, non gcc, non MSVC compiler found!
#endif

#ifdef __cplusplus
#define utf8_null NULL
#else
#define utf8_null 0
#endif

	// Return less than 0, 0, greater than 0 if src1 < src2, src1 == src2, src1 >
	// src2 respectively, case insensitive.
	DllExport utf8_nonnull utf8_pure utf8_weak int utf8casecmp(const void *src1,
		const void *src2);

	// Append the utf8 string src onto the utf8 string dst.
	DllExport utf8_nonnull utf8_weak void *utf8cat(void *utf8_restrict dst,
		const void *utf8_restrict src);

	// Find the first match of the utf8 codepoint chr in the utf8 string src.
	DllExport utf8_nonnull utf8_pure utf8_weak void *utf8chr(const void *src,
		utf8_int32_t chr);

	// Return less than 0, 0, greater than 0 if src1 < src2,
	// src1 == src2, src1 > src2 respectively.
	DllExport utf8_nonnull utf8_pure utf8_weak int utf8cmp(const void *src1,
		const void *src2);

	// Copy the utf8 string src onto the memory allocated in dst.
	DllExport utf8_nonnull utf8_weak void *utf8cpy(void *utf8_restrict dst,
		const void *utf8_restrict src);

	// Number of utf8 codepoints in the utf8 string src that consists entirely
	// of utf8 codepoints not from the utf8 string reject.
	DllExport utf8_pure utf8_weak size_t utf8cspn(const void *src,
		const void *reject);

	// Duplicate the utf8 string src by getting its size, malloc'ing a new buffer
	// copying over the data, and returning that. Or 0 if malloc failed.
	DllExport utf8_nonnull utf8_weak void *utf8dup(const void *src);

	// Number of utf8 codepoints in the utf8 string str,
	// excluding the null terminating byte.
	DllExport utf8_nonnull utf8_pure utf8_weak size_t utf8len(const void *str);

	// Return less than 0, 0, greater than 0 if src1 < src2, src1 == src2, src1 >
	// src2 respectively, case insensitive. Checking at most n bytes of each utf8
	// string.
	DllExport utf8_nonnull utf8_pure utf8_weak int utf8ncasecmp(const void *src1,
		const void *src2, size_t n);

	// Append the utf8 string src onto the utf8 string dst,
	// writing at most n+1 bytes. Can produce an invalid utf8
	// string if n falls partway through a utf8 codepoint.
	DllExport utf8_nonnull utf8_weak void *utf8ncat(void *utf8_restrict dst,
		const void *utf8_restrict src, size_t n);

	// Return less than 0, 0, greater than 0 if src1 < src2,
	// src1 == src2, src1 > src2 respectively. Checking at most n
	// bytes of each utf8 string.
	DllExport utf8_nonnull utf8_pure utf8_weak int utf8ncmp(const void *src1,
		const void *src2, size_t n);

	// Copy the utf8 string src onto the memory allocated in dst.
	// Copies at most n bytes. If there is no terminating null byte in
	// the first n bytes of src, the string placed into dst will not be
	// null-terminated. If the size (in bytes) of src is less than n,
	// extra null terminating bytes are appended to dst such that at
	// total of n bytes are written. Can produce an invalid utf8
	// string if n falls partway through a utf8 codepoint.
	DllExport utf8_nonnull utf8_weak void *utf8ncpy(void *utf8_restrict dst,
		const void *utf8_restrict src, size_t n);

	// Similar to utf8dup, except that at most n bytes of src are copied. If src is
	// longer than n, only n bytes are copied and a null byte is added.
	//
	// Returns a new string if successful, 0 otherwise
	DllExport utf8_nonnull utf8_weak void *utf8ndup(const void *src, size_t n);

	// Locates the first occurence in the utf8 string str of any byte in the
	// utf8 string accept, or 0 if no match was found.
	DllExport utf8_nonnull utf8_pure utf8_weak void *utf8pbrk(const void *str,
		const void *accept);

	// Find the last match of the utf8 codepoint chr in the utf8 string src.
	DllExport utf8_nonnull utf8_pure utf8_weak void *utf8rchr(const void *src, int chr);

	// Number of bytes in the utf8 string str,
	// including the null terminating byte.
	DllExport utf8_nonnull utf8_pure utf8_weak size_t utf8size(const void *str);

	// Number of utf8 codepoints in the utf8 string src that consists entirely
	// of utf8 codepoints from the utf8 string accept.
	DllExport utf8_nonnull utf8_pure utf8_weak size_t utf8spn(const void *src,
		const void *accept);

	// The position of the utf8 string needle in the utf8 string haystack.
	DllExport utf8_nonnull utf8_pure utf8_weak void *utf8str(const void *haystack,
		const void *needle);

	// The position of the utf8 string needle in the utf8 string haystack, case
	// insensitive.
	DllExport utf8_nonnull utf8_pure utf8_weak void *utf8casestr(const void *haystack,
		const void *needle);

	// Return 0 on success, or the position of the invalid
	// utf8 codepoint on failure.
	DllExport utf8_nonnull utf8_pure utf8_weak void *utf8valid(const void *str);

	// Sets out_codepoint to the next utf8 codepoint in str, and returns the address
	// of the utf8 codepoint after the current one in str.
	DllExport utf8_nonnull utf8_weak void *
		utf8codepoint(const void *utf8_restrict str,
			utf8_int32_t *utf8_restrict out_codepoint);

	// Returns the size of the given codepoint in bytes.
	DllExport utf8_weak size_t utf8codepointsize(utf8_int32_t chr);

	// Write a codepoint to the given string, and return the address to the next
	// place after the written codepoint. Pass how many bytes left in the buffer to
	// n. If there is not enough space for the codepoint, this function returns
	// null.
	DllExport utf8_nonnull utf8_weak void *utf8catcodepoint(void *utf8_restrict str,
		utf8_int32_t chr, size_t n);

	// Returns 1 if the given character is lowercase, or 0 if it is not.
	DllExport utf8_weak int utf8islower(utf8_int32_t chr);

	// Returns 1 if the given character is uppercase, or 0 if it is not.
	DllExport utf8_weak int utf8isupper(utf8_int32_t chr);

	// Transform the given string into all lowercase codepoints.
	DllExport utf8_nonnull utf8_weak void utf8lwr(void *utf8_restrict str);

	// Transform the given string into all uppercase codepoints.
	DllExport utf8_nonnull utf8_weak void utf8upr(void *utf8_restrict str);

	// Make a codepoint lower case if possible.
	DllExport utf8_weak utf8_int32_t utf8lwrcodepoint(utf8_int32_t cp);

	// Make a codepoint upper case if possible.
	DllExport utf8_weak utf8_int32_t utf8uprcodepoint(utf8_int32_t cp);

#ifdef __cplusplus
}
#endif