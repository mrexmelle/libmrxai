
#ifndef __S_ANN_LAYER_H__
#define __S_ANN_LAYER_H__

#include "annnode.h"

#pragma pack(push, 1)

typedef struct
{
	SAnnNode * iNodes;
	unsigned short iNodeCount;
} SAnnLayer;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#pragma pack(pop)

extern void annlayer_create(SAnnLayer * aAnnLayer, unsigned short aNodeCount);
extern void annlayer_free(SAnnLayer * aAnnLayer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __S_ANN_LAYER_H__

