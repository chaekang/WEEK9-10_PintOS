#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#include <stdint.h>

/* Scaling Factor */
#define F (1 << 14)

/* 변환 */
#define INT_TO_FP(n) ((n) * F) // int to fixed point
#define FP_TO_INT_ZERO(x) ((x) / F) // fixed point to int and discard decimals
#define FP_TO_INT_NEAREST(x) \
    ((x) >= 0 ? (((x) + F / 2) / F) : (((x) - F / 2) / F)) // rounding

/* 사칙연산 */
#define ADD_FP(x, y) ((x) + (y)) // 더하기 빼기
#define SUB_FP(x, y) ((x) - (y)) 

#define ADD_MIX(x, n) ((x) + (n) * F)
#define SUB_MIX(x, n) ((x) - (n) * F)

#define MUL_MIX(x, n) ((x) * (n)) // 곱하기
#define MUL_FP(x, y) ((int64_t) (x) * (y) / F)

#define DIV_MIX(x, n) ((x) / (n)) // 나눗셈
#define DIV_FP(x, y) ((int64_t) (x) * F / (y))

#endif