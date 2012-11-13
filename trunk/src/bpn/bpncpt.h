
#ifndef __BPN_CPT_H__
#define __BPN_CPT_H__

#pragma pack(push, 1)

typedef struct
{
	unsigned char iIdCount;
	unsigned short * iIdList;

	unsigned int iTableSize;

	double * iTable;
} SBpnCpt;

#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// public functions
extern int bpncpt_create(SBpnCpt * aCpt, unsigned short * aIdList, unsigned char aIdCount);
extern void bpncpt_copy_table(SBpnCpt * aCpt, double * aTable);
extern void bpncpt_free(SBpnCpt * aCpt);
extern int bpncpt_sum_out(SBpnCpt * aCpt, unsigned short aSumOutId, SBpnCpt * aResult);
extern int bpncpt_multiplicate(SBpnCpt * aCpt1, SBpnCpt * aCpt2, SBpnCpt * aResult);
extern int bpncpt_multiplicate_2(SBpnCpt * aCpt1, SBpnCpt * aCpt2, SBpnCpt * aResult);
extern int bpncpt_eliminate_constant_value(SBpnCpt * aCpt, unsigned short aConstantId, unsigned char aConstantValue, SBpnCpt * aResult);
extern int bpncpt_normalize_table(SBpnCpt * aCpt, SBpnCpt * aResult);
extern int bpncpt_id_seq_in_list(SBpnCpt * aCpt, unsigned short aId);
extern int bpncpt_id_mask_in_list(SBpnCpt * aCpt, unsigned short aId, unsigned int * aMask);

// private functions
extern int bpncpt__id_status_in_table_index(SBpnCpt * aCpt, unsigned short aId, unsigned int aTableIndex);
extern int bpncpt__determine_new_index(unsigned int aBitMap, unsigned char aSkippedBit, unsigned int * aNewBitMap);
extern int bpncpt__id_count_cmp(SBpnCpt * aCpt1, SBpnCpt * aCpt2);
extern int bpncpt__validate_for_multiplication(SBpnCpt * aBiggerCpt, SBpnCpt * aSmallerCpt);
extern int bpncpt__get_normalizing_constant(SBpnCpt * aCpt, double * aAlpha);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __BPN_CPT_H__

