
#include <stdlib.h>
#include <string.h>
#include "bpnveformula.h"

int bpnveformula_create(SBpnVeFormula * aVeFormula, unsigned short aCptCount)
{
	// vars
	int i;

	// init cpt list
	aVeFormula->iCptCount=aCptCount;

	if(aCptCount>0)
	{
		aVeFormula->iCptList=(SBpnCpt**)malloc(aCptCount*sizeof(SBpnCpt*));
		for(i=0; i<aCptCount; i++)
		{
			aVeFormula->iCptList[i]=(SBpnCpt*)malloc(sizeof(SBpnCpt));
			bpncpt_create(aVeFormula->iCptList[i], NULL, 0);
		}
	}
	else
	{
		aVeFormula->iCptList=NULL;
	}

	return 0;
}

void bpnveformula_free(SBpnVeFormula * aVeFormula)
{
	// vars
	int i;
	
	// free cpt list
	if(aVeFormula->iCptList)
	{
		for(i=0; i<aVeFormula->iCptCount; i++)
		{
			bpncpt_free(aVeFormula->iCptList[i]);
			free(aVeFormula->iCptList[i]);
			aVeFormula->iCptList[i]=NULL;
		}
		free(aVeFormula->iCptList);
		aVeFormula->iCptList=NULL;
	}
}

void bpnveformula_assign_cpt(SBpnVeFormula * aVeFormula, unsigned short aCptIndex, SBpnCpt * aCpt)
{
	bpncpt_create(aVeFormula->iCptList[aCptIndex], aCpt->iIdList, aCpt->iIdCount);
	bpncpt_copy_table(aVeFormula->iCptList[aCptIndex], aCpt->iTable);
}

int bpnveformula_query(SBpnVeFormula * aVeFormula, unsigned short aQueriedId,
	unsigned short * aEvidenceList, unsigned char * aEvidenceValue, unsigned short aEvidenceCount,
	unsigned short * aSumOverSequence, unsigned short aSumOverCount, SBpnCpt * aResult)
{
	// vars
	unsigned short i, j;

	// eliminate known evidence variable in table list
	for(i=0; i<aEvidenceCount; i++)
	{
		bpnveformula__eliminate_exact_evidence(aVeFormula, aEvidenceList[i], aEvidenceValue[i]);
	}

	// eliminate known evidence variable in table list
	for(i=0; i<aSumOverCount; i++)
	{
		// ensure no evidence in the sum over list
		for(j=0; j<aEvidenceCount; j++)
		{
			if(aSumOverSequence[i]==aEvidenceList[j]) { break; }
		}

		// ensure queried id is summed over last
		if(j==aEvidenceCount && aSumOverSequence[i]!=aQueriedId)
		{
			bpnveformula__sum_over(aVeFormula, aSumOverSequence[i]);
		}
	}

	// sum over queried id
	bpnveformula__sum_over(aVeFormula, aQueriedId);
	
	// normalize table
	bpncpt_normalize_table(aVeFormula->iCptList[0], aResult);

	return 0;
}

int bpnveformula__eliminate_exact_evidence(SBpnVeFormula * aVeFormula, unsigned short aId, unsigned char aValue)
{
	// vars
	unsigned short i;
	int idPos;

	for(i=0; i<aVeFormula->iCptCount; i++)
	{
		// check id's existence in current cpt
		idPos=bpncpt_id_seq_in_list(aVeFormula->iCptList[i], aId);

		// if exists, replace current cpt
		if(idPos>=0)
		{
			// create new node
			SBpnCpt * newNode=(SBpnCpt*)malloc(sizeof(SBpnCpt));
			bpncpt_create(newNode, NULL, 0);
			bpncpt_eliminate_constant_value(aVeFormula->iCptList[i], aId, aValue, newNode);
			
			// free current cpt
			bpncpt_free(aVeFormula->iCptList[i]);
			free(aVeFormula->iCptList[i]);

			// replace with newNode
			aVeFormula->iCptList[i]=newNode;
		}
	}

	return 0;
}

int bpnveformula__sum_over(SBpnVeFormula * aVeFormula, unsigned short aId)
{
	// vars
	unsigned short i, j, k;
	int idPos1, idPos2;
	int status;

	for(i=0; i<aVeFormula->iCptCount-1; i++)
	{
		idPos1=bpncpt_id_seq_in_list(aVeFormula->iCptList[i], aId);
		if(idPos1<0) { continue; }
		else
		{
			for(j=i+1; j<aVeFormula->iCptCount; )
			{
				idPos2=bpncpt_id_seq_in_list(aVeFormula->iCptList[j], aId);
				if(idPos2<0) { j++; }
				else
				{
					// create new node
					SBpnCpt * newNode=(SBpnCpt*)malloc(sizeof(SBpnCpt));
					bpncpt_create(newNode, NULL, 0);
					bpncpt_multiplicate_2(aVeFormula->iCptList[i], aVeFormula->iCptList[j], newNode);
					
					// free used cpts
					bpncpt_free(aVeFormula->iCptList[i]);
					free(aVeFormula->iCptList[i]);

					bpncpt_free(aVeFormula->iCptList[j]);
					free(aVeFormula->iCptList[j]);

					// replace with newNode
					aVeFormula->iCptList[i]=newNode;
					
					// pull the next nodes
					for(k=j; k<aVeFormula->iCptCount-1; k++)
					{
						aVeFormula->iCptList[k]=aVeFormula->iCptList[k+1];
					}

					// decrement cpt count
					aVeFormula->iCptCount--;
				}
			}

			// when j already reaches end of cpt list, that means multiplication process has finished, let's proceed to summing-out
			if(j==aVeFormula->iCptCount)
			{
				// create new node
				SBpnCpt * newNode=(SBpnCpt*)malloc(sizeof(SBpnCpt));
				bpncpt_create(newNode, NULL, 0);
				status=bpncpt_sum_out(aVeFormula->iCptList[i], aId, newNode);

				if(status==0)
				{
					// free current cpt
					bpncpt_free(aVeFormula->iCptList[i]);
					free(aVeFormula->iCptList[i]);

					// replace with newNode
					aVeFormula->iCptList[i]=newNode;
				}
				// else also includes case where count of index of iCptList[i] is exactly one (nothing to sum-out)

				break;
			}
		}
	}

	return 0;
}

