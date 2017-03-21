/*
 * Copyright(c) 2012 Tim Ruehsen
 * Copyright(c) 2016 Free Software Foundation, Inc.
 *
 * This file is part of libwget.
 *
 * Libwget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Libwget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libwget.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * a multi-thread safe wrapper around gnulib random_r()
 *
 * Changelog
 * 22.01.2016  Tim Ruehsen  created
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#ifdef _WIN32
#  include <process.h>  /* getpid() */
#endif

#include <wget.h>
#include "private.h"

/**
 * \file
 * \brief Random functions
 * \defgroup libwget-random Random functions
 * @{
 *
 * This is wrapper code around gnulib's srandom_r() and random_r() with automatic seeding
 */

static wget_thread_mutex_t mutex = WGET_THREAD_MUTEX_INITIALIZER;
static int seeded;
static char statebuf[64];
static struct random_data state;

/**
 * \return Random value between 0 and RAND_MAX
 *
 * This functions wraps around gnulib's random_r(). It performs a thread-safe seeding on the first use,
 * if not done before by wget_srandom();
 */
int wget_random(void)
{
	int32_t r;

	wget_thread_mutex_lock(&mutex);

	if (!seeded) {
		// seed random generator, used e.g. by Digest Authentication and --random-wait
		initstate_r((unsigned)(time(NULL) ^ getpid()), statebuf, sizeof(statebuf), &state);
		seeded = 1;
	}

	if (random_r(&state, &r))
		r = 0; // return 0 on failure

	wget_thread_mutex_unlock(&mutex);

	return (int)r;
}

/**
 * \param[in] seed Value to seed the random generator
 *
 * This functions wraps around srandom_r() to make a thread-safe seeding for wget_random().
 */
void wget_srandom(unsigned int seed)
{
	wget_thread_mutex_lock(&mutex);

	initstate_r(seed, statebuf, sizeof(statebuf), &state);
	seeded = 1;

	wget_thread_mutex_unlock(&mutex);
}

/**@}*/
