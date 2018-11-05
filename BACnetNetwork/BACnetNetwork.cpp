#include "BACnetNetwork.h"
#include "SimpleNetwork.h"

CObjectPtr<IBACnetSimpleNetwork> BACNETNETWORK_ENTRYPOINT CreateSimpleNetwork(CObjectPtr<IBACnetThreadpool> pThreadPool)
{
	try
	{
		return CObjectPtr<IBACnetSimpleNetwork>(new CSimpleNetwork(pThreadPool), true);
	}
	catch(BACnetException ex)
	{
		return nullptr;
	}
}
