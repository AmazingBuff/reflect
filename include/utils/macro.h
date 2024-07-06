#pragma once

#define IDENTITY(N) N
#define APPLY(macro, ...) IDENTITY(macro(__VA_ARGS__))
// for further use
// only support 10 arguments for now
#define COUNT_N(_1, _2, _3, _4 ,_5, _6, _7, _8, _9, _10, N, ...) N 
#define COUNT(...) APPLY(COUNT_N, __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define CONCATENATE_0() 
#define CONCATENATE_1(A) A
#define CONCATENATE_2(A, B) A##_##B
#define CONCATENATE_3(A, B, C) A##_##B##_##C
#define CONCATENATE_4(A, B, C, D) A##_##B##_##C##_##D
#define CONCATENATE_5(A, B, C, D, E) A##_##B##_##C##_##D##_##E
#define CONCATENATE_6(A, B, C, D, E, F) A##_##B##_##C##_##D##_##E##_##F
#define CONCATENATE_7(A, B, C, D, E, F, G) A##_##B##_##C##_##D##_##E##_##F##_##G
#define CONCATENATE_8(A, B, C, D, E, F, G, H) A##_##B##_##C##_##D##_##E##_##F##_##G##_##H
#define CONCATENATE_9(A, B, C, D, E, F, G, H, I) A##_##B##_##C##_##D##_##E##_##F##_##G##_##H##_##I
#define CONCATENATE_10(A, B, C, D, E, F, G, H, I, J) A##_##B##_##C##_##D##_##E##_##F##_##G##_##H##_##I##_##J

#define CONCATENATE_NUMBER(N) CONCATENATE_##N
#define CONCATENATE(...) APPLY(APPLY(CONCATENATE_NUMBER, COUNT(__VA_ARGS__)), __VA_ARGS__)