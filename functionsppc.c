// Copyright (c) 2019 Dennis van der Boon
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <exec/types.h>
#include <devices/timer.h>
#include <powerpc/powerpc.h>
#include <powerpc/tasksPPC.h>
#include <powerpc/memoryPPC.h>
#include <powerpc/semaphoresPPC.h>
#include <powerpc/portsPPC.h>
#include "libstructs.h"
#include "Internalsppc.h"

#define function 0 //debug

PPCFUNCTION ULONG myRun68K(struct PrivatePPCBase* PowerPCBase, struct PPCArgs* PPStruct)
{
    return 0;
}

PPCFUNCTION ULONG myWaitFor68K(struct PrivatePPCBase* PowerPCBase, struct PPCArgs* PPStruct)
{
    return 0;
}

PPCFUNCTION VOID mySPrintF(struct PrivatePPCBase* PowerPCBase, STRPTR Formatstring, APTR Values)
{
    return;
}

PPCFUNCTION APTR myAllocVecPPC(struct PrivatePPCBase* PowerPCBase, ULONG size, ULONG flags, ULONG align)
{
    return NULL;
}

PPCFUNCTION LONG myFreeVecPPC(struct PrivatePPCBase* PowerPCBase, APTR memblock)
{
    return 0;
}

PPCFUNCTION struct TaskPPC* myCreateTaskPPC(struct PrivatePPCBase* PowerPCBase, struct TagItem* taglist)
{
    return NULL;
}

PPCFUNCTION VOID myDeleteTaskPPC(struct PrivatePPCBase* PowerPCBase, struct TaskPPC* PPCtask)
{
    return;
}

PPCFUNCTION struct TaskPPC* myFindTaskPPC(struct PrivatePPCBase* PowerPCBase, STRPTR name)
{
    return NULL;
}

PPCFUNCTION LONG myInitSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return 0;
}

PPCFUNCTION VOID myFreeSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return;
}

PPCFUNCTION LONG myAddSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
	LONG result = 0;
	printDebugEntry(PowerPCBase, function, (ULONG)SemaphorePPC, 0, 0, 0);

	if (result = myInitSemaphorePPC(PowerPCBase, SemaphorePPC))
	{
		myObtainSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemSemList);

		myEnqueuePPC(PowerPCBase, (struct List*)&PowerPCBase->pp_Semaphores, (struct Node*)SemaphorePPC);

		myReleaseSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemSemList);

	}
    printDebugExit(PowerPCBase, function, (ULONG)result);

	return result;
}

PPCFUNCTION VOID myRemSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
	printDebugEntry(PowerPCBase, function, (ULONG)SemaphorePPC, 0, 0, 0);

	myObtainSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemSemList);

	myRemovePPC(PowerPCBase, (struct Node*)SemaphorePPC);

	myReleaseSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemSemList);

	myFreeSemaphorePPC(PowerPCBase, SemaphorePPC);

    printDebugExit(PowerPCBase, function, 0);
}

PPCFUNCTION VOID myObtainSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return;
}

PPCFUNCTION LONG myAttemptSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return 0;
}

PPCFUNCTION VOID myReleaseSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return;
}

PPCFUNCTION struct SignalSemaphorePPC* myFindSemaphorePPC(struct PrivatePPCBase* PowerPCBase, STRPTR name)
{
	printDebugEntry(PowerPCBase, function, (ULONG)name, 0, 0, 0);

	myObtainSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemSemList);

	struct SignalSemaphorePPC* mySem = (struct SignalSemaphorePPC*)myFindNamePPC(PowerPCBase, (struct List*)&PowerPCBase->pp_SemSemList, name);

	myReleaseSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemSemList);

    printDebugExit(PowerPCBase, function, (ULONG)mySem);

	return mySem;
}

PPCFUNCTION VOID myInsertPPC(struct PrivatePPCBase* PowerPCBase, struct List* list, struct Node* node, struct Node* pred)
{
	if (pred == 0)
	{
		myAddHeadPPC(PowerPCBase, list, node);
        return;
	}

	struct Node* succ = pred->ln_Succ;

	if (succ)
	{
		node->ln_Succ = succ;
		node->ln_Pred = pred;
		succ->ln_Pred = node;
		pred->ln_Succ = node;
		return;
	}

	struct Node* predPred = pred->ln_Pred;

	node->ln_Succ = pred;
	pred->ln_Pred = node;
	predPred->ln_Succ = node;

	return;
}

PPCFUNCTION VOID myAddHeadPPC(struct PrivatePPCBase* PowerPCBase, struct List* list, struct Node* node)
{
	struct Node* succ = list->lh_Head;
	list->lh_Head = node;
	node->ln_Succ = succ;
	node->ln_Pred = (struct Node*)list;
	succ->ln_Pred = node;

	return;
}

PPCFUNCTION VOID myAddTailPPC(struct PrivatePPCBase* PowerPCBase, struct List* list, struct Node* node)
{
	struct Node* tailpred = list->lh_TailPred;
	list->lh_TailPred = node;
	node->ln_Succ = (struct Node*)&list->lh_Tail;
	node->ln_Pred = tailpred;
	tailpred->ln_Succ = node;

	return;
}

PPCFUNCTION VOID myRemovePPC(struct PrivatePPCBase* PowerPCBase, struct Node* node)
{
	struct Node* succ = node->ln_Succ;
	struct Node* pred = node->ln_Pred;
	succ->ln_Pred = pred;
	pred->ln_Succ = succ;

	return;
}

PPCFUNCTION struct Node* myRemHeadPPC(struct PrivatePPCBase* PowerPCBase, struct List* list)
{
	struct Node* firstNode = list->lh_Head;
	struct Node* succ = firstNode->ln_Succ;
	if (succ)
	{
		list->lh_Head = succ;
		succ->ln_Pred = (struct Node*)list;
		return firstNode;
	}
	return 0;
}

PPCFUNCTION struct Node* myRemTailPPC(struct PrivatePPCBase* PowerPCBase, struct List* list)
{
	struct Node* tailpred = list->lh_TailPred;
	struct Node* pred = tailpred->ln_Pred;
	if (pred)
	{
		list->lh_TailPred = pred;
		pred->ln_Succ = (struct Node*)&list->lh_Tail;
		return tailpred;
	}
	return 0;
}

PPCFUNCTION VOID myEnqueuePPC(struct PrivatePPCBase* PowerPCBase, struct List* list, struct Node* node)
{
	LONG myPrio = (LONG)node->ln_Pri;
	struct Node* nextNode = list->lh_Head;

	while(nextNode->ln_Succ)
	{
		LONG cmpPrio = (LONG)nextNode->ln_Pri;

		if (cmpPrio > myPrio)
		{
			break;
		}
		nextNode = nextNode->ln_Succ;
	}
	struct Node* pred = nextNode->ln_Pred;
	nextNode->ln_Pred = node;
	node->ln_Succ = nextNode;
	node->ln_Pred = pred;
	pred->ln_Succ = node;

	return;
}

PPCFUNCTION struct Node* myFindNamePPC(struct PrivatePPCBase* PowerPCBase, struct List* list, STRPTR name)
{
    return NULL;
}

PPCFUNCTION struct TagItem* myFindTagItemPPC(struct PrivatePPCBase* PowerPCBase, ULONG value, struct TagItem* taglist)
{
    return NULL;
}

PPCFUNCTION ULONG myGetTagDataPPC(struct PrivatePPCBase* PowerPCBase, ULONG value, ULONG d0arg, struct TagItem* taglist)
{
    return 0;
}

PPCFUNCTION struct TagItem* myNextTagItemPPC(struct PrivatePPCBase* PowerPCBase, struct TagItem** tagitem)
{
    return NULL;
}

PPCFUNCTION LONG myAllocSignalPPC(struct PrivatePPCBase* PowerPCBase, LONG signum)
{
    return 0;
}

PPCFUNCTION VOID myFreeSignalPPC(struct PrivatePPCBase* PowerPCBase, LONG signum)
{
    return;
}

PPCFUNCTION ULONG mySetSignalPPC(struct PrivatePPCBase* PowerPCBase, ULONG signals, ULONG mask)
{
    return 0;
}

PPCFUNCTION VOID mySignalPPC(struct PrivatePPCBase* PowerPCBase, struct TaskPPC* task, ULONG signals)
{
    return;
}

PPCFUNCTION ULONG myWaitPPC(struct PrivatePPCBase* PowerPCBase, ULONG signals)
{
    return 0;
}

PPCFUNCTION LONG mySetTaskPriPPC(struct PrivatePPCBase* PowerPCBase, struct TaskPPC* task, LONG pri)
{
    return 0;
}

PPCFUNCTION VOID mySignal68K(struct PrivatePPCBase* PowerPCBase, struct Task* task, ULONG signals)
{
    return;
}

PPCFUNCTION VOID mySetCache(struct PrivatePPCBase* PowerPCBase, ULONG flags, APTR start, ULONG length)
{
    return;
}

PPCFUNCTION APTR mySetExcHandler(struct PrivatePPCBase* PowerPCBase, struct TagItem* taglist)
{
    return NULL;
}

PPCFUNCTION VOID myRemExcHandler(struct PrivatePPCBase* PowerPCBase, APTR xlock)
{
    return;
}

PPCFUNCTION ULONG mySuper(struct PrivatePPCBase* PowerPCBase)
{
    return 0;
}

PPCFUNCTION VOID myUser(struct PrivatePPCBase* PowerPCBase, ULONG key)
{
    return;
}

PPCFUNCTION ULONG mySetHardware(struct PrivatePPCBase* PowerPCBase, ULONG flags, APTR param)
{
    return 0;
}

PPCFUNCTION VOID myModifyFPExc(struct PrivatePPCBase* PowerPCBase, ULONG fpflags)
{
    return;
}

PPCFUNCTION ULONG myWaitTime(struct PrivatePPCBase* PowerPCBase, ULONG signals, ULONG time)
{
    return 0;
}

PPCFUNCTION struct TaskPtr* myLockTaskList(struct PrivatePPCBase* PowerPCBase)
{
	myObtainSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemTaskList);
	return ((struct TaskPtr*)&PowerPCBase->pp_AllTasks.mlh_Head);
}

PPCFUNCTION VOID myUnLockTaskList(struct PrivatePPCBase* PowerPCBase)
{
	myReleaseSemaphorePPC(PowerPCBase, &PowerPCBase->pp_SemTaskList);
	return;
}

PPCFUNCTION VOID mySetExcMMU(struct PrivatePPCBase* PowerPCBase)
{
    return;
}

PPCFUNCTION VOID myClearExcMMU(struct PrivatePPCBase* PowerPCBase)
{
    return;
}

PPCFUNCTION VOID myChangeMMU(struct PrivatePPCBase* PowerPCBase, ULONG mode)
{
    return;
}

PPCFUNCTION VOID myGetInfo(struct PrivatePPCBase* PowerPCBase, struct TagItem* taglist)
{
    return;
}

PPCFUNCTION struct MsgPortPPC* myCreateMsgPortPPC(struct PrivatePPCBase* PowerPCBase)
{
    return NULL;
}

PPCFUNCTION VOID myDeleteMsgPortPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPortPPC* port)
{
    return;
}

PPCFUNCTION VOID myAddPortPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPortPPC* port)
{
    return;
}

PPCFUNCTION VOID myRemPortPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPortPPC* port)
{
    return;
}

PPCFUNCTION struct MsgPortPPC* myFindPortPPC(struct PrivatePPCBase* PowerPCBase, STRPTR port)
{
    return NULL;
}

PPCFUNCTION struct Message* myWaitPortPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPortPPC* port)
{
    return NULL;
}

PPCFUNCTION VOID myPutMsgPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPortPPC* port, struct Message* message)
{
    return;
}

PPCFUNCTION struct Message* myGetMsgPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPortPPC* port)
{
    return NULL;
}

PPCFUNCTION VOID myReplyMsgPPC(struct PrivatePPCBase* PowerPCBase, struct Message* message)
{
    return;
}

PPCFUNCTION VOID myFreeAllMem(struct PrivatePPCBase* PowerPCBase)
{
    return;
}

PPCFUNCTION VOID myCopyMemPPC(struct PrivatePPCBase* PowerPCBase, APTR source, APTR dest, ULONG size)
{
    return;
}

PPCFUNCTION struct Message* myAllocXMsgPPC(struct PrivatePPCBase* PowerPCBase, ULONG length, struct MsgPortPPC* port)
{
    return NULL;
}

PPCFUNCTION VOID myFreeXMsgPPC(struct PrivatePPCBase* PowerPCBase, struct Message* message)
{
	myFreeVecPPC(PowerPCBase, message);

	return;
}

PPCFUNCTION VOID myPutXMsgPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPort* port, struct Message* message)
{
    return;
}

PPCFUNCTION VOID myGetSysTimePPC(struct PrivatePPCBase* PowerPCBase, struct timeval* timeval)
{
    return;
}

PPCFUNCTION VOID myAddTimePPC(struct PrivatePPCBase* PowerPCBase, struct timeval* dest, struct timeval* source)
{
    return;
}

PPCFUNCTION VOID mySubTimePPC(struct PrivatePPCBase* PowerPCBase, struct timeval* dest, struct timeval* source)
{
    return;
}

PPCFUNCTION LONG myCmpTimePPC(struct PrivatePPCBase* PowerPCBase, struct timeval* dest, struct timeval* source)
{
    return 0;
}

PPCFUNCTION struct MsgPortPPC* mySetReplyPortPPC(struct PrivatePPCBase* PowerPCBase, struct Message* message, struct MsgPortPPC* port)
{
    return NULL;
}

PPCFUNCTION ULONG mySnoopTask(struct PrivatePPCBase* PowerPCBase, struct TagItem* taglist)
{
    return 0;
}

PPCFUNCTION VOID myEndSnoopTask(struct PrivatePPCBase* PowerPCBase, ULONG id)
{
    return;
}

PPCFUNCTION VOID myGetHALInfo(struct PrivatePPCBase* PowerPCBase, struct TagItem* taglist)
{
    return;
}

PPCFUNCTION VOID mySetScheduling(struct PrivatePPCBase* PowerPCBase, struct TagItem* taglist)
{
    return;
}

PPCFUNCTION struct TaskPPC* myFindTaskByID(struct PrivatePPCBase* PowerPCBase, LONG id)
{
    return NULL;
}

PPCFUNCTION LONG mySetNiceValue(struct PrivatePPCBase* PowerPCBase, struct TaskPPC* task, LONG nice)
{
    return 0;
}

PPCFUNCTION LONG myTrySemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC, ULONG timeout)
{
    return 0;
}

PPCFUNCTION VOID myNewListPPC(struct PrivatePPCBase* PowerPCBase, struct List* list)
{
	list->lh_TailPred = (struct Node*)list;
	list->lh_Tail = 0;
	list->lh_Head = (struct Node*)((ULONG)list+4);

	return;
}

PPCFUNCTION ULONG mySetExceptPPC(struct PrivatePPCBase* PowerPCBase, ULONG signals, ULONG mask, ULONG flag)
{
    return 0;
}

PPCFUNCTION VOID myObtainSemaphoreSharedPPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return;
}

PPCFUNCTION LONG myAttemptSemaphoreSharedPPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return 0;
}

PPCFUNCTION VOID myProcurePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC, struct SemaphoreMessage* SemaphoreMessage)
{
    return;
}

PPCFUNCTION VOID myVacatePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC, struct SemaphoreMessage* SemaphoreMessage)
{
    return;
}

PPCFUNCTION VOID myCauseInterrupt(struct PrivatePPCBase* PowerPCBase)
{
    return;
}

PPCFUNCTION APTR myCreatePoolPPC(struct PrivatePPCBase* PowerPCBase, ULONG flags, ULONG puddle_size, ULONG thres_size)
{
	struct poolHeader* myPoolHeader = 0;

	printDebugEntry(PowerPCBase, function, flags, puddle_size, thres_size, 0);

	if (puddle_size >= thres_size)
	{
		if (myPoolHeader = AllocVec68K(PowerPCBase, sizeof(struct poolHeader), flags))
		{
			puddle_size = (puddle_size + 31) & -32;
			myNewListPPC(PowerPCBase, (struct List*)&myPoolHeader->ph_PuddleList);
			myNewListPPC(PowerPCBase, (struct List*)&myPoolHeader->ph_BlockList);

			myPoolHeader->ph_requirements = flags;
			myPoolHeader->ph_PuddleSize = puddle_size;
			myPoolHeader->ph_ThresholdSize = thres_size;

			myAddHeadPPC(PowerPCBase, &PowerPCBase->pp_ThisPPCProc->tp_TaskPools, (struct Node*)&myPoolHeader->ph_Node);
		}
	}
	printDebugExit(PowerPCBase, function, (ULONG)myPoolHeader);
	return myPoolHeader;
}

PPCFUNCTION VOID myDeletePoolPPC(struct PrivatePPCBase* PowerPCBase, APTR poolheader)
{
    return;
}

PPCFUNCTION APTR myAllocPooledPPC(struct PrivatePPCBase* PowerPCBase, APTR poolheader, ULONG size)
{
    return NULL;
}

PPCFUNCTION VOID myFreePooledPPC(struct PrivatePPCBase* PowerPCBase, APTR poolheader, APTR ptr, ULONG size)
{
    return;
}

PPCFUNCTION APTR myRawDoFmtPPC(struct PrivatePPCBase* PowerPCBase, STRPTR formatstring, APTR datastream, void (*putchproc)(), APTR putchdata)
{
    return NULL;
}

PPCFUNCTION LONG myPutPublicMsgPPC(struct PrivatePPCBase* PowerPCBase, STRPTR portname, struct Message* message)
{
    return 0;
}

PPCFUNCTION LONG myAddUniquePortPPC(struct PrivatePPCBase* PowerPCBase, struct MsgPortPPC* port)
{
    return 0;
}

PPCFUNCTION LONG myAddUniqueSemaphorePPC(struct PrivatePPCBase* PowerPCBase, struct SignalSemaphorePPC* SemaphorePPC)
{
    return 0;
}

PPCFUNCTION BOOL myIsExceptionMode(struct PrivatePPCBase* PowerPCBase)
{
    return FALSE;
}

PPCFUNCTION VOID myAllocPrivateMem(void)
{
    return;
}

PPCFUNCTION VOID myFreePrivateMem(void)
{
    return;
}

PPCFUNCTION VOID myResetPPC(void)
{
    return;
}

PPCFUNCTION BOOL myChangeStack(struct PrivatePPCBase* PowerPCBase, ULONG NewStackSize)
{
    return FALSE;
}

PPCFUNCTION ULONG myRun68KLowLevel(struct PrivatePPCBase* PowerPCBase, ULONG Code, LONG Offset, ULONG a0,
                                 ULONG a1, ULONG d0, ULONG d1)
{
    return 0;
}
