// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef COYOTE_RANDOM_STRATEGY_H
#define COYOTE_RANDOM_STRATEGY_H

#include "random.h"
#include "../operations/operations.h"

#define ENABLE_PROBABILISTIC_RANDOM

namespace coyote
{
	class RandomStrategy
	{
	private:
		// The pseudo-random generator.
		Random generator;

		// The seed used by the current iteration.
		size_t iteration_seed;

#ifdef ENABLE_PROBABILISTIC_RANDOM
		// ID of the currently scheduled operation
		size_t current_operation_id;
#endif

	public:
		RandomStrategy(size_t seed) noexcept;

		RandomStrategy(RandomStrategy&& strategy) = delete;
		RandomStrategy(RandomStrategy const&) = delete;

		RandomStrategy& operator=(RandomStrategy&& strategy) = delete;
		RandomStrategy& operator=(RandomStrategy const&) = delete;

		// Returns the next operation.
		size_t next_operation(Operations& operations);

		// Returns the next boolean choice.
		bool next_boolean();

		// Returns the next integer choice.
		size_t next_integer(size_t max_value);

		// Returns the seed used in the current iteration.
		size_t seed();

		// Prepares the next iteration.
		void prepare_next_iteration();
	};
}

#endif // COYOTE_RANDOM_STRATEGY_H
