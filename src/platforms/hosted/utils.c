/*
 * This file is part of the Black Magic Debug project.
 *
 * Copyright (C) 2020 Uwe Bonnes (bon@elektron.ikp.physik.tu-darmstadt.de)
 * Base on code:
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 * and others.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This file deduplicates codes used in several pc-hosted platforms
 */

#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "general.h"
#include "timing.h"
#include "bmp_hosted.h"

#if defined(_WIN32) && !defined(__MINGW32__)
#warning "This vasprintf() is dubious!"

int vasprintf(char **strp, const char *fmt, va_list ap)
{
	int size = 128, ret = 0;

	*strp = malloc(size);
	while (*strp && ((ret = vsnprintf(*strp, size, fmt, ap)) == size))
		*strp = realloc(*strp, size <<= 1);

	return ret;
}
#endif

void platform_delay(uint32_t ms)
{
#if defined(_WIN32) && !defined(__MINGW32__)
	Sleep(ms);
#else
#if !defined(usleep)
	int usleep(unsigned int);
#endif
	usleep(ms * 1000);
#endif
}

uint32_t platform_time_ms(void)
{
	timeval_s tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

bool begins_with(const char *const str, const size_t str_length, const char *const value)
{
	const size_t value_length = strlen(value);
	if (str_length < value_length)
		return false;
	return memcmp(str, value, value_length) == 0;
}

bool ends_with(const char *const str, const size_t str_length, const char *const value)
{
	const size_t value_length = strlen(value);
	if (str_length < value_length)
		return false;
	const size_t offset = str_length - value_length;
	return memcmp(str + offset, value, value_length) == 0;
}

bool constains_substring(const char *const str, const size_t str_len, const char *const search)
{
	const size_t search_len = strlen(search);
	if (str_len < search_len)
		return false;
	/* For each possible valid offset */
	for (size_t offset = 0; offset <= str_len - search_len; ++offset) {
		/* If we have a match, we're done */
		if (memcmp(str + offset, search, search_len) == 0)
			return true;
	}
	/* We failed to find a match */
	return false;
}
