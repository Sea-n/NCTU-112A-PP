#include "PPintrin.h"
/* Author: Sean Wei <https://sean.cat/> */

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N) {
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH) {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }

  _pp_vstore_float(output + N, zero, maskAll);
}

void clampedExpVector(float *values, int *exponents, float *output, int N) {
  //
  // PP STUDENTS: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //

  __pp_vec_float resF, valF, maxF, zeroF;
  __pp_vec_int zeroI, oneI, expI;
  __pp_mask maskAll, maskExp, maskMax;

  maskAll = _pp_init_ones();  // maskAll = [* * * *]
  _pp_vset_int(oneI, 1, maskAll);   // oneI  = [1 1 1 1]
  _pp_vset_int(zeroI, 0, maskAll);  // zeroI = [0 0 0 0]
  _pp_vset_float(zeroF, 0.0f, maskAll);  // zeroF = [0.0f 0.0f]
  _pp_vset_float(maxF, 9.999999f, maskAll);

  for (int i = 0; i < N; i += VECTOR_WIDTH) {
    _pp_vload_int(expI, exponents + i, maskAll);  // expI = exponents
    _pp_vload_float(valF, values + i, maskAll);   // valF = values
    _pp_vset_float(resF, 1.0, maskAll);           // resF = 1.0

    do {
      _pp_vgt_int(maskExp, expI, zeroI, maskAll);  // if (expI > 0)
      _pp_vsub_int(expI, expI, oneI, maskExp);     //   expI -= 1
      _pp_vmult_float(resF, resF, valF, maskExp);  //   resF *= valF
    } while (_pp_cntbits(maskExp));

    _pp_vgt_float(maskMax, resF, maxF, maskAll);  // if (resF > 9.99)
    _pp_vmove_float(resF, maxF, maskMax);         //   resF := maxF
    _pp_vstore_float(output + i, resF, maskAll);  // output[i] = resF
  }

  _pp_vstore_float(output + N, zeroF, maskAll);
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N) {

  //
  // PP STUDENTS: Implement your vectorized version of arraySumSerial here
  //

  __pp_vec_float sumF, valF;
  __pp_mask maskAll;

  maskAll = _pp_init_ones();  // maskAll = [* * * *]
  _pp_vset_float(sumF, 0.0f, maskAll);

  for (int i = 0; i < N; i += VECTOR_WIDTH) {
    _pp_vload_float(valF, values + i, maskAll);  // valF = values
    _pp_vadd_float(sumF, sumF, valF, maskAll);   // sumF += valF
  }

  float sum[VECTOR_WIDTH];
  for (int i=2; i<VECTOR_WIDTH; i*=2) {
    _pp_hadd_float(sumF, sumF);  // [AB AB CD CD]
    _pp_interleave_float(sumF, sumF);  // [AB CD AB CD]
  }
  _pp_hadd_float(sumF, sumF);  // [ABCD * 4]
  _pp_vstore_float(sum, sumF, maskAll);

  return sum[0];
}
