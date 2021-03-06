#include "Transaction.h"

BACnetResult CTransaction::OnTimerExpired(CObjectPtr<IBACnetThreadpool> pThreadpool, CallbackHandle pHandle, CObjectPtr<IBACnetThreadpoolTimer> pTimer)
{
	if(segwait)
	{
		SegmentTimerTimeout();
	}
	else
	{
		RequestTimerTimeout();
	}
	return BC_OK;
}

void CTransaction::StartRequestTimer(U32 Timeout)
{
	if(Timer)
	{
		segwait = false;
		Timer->WaitFor(Timeout, 0);
	}
}

void CTransaction::StopTimer(bool DontBlock)
{
	if(Timer)
	{
		Timer->Cancel();
	}
}

void CTransaction::ResetTimer(U32 Timeout)
{
	if(Timer)
	{
		Timer->WaitFor(Timeout, 0);
	}
}

void CTransaction::StartSegmentTimer(U32 Timeout)
{
	if(Timer)
	{
		segwait = true;
		Timer->WaitFor(Timeout, 0);
	}
}

bool CTransaction::InWindow(U8 SeqNumA, U8 SeqNumB)
{
	return ((SeqNumA - SeqNumB) & 0xFF) < ActualWindowSize;
}

bool CTransaction::DuplicateInWindow(U8 SeqNumA, U8 FirstSeqNum, U8 LastSeqNum)
{
	U8 ReceivedCount = (LastSeqNum - FirstSeqNum) & 0xFF;
	return ((ReceivedCount <= ActualWindowSize) && (((SeqNumA - FirstSeqNum) & 0xFF) <= ReceivedCount));
}


BACnetResult CTransaction::SendSegmentAck(bool IsServer, bool IsNAK)
{
	U8 buf[] = {
		(U8)((PDUType_SegmentAck << 4) | (IsServer ? PDUFlag_IsServer : 0) | (IsNAK ? PDUFlag_SegmentNAK : 0)),
		InvokeID,
		LastSequenceNumber,
		ActualWindowSize,
	};
	CObjectPtr<IBACnetBuffer> b = CreateBACnetBuffer(sizeof(buf), buf);
	CObjectPtr<IBACnetTransmitBuffer> tx = CreateBACnetTransmitBuffer();
	tx->Push(b);
	return p->net->WriteAPDU(dest, tx, Message_WaitForTransmit);
}

BACnetResult CTransaction::CompletePDUProcessing(BACnetResult ReturnCode)
{
	if(State != State_Complete)
	{
		ResultCode = ReturnCode;
		State = State_Complete;
		p->RemoveTransaction(CObjectPtr<CTransaction>(this));
		pEvent->Set();
	}
	return BC_OK;
}

CTransaction::CTransaction(CObjectPtr<CSegmentAssembler> Parent, U8 InvokeId, CObjectPtr<IBACnetNetworkAddress> pDest) :
	p(Parent),
	dest(pDest),
	pEvent(CreateBACnetEvent(true, false)),
	Timer(CreateBACnetThreadpoolTimer(std::bind(&CTransaction::OnTimerExpired, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))),
	ResultCode(0),
	RetryCount(0),
	SegmentRetryCount(0),
	DuplicateCount(0),
	Tseg(1000),
	Tsegwait(Tseg << 2),
	Tout(5000),
	Nretry(3),
	LastSequenceNumber(0),
	InitialSequenceNumber(0),
	ActualWindowSize(0),
	ProposedWindowSize(0),
	InvokeID(InvokeId),
	ServiceChoice(0),
	State(State_Idle),
	MaxAPDUSize(MaxAPDU_MinSize),
	MaxSegmentsAllowed(p->MaxSegmentsAllowed),
	SentAllSegments(true)
{
}


CTransaction::~CTransaction()
{
	StopTimer();
	if(State != State_Complete)
	{
		OutputDebugString(L"Transaction is not complete upon closing the TSM!");
		__debugbreak();
	}
}

U8 CTransaction::GetInvokeID()
{
	return InvokeID;
}

CObjectPtr<IBACnetNetworkAddress> CTransaction::GetSender() const
{
	return dest;
}

U8 CTransaction::GetServiceChoice() const
{
	return ServiceChoice;
}

BACnetResult CTransaction::GetTransactionResult() const
{
	return ResultCode;
}

BACnetResult CTransaction::WaitForCompletion()
{
	return WaitForObject(pEvent);
}

CObjectPtr<IBACnetBuffer> CTransaction::GetTransactionData()
{
	return segs.CoalesceSegments();
}

