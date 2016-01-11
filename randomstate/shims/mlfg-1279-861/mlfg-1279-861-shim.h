#ifdef _WIN32
#include "../../src/common/stdint.h"
#define inline __inline
#else
#include <stdint.h>
#endif

#include "../../src/common/entropy.h"
#include "../../src/common/binomial.h"
#include "../../src/mlfg-1279-861/mlfg-1279-861.h"

#define BITMASK_UPPER 0xffff0000
#define BITMAST_UPPER22 0xfffffc00
#define BITMAST_UPPER21 0xfffff800

typedef struct s_aug_state {
    mlfg_state *rng;
    binomial_t *binomial;

    int has_gauss, shift_zig_random_int, has_uint32;
    double gauss;
    uint32_t uinteger;
    uint64_t zig_random_int;
} aug_state;

inline uint32_t random_uint32(aug_state* state)
{
    // Two are needed since there is only 31 bits in each
#if defined(_WIN32) && !defined(_WIN64)
   uint32_t a, b;
    a = mlfg_next(state->rng) & BITMASK_UPPER;
    b = mlfg_next(state->rng) >> 16;
    return a | b;
#else
    return (mlfg_next(state->rng) & BITMASK_UPPER) | (mlfg_next(state->rng) >> 16);
#endif
}

inline uint64_t random_uint64(aug_state* state)
{

    uint64_t out = ((uint64_t)(mlfg_next(state->rng)) & BITMAST_UPPER22) << 32;
    out |= ((uint64_t)(mlfg_next(state->rng)) & BITMAST_UPPER21) << 10;
    out |= ((mlfg_next(state->rng)) & BITMAST_UPPER21) >> 11;
    // Three are needed since there is only 31 bits in each
    return out;
}

inline void set_seed(aug_state* state, uint64_t val)
{
    mlfg_seed(state->rng, val);
}

inline void entropy_init(aug_state* state)
{
    uint64_t seeds[1];
    entropy_fill((void*) seeds, sizeof(seeds));
    mlfg_seed(state->rng, seeds[0]);
}

inline double random_double(aug_state* state)
{
    // These differ by 1 bit since there are only 31 bits in each
    int32_t a = mlfg_next(state->rng) >> 5, b = mlfg_next(state->rng) >> 6;
    return (a * 67108864.0 + b) / 9007199254740992.0;
}