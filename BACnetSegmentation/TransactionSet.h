#pragma once

#include <unordered_map>
using namespace std;

class TransactionSet;

#include "Transaction.h"

class TransactionSet : 
	public ComWrapper<IBACnetUnknown>
{
	typedef pair<U8, CObjectPtr<IBACnetNetworkAddress>> TransactionKey;
	struct HashAddressPair
	{
		size_t operator()(const TransactionKey& _Keyval) const;
	};
	typedef unordered_map<TransactionKey, CObjectPtr<CTransaction>, HashAddressPair> TransactionMap;
	TransactionMap m;
	SRWLOCK lock;

public:
	TransactionSet();
	~TransactionSet();

	CObjectPtr<CTransaction> Find(U8 InvokeID, CObjectPtr<IBACnetNetworkAddress> pAddr);
	bool Add(CObjectPtr<CTransaction> trx);
	bool Remove(CObjectPtr<CTransaction> trx);
};

