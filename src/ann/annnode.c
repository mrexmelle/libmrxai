
#include "annnode.h"

void annnode_activate(SAnnNode * aNode)
{
	aNode->iOutputSignal=aNode->iActivationFunction(aNode->iInputSignal);
}

void annnode_backward_activate(SAnnNode * aNode)
{
	aNode->iOutputError=aNode->iOutputError*aNode->iDerivatedActivationFunction(aNode->iOutputSignal);
}

