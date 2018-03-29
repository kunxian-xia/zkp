#pragma once

#define WIN32_LEAN_AND_MEAN  // Prevent conflicts in Windows.h and WinSock.h
#pragma warning( disable : LIMBS005 )	// Ignore warning about intsafe.h macro redefinitions

// Uncomment to compress EC points before writing them to an archive
#define CONDENSE_ARCHIVES

#ifdef _DEBUG
// Uncomment this line to check for memory leaks using VS.NET
//#define MEM_LEAK

// Uncomment this line to use a small identity encoding field
//#define DEBUG_ENCODING

// Uncomment this line to use sequential, rather than random, field elements
//#define DEBUG_RANDOM

// Uncomment to skip the strengthening step.  Legit evals should still pass.
//#define DEBUG_STRENGTHEN

// Perform extra checks on elliptic curve operations
//#define DEBUG_EC

// Compare results between the PV and the DV case
//#define DEBUG_PV_VS_DV

//#define DEBUG_FAST_DV_IO

#define VERBOSE

#endif


// Revert to the old version of our QAP protocol (from the EuroCrypt paper)
//#define USE_OLD_PROTO 

#ifdef USE_OLD_PROTO
// If defined, pushes more of the key generation work off to the worker.
//#define PUSH_EXTRA_WORK_TO_WORKER
#ifdef PUSH_EXTRA_WORK_TO_WORKER
// Enable the option below to have the worker invest a large effort to precompute polynomials the verifier used to supply.  
// Experiments suggest it's unlikely to be a win
//#define PRE_COMP_POLYS 
#endif // PUSH_EXTRA_WORK_TO_WORKER
#endif // USE_OLD_PROTO

// Ensure consistency by checking all IO at polys that are non-zero on both V and W
#define CHECK_NON_ZERO_IO 1

// Use Enigma's multi-mul routine instead of our new, memory-intense version
//#define OLD_MULTI_EXP

// Hunting for memory leaks
#ifdef MEM_LEAK
#define CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// Print file names for the locations of memory leaks
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#define new DEBUG_NEW
#endif
// Done with code for hunting memory leaks


// Microsoft's bignum library expects NDEBUG to be defined (or badness occurs!)
#ifndef NDEBUG
  #define UNDEFINENDEBUG
  #define NDEBUG
#endif
//#include "pfc_bn.h" //is it short for pairing-friendly curves - BN curve?
//#include "modfft.h"
#ifdef UNDEFINENDEBUG
  #undef NDEBUG
  #undef UNDEFINENDEBUG
#endif

#include <stdint.h>
#include <libff/algebra/fields/bigint.hpp>

#ifndef DEBUG_ENCODING
#define LIMBS 4
typedef libff::bigint<LIMBS> FieldElt;
#else // DEBUG_ENCODING
#define LIMBS 1
typedef libff::bigint<LIMBS> FieldElt;
#endif // DEBUG_ENCODING

typedef struct {
  libff::bigint<LIMBS> modulus;
  libff::bigint<LIMBS> one;
  libff::bigint<LIMBS> zero;
  
  size_t num_bits;
  libff::bigint<LIMBS> euler;                         //(modulus-1)/2
  size_t s;                                       // modulus = 2^s * t + 1
  libff::bigint<LIMBS> t;                             // with t odd
  libff::bigint<LIMBS> t_minus_1_over_2;              // (t-1)/2
  mp_limb_t inv;                                  // modulus^(-1) mod W, where W = 2^(word size)
  libff::bigint<LIMBS> Rsquared;                      // R^2, where R = W^k, where k = ??
} field_desc_tc;
// When we recurse through the circuit, 
// keep track of root modifiers that result 
// from constant addition or mult gates
typedef struct {
	FieldElt add;
	FieldElt mul;
} Modifier;

//#include "timer.h"

//extern TimerList timers;