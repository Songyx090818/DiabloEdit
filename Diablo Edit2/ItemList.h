#pragma once

#include "D2Item.h"

struct CItemList
{
//members
	CItemList();
	~CItemList();
	void ReadData(CInBitsStream & bs);
	void WriteData(COutBitsStream & bs) const;
private:
	void ClearItems();
//fields
public:
	//������Ʒ��Ϣ
	WORD		wMajic;		//0x4D4A,"JM"
	WORD		nItems;		//��Ʒ��Ŀ
	std::vector<CD2Item *>	vpItems;
	WORD		wEndMajic;	//0x4D4A,"JM"
};