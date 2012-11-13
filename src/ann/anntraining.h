
#ifndef __ANN_TRAINING_H__
#define __ANN_TRAINING_H__

#include "annarch.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern void anntraining_forward_propagate(SAnnArch * aArch);
extern void anntraining_backward_propagate(SAnnArch * aArch);
extern double anntraining_count_total_error(SAnnArch * aArch);
extern void anntraining_update_weights_and_biases(SAnnArch * aArch);
extern void anntraining_reset_weights_and_biases_delta(SAnnArch * aArch);
extern unsigned long long anntraining_train_single_pattern(SAnnArch * aArch, double * aInputSet, double * aOutputSet);
extern unsigned long long anntraining_train_multiple_pattern(SAnnArch * aArch, double * aInputSet, double * aOutputSet, unsigned short aSetCount);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ANN_TRAINING_H__

