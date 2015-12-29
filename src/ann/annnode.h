
#ifndef __S_ANN_NODE_H__
#define __S_ANN_NODE_H__

#pragma pack(push, 1)

typedef struct
{
	double iInputSignal;
	double iOutputSignal;
	double(*iActivationFunction)(double);

	double iInputError;
	double iOutputError;
	double(*iDerivatedActivationFunction)(double);
} SAnnNode;

#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern void annnode_activate(SAnnNode * aNode);
extern void annnode_backward_activate(SAnnNode * aNode);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __S_ANN_NODE_H__

