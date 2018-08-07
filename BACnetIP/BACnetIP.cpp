#include "BACnetIP.h"

#include "LocalSocket.h"
#include "WideAreaSubnet.h"

CObjectPtr<IBACnetLocalSubnet> CreateLocalSubnet(CObjectPtr<IBACnetThreadPool> pThreadPool, U16 PortNumber)
{
	try
	{
		return CObjectPtr<IBACnetLocalSubnet>(new LocalSocket(pThreadPool, PortNumber), true);
	}
	catch(BACnetException ex)
	{
		return nullptr;
	}
}

CObjectPtr<IBACnetWideAreaSubnet> CreateWideAreaSubnet(CObjectPtr<IBACnetThreadPool> pThreadPool, U16 PortNumber)
{
	try
	{
		return CObjectPtr<IBACnetWideAreaSubnet>(new WideAreaSubnet(pThreadPool, PortNumber), true);
	}
	catch(BACnetException ex)
	{
		return nullptr;
	}
}
