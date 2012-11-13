
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "bpncpt.h"

int bpncpt_create(SBpnCpt * aCpt, unsigned short * aIdList, unsigned char aIdCount)
{
	// error trapper
	if(aIdCount>32) { return -1; }

	aCpt->iIdCount=aIdCount;
	if(aIdCount>0)
	{
		// build id list
		aCpt->iIdList=(unsigned short*)malloc(aIdCount*sizeof(unsigned short));
		if(aCpt->iIdList==NULL) { return -2; }

		memcpy(aCpt->iIdList, aIdList, aIdCount*sizeof(unsigned short));

		// calculate table size
		aCpt->iTableSize=0x00000000 | (1 << aIdCount);

		// build empty table
		aCpt->iTable=(double*)malloc(aCpt->iTableSize*sizeof(double));
		if(aCpt->iTable==NULL)
		{
			free(aCpt->iIdList);
			aCpt->iIdList=NULL;
			return -3;
		}
	}
	else
	{
		aCpt->iIdList=NULL;

		aCpt->iTableSize=0;
		aCpt->iTable=NULL;
	}

	return 0;
}

void bpncpt_copy_table(SBpnCpt * aCpt, double * aTable)
{
	memcpy(aCpt->iTable, aTable, aCpt->iTableSize*sizeof(double));
}

void bpncpt_free(SBpnCpt * aCpt)
{
	// clear id list
	memset(aCpt->iIdList, 0x0, aCpt->iIdCount);
	aCpt->iIdCount=0;
	if(aCpt->iIdList) { free(aCpt->iIdList); aCpt->iIdList=NULL; }

	// clear table
	aCpt->iTableSize=0;
	if(aCpt->iTable) { free(aCpt->iTable); aCpt->iTable=NULL; }
}

int bpncpt_sum_out(SBpnCpt * aCpt, unsigned short aSumOutId, SBpnCpt * aResult)
{
	// vars
	unsigned int i, j, status;

	// check if aSumOutId is in aCpt->iIdList
	int idxInList=bpncpt_id_seq_in_list(aCpt, aSumOutId);
	if(idxInList<0) { return -1; }

	// if aSumOutId is the only id in list, return with error
	if(aCpt->iIdCount==1) { return -2; }

	// id count should equal with aCpt->iIdCount-1
	aResult->iIdCount=aCpt->iIdCount-1;	

	// alloc id list
	if(aResult->iIdList) { free(aResult->iIdList); aResult->iIdList=NULL; }
	aResult->iIdList=(unsigned short*)malloc(aResult->iIdCount*sizeof(unsigned short));

	// set aResult's id list
	for(i=0, j=0; i<aCpt->iIdCount; i++)
	{
		if(aCpt->iIdList[i]!=aSumOutId) { aResult->iIdList[j++]=aCpt->iIdList[i]; }
	}

	// set table size value
	aResult->iTableSize=0x00000000 | (1 << aResult->iIdCount);

	// alloc table for aResult
	if(aResult->iTable) { free(aResult->iTable); aResult->iTable=NULL; }
	aResult->iTable=(double*)malloc(aResult->iTableSize*sizeof(double));
	memset(aResult->iTable, 0x0, aResult->iTableSize*sizeof(double));

	// recalculate and remap result
	for(i=0; i<aCpt->iTableSize; i++)
	{
		unsigned int res;
		status=bpncpt__determine_new_index(i, (unsigned char)(idxInList), &res);
		aResult->iTable[res]+=aCpt->iTable[i];
	}

	return 0;
}

int bpncpt_multiplicate(SBpnCpt * aCpt1, SBpnCpt * aCpt2, SBpnCpt * aResult)
{
	int i, j;
	int status;

	// define bigger and smaller cpt
	SBpnCpt * smallerCpt;
	SBpnCpt * biggerCpt;

	// cpt with less count of id is the smaller
	if(bpncpt__id_count_cmp(aCpt1, aCpt2)<0)
	{
		smallerCpt=aCpt1;
		biggerCpt=aCpt2;
	}
	else
	{
		smallerCpt=aCpt2;
		biggerCpt=aCpt1;
	}

	// check if both cpts are valid for multiplication
	status=bpncpt__validate_for_multiplication(biggerCpt, smallerCpt);
	if(status!=0) { return -1; }

	// create aResult with map as large as bigger cpt
	aResult->iIdCount=biggerCpt->iIdCount;
	aResult->iTableSize=0x00000000 | (1 << aResult->iIdCount);

	// realloc the id list
	if(aResult->iIdList) { free(aResult->iIdList); aResult->iIdList=NULL; }
	aResult->iIdList=(unsigned short*)malloc(aResult->iIdCount*sizeof(unsigned short));

	// set aResult's id list
	memcpy(aResult->iIdList, biggerCpt->iIdList, aResult->iIdCount*sizeof(unsigned short));

	// realloc the table
	if(aResult->iTable) { free(aResult->iTable); aResult->iTable=NULL; }
	aResult->iTable=(double*)malloc(aResult->iTableSize*sizeof(double));
	memset(aResult->iTable, 0x0, aResult->iTableSize*sizeof(double));

	for(i=0; i<aResult->iTableSize; i++)
	{
		// find mask to be used as index in smaller cpt
		unsigned int smallerTableMask=0x00000000;
		for(j=0; j<smallerCpt->iIdCount; j++)
		{
			// get active status of selected id in bigger table
			int idStatus=bpncpt__id_status_in_table_index(biggerCpt, smallerCpt->iIdList[j], i);

			// if true then bit should be 1
			if(idStatus==1)
			{
				// append bitmask of id in smaller table
				int idSeq=bpncpt_id_seq_in_list(smallerCpt, smallerCpt->iIdList[j]);
				smallerTableMask |= (1 << idSeq);
			}
		}
		// fill content of aResult at index i
		aResult->iTable[i]=biggerCpt->iTable[i]*smallerCpt->iTable[smallerTableMask];
	}

	return 0;
}

int bpncpt_multiplicate_2(SBpnCpt * aCpt1, SBpnCpt * aCpt2, SBpnCpt * aResult)
{
	// vars
	int i, j, k;
	unsigned short idList[64]; // 64 is maximum possibility of number of triggers of both cpts joined
	SBpnCpt bpnCpt[2];

	// insert all index in aCpt1
	for(i=0; i<aCpt1->iIdCount; i++)
	{
		idList[i]=aCpt1->iIdList[i];
	}
	// insert distinct index from aCpt2, retain variable 'i' to later be used as counter
	for(j=0; j<aCpt2->iIdCount; j++)
	{
		for(k=0; k<i; k++)
		{
			if(idList[k]==aCpt2->iIdList[j])
			{
				break;
			}
		}
		if(k==i) { idList[i++]=aCpt2->iIdList[j]; }
	}

	// create temporary cpts
	bpncpt_create(&bpnCpt[0], idList, i);
	bpncpt_create(&bpnCpt[1], NULL, 0);

	// convert value of first temp cpt to 0
	double * ptr=bpnCpt[0].iTable;
	for(j=0; j<bpnCpt[0].iTableSize; j++)
	{
		*ptr++=1.0f;
	}

	// multiplicate first temp cpt with aCpt1, result = second temp cpt
	bpncpt_multiplicate(&bpnCpt[0], aCpt1, &bpnCpt[1]);

	// multiplicate second cpt with aCpt2 and store the result in aResult
	bpncpt_multiplicate(&bpnCpt[1], aCpt2, aResult);

	// free both temp cpts
	bpncpt_free(&bpnCpt[0]);
	bpncpt_free(&bpnCpt[1]);

	return 0;
}

int bpncpt_eliminate_constant_value(SBpnCpt * aCpt, unsigned short aConstantId, unsigned char aConstantValue, SBpnCpt * aResult)
{
	// vars
	unsigned int i, j, status;
	unsigned int mask;

	// check if aConstantId is in aCpt->iIdList
	int idxInList=bpncpt_id_seq_in_list(aCpt, aConstantId);
	if(idxInList<0) { return -1; }

	// get mask
	bpncpt_id_mask_in_list(aCpt, aConstantId, &mask);

	// id count should equal with aCpt->iIdCount-1
	aResult->iIdCount=aCpt->iIdCount-1;	

	// alloc id list
	if(aResult->iIdList) { free(aResult->iIdList); aResult->iIdList=NULL; }
	aResult->iIdList=(unsigned short*)malloc(aResult->iIdCount*sizeof(unsigned short));

	// set aResult's id list
	for(i=0, j=0; i<aCpt->iIdCount; i++)
	{
		if(aCpt->iIdList[i]!=aConstantId) { aResult->iIdList[j++]=aCpt->iIdList[i]; }
	}

	// set table size value
	aResult->iTableSize=0x00000000 | (1 << aResult->iIdCount);

	// alloc table for aResult
	if(aResult->iTable) { free(aResult->iTable); aResult->iTable=NULL; }
	aResult->iTable=(double*)malloc(aResult->iTableSize*sizeof(double));
	memset(aResult->iTable, 0x0, aResult->iTableSize*sizeof(double));

	// recalculate and remap result
	for(i=0; i<aCpt->iTableSize; i++)
	{
		unsigned int res;
		status=bpncpt__determine_new_index(i, (unsigned char)(idxInList), &res);
		if((i&mask) && aConstantValue) // if both id value at index and aConstantValue is true
		{
			aResult->iTable[res]=aCpt->iTable[i];
		}
		else if(!(i&mask) && !aConstantValue) // if both id value at index and aConstantValue is false
		{
			aResult->iTable[res]=aCpt->iTable[i];			
		}
	}

	return 0;
}

int bpncpt_normalize_table(SBpnCpt * aCpt, SBpnCpt * aResult)
{
	// vars
	int status;
	double alpha;
	unsigned char i;

	status=bpncpt__get_normalizing_constant(aCpt, &alpha);
	if(status!=0) { return -1; }

	// set id count
	aResult->iIdCount=aCpt->iIdCount;

	// alloc id list
	if(aResult->iIdList) { free(aResult->iIdList); aResult->iIdList=NULL; }
	aResult->iIdList=(unsigned short*)malloc(aResult->iIdCount*sizeof(unsigned short));

	// set aResult's id list
	memcpy(aResult->iIdList, aCpt->iIdList, aResult->iIdCount*sizeof(unsigned short));

	// set table size value
	aResult->iTableSize=0x00000000 | (1 << aResult->iIdCount);

	// alloc table for aResult
	if(aResult->iTable) { free(aResult->iTable); aResult->iTable=NULL; }
	aResult->iTable=(double*)malloc(aResult->iTableSize*sizeof(double));
	memset(aResult->iTable, 0x0, aResult->iTableSize*sizeof(double));

	// calculate result
	for(i=0; i<aResult->iTableSize; i++)
	{
		aResult->iTable[i]=alpha*aCpt->iTable[i];
	}

	return 0;
}

int bpncpt_id_seq_in_list(SBpnCpt * aCpt, unsigned short aId)
{
	int i;

	// query aId in aCpt
	for(i=0; i<aCpt->iIdCount; i++)
	{
		// if found return the index
		if(aCpt->iIdList[i]==aId) { return i; }
	}

	// if not found return -1
	return -1;
}

int bpncpt_id_mask_in_list(SBpnCpt * aCpt, unsigned short aId, unsigned int * aMask)
{
	// find sequence in list
	int idSeq=bpncpt_id_seq_in_list(aCpt, aId);
	if(idSeq<0) { return -1; }

	// create mask by left-shifting 1 as many as the sequence
	*aMask=0x00000000 | (1 << idSeq);
	return 0;
}

int bpncpt__id_status_in_table_index(SBpnCpt * aCpt, unsigned short aId, unsigned int aTableIndex)
{
	int mask, status;

	// get mask of queried id
	status=bpncpt_id_mask_in_list(aCpt, aId, &mask);
	if(status<0) { return -1; }

	// return masking result
	if(aTableIndex & mask) { return 1; }
	else { return 0; }
}

int bpncpt__determine_new_index(unsigned int aBitMap, unsigned char aSkippedBit, unsigned int * aNewBitMap)
{
	// trap overflow attempt
	if(aSkippedBit>31) { return -1; }

	// set mask of skipped bit
	unsigned int mapMask=0x00000000 | (1 << aSkippedBit);

	// set all bits before the skipped bit to 1
	unsigned int lowerMask=mapMask-1;

	// set all bits including the skipped bit to 1
	unsigned int lowerIncludingSkippedMask=mapMask+lowerMask;

	// reverse the lowerIncludingSkippedMask to get higherMask
	unsigned int higherMask=~lowerIncludingSkippedMask;
	
	// remap result to aNewBitmap
	*aNewBitMap=(0x00000000 | (aBitMap & lowerMask)) | ((aBitMap & higherMask) >> 1);

	return 0;
}

int bpncpt__id_count_cmp(SBpnCpt * aCpt1, SBpnCpt * aCpt2)
{
	return aCpt1->iIdCount-aCpt2->iIdCount;
}

int bpncpt__validate_for_multiplication(SBpnCpt * aBiggerCpt, SBpnCpt * aSmallerCpt)
{
	int i, j;

	// ensure all id in smaller cpt exists in bigger cpt
	for(i=0; i<aSmallerCpt->iIdCount; i++)
	{
		for(j=0; j<aBiggerCpt->iIdCount; j++)
		{
			if(aSmallerCpt->iIdList[i]==aBiggerCpt->iIdList[j]) { break; }
		}
		if(j==aBiggerCpt->iIdCount) { return -1; /* false */ }
	}
	
	return 0; /* true */
}

int bpncpt__get_normalizing_constant(SBpnCpt * aCpt, double * aAlpha)
{
	if(aCpt->iIdCount!=1) { return -1; }

	*aAlpha=1.0f/(aCpt->iTable[0]+aCpt->iTable[1]);

	return 0;
}

