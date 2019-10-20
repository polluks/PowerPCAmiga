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


#include <libraries/mediatorpci.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/mediatorpci.h>
#include <proto/expansion.h>

#include <exec/resident.h>
#include <exec/libraries.h>
#include <exec/execbase.h>

#include <dos/var.h>

#include <powerpc/powerpc.h>
#include <intuition/intuition.h>
#include "librev.h"
#include "constants.h"
#include "libstructs.h"
#include "internals68k.h"


int main (void)
{
    return -1;
}

static const ULONG cardList[] =
{
    DEVICE_MPC107<<16|VENDOR_MOTOROLA,
    DEVICE_HARRIER<<16|VENDOR_MOTOROLA,
    DEVICE_MPC8343E<<16|VENDOR_FREESCALE,
    0
};

static const ULONG atiList[] =
{
    DEVICE_RV280PRO,
    DEVICE_RV280,
    DEVICE_RV280_2,
    DEVICE_RV280MOB,
    DEVICE_RV280SE,
    0
};

static void CleanUp(struct InternalConsts *myConsts)
{
    struct PciBase *MediatorPCIBase;
    struct DosLibrary *DOSBase;
    struct ExpansionBase *ExpansionBase;
    struct ExecBase *SysBase = myConsts->ic_SysBase;

#if 0
    if (nameSpace)
    {
        FreeVec(nameSpace);
    }
#endif
    if (MediatorPCIBase = myConsts->ic_PciBase)
    {
        CloseLibrary((struct Library*)MediatorPCIBase);
    }

    if (DOSBase = myConsts->ic_DOSBase)
    {
        CloseLibrary((struct Library*)DOSBase);
    }

    if (ExpansionBase = myConsts->ic_ExpansionBase)
    {
        CloseLibrary((struct Library*)ExpansionBase);
    }

    return;
}

struct PPCBase *mymakeLibrary(struct InternalConsts *myConsts)
{
    struct PPCBase *PowerPCBase = NULL;
    struct ExecBase *SysBase = myConsts->ic_SysBase;

    CacheClearU();

    return PowerPCBase;
}

__entry struct PPCBase *LibInit(__reg("d0") struct PPCBase *ppcbase,
              __reg("a0") BPTR seglist, __reg("a6") struct ExecBase* __sys)
{
    struct ExecBase *SysBase;
    struct DosLibrary *DOSBase;
    struct UtilityBase *UtilityBase;
    struct PciBase *MediatorPCIBase;
    struct ExpansionBase *ExpansionBase;
    struct PPCBase *PowerPCBase;
    ULONG medflag   = 0;
    ULONG gfxisati  = 0;
    ULONG gfxnotv45 = 0;
    struct ConfigDev *cd = NULL;
    struct PciDevice *ppcdevice = NULL;
    struct PciDevice *gfxdevice = NULL;
    struct MemHeader *pcimemDMAnode;
    ULONG cardNumber = 0;
    BYTE memPrio;
    ULONG card, devfuncnum, res, i, n, testLen;
    ULONG testSize, bytesFree;
    volatile ULONG status;
    struct PPCZeroPage *myZeroPage;
    struct MemHeader *myPPCMemHeader;
    struct InitData *cardData;
    APTR nameSpace = NULL;

    struct InternalConsts consts;
    struct InternalConsts *myConsts = &consts;

    SysBase = __sys;
    myConsts->ic_SysBase = __sys;
    myConsts->ic_SegList = seglist;

    if (!(SysBase->AttnFlags & (AFF_68040|AFF_68060)))
    {
        PrintCrtErr(myConsts, "This library requires a 68LC040 or better");
        return NULL;
    }

#if 1
    if ((FindName(&SysBase->MemList,"ppc memory")) || (FindName(&SysBase->LibList,LIBNAME)))
    {
        PrintCrtErr(myConsts, "Other PPC library already active (WarpOS/Sonnet");
        return NULL;
    }
#endif

    if (!(DOSBase = (struct DosLibrary*)OpenLibrary("dos.library",37L)))
    {
        PrintCrtErr(myConsts, "Could not open dos.library V37+");
        return NULL;
    }

    myConsts->ic_DOSBase = DOSBase;

    UtilityBase = (struct UtilityBase*)OpenLibrary("utility.library",0L);

    if (!(MediatorPCIBase = (struct PciBase*)OpenLibrary("pci.library",VERPCI)))
    {
        PrintCrtErr(myConsts, "Could not open pci.library V13.8+");
        return NULL;
    }

    myConsts->ic_PciBase = MediatorPCIBase;

#if 1
    if ((MediatorPCIBase->pb_LibNode.lib_Version == VERPCI) && (MediatorPCIBase->pb_LibNode.lib_Revision < REVPCI))
    {
        PrintCrtErr(myConsts, "Could not open pci.library V13.8+");
        return NULL;
    }
#endif

    if (!(ExpansionBase = (struct ExpansionBase*)OpenLibrary("expansion.library",37L)))
    {
        PrintCrtErr(myConsts, "Could not open expansion.library V37+");;
        return NULL;
    }

    myConsts->ic_ExpansionBase = ExpansionBase;

    if (FindConfigDev(NULL, VENDOR_ELBOX, MEDIATOR_MKII))
    {
        if (!(cd = FindConfigDev(NULL, VENDOR_ELBOX, MEDIATOR_LOGIC)))
        {
            PrintCrtErr(myConsts, "Could not find a supported Mediator board");
            return NULL;
        }
        medflag = 1;
    }

    else if (FindConfigDev(NULL, VENDOR_ELBOX, MEDIATOR_1200TX))
    {
        if (!(FindConfigDev(NULL, VENDOR_ELBOX, MEDIATOR_1200LOGIC)))
        {
            PrintCrtErr(myConsts, "Could not find a supported Mediator board");
            return NULL;
        }
    }

    if (cd)
    {
        if (!(cd->cd_BoardSize == 0x20000000))
        {
            PrintCrtErr(myConsts, "Mediator WindowSize jumper incorrectly configured");
            return NULL;
        }
    }

    for (i=0; i<MAX_PCI_SLOTS; i++)
    {
        devfuncnum = i<<DEVICENUMBER_SHIFT;
        res = ReadConfigurationLong((UWORD)devfuncnum, PCI_OFFSET_ID);

        while (card = cardList[cardNumber])
        {
            if (card == res)
            {
                break;
            }
            cardNumber += 1;
        }
        if (card == res)
        {
            break;
        }
        cardNumber = 0;
    }

    if (!(card))
    {
        PrintCrtErr(myConsts, "No supported PPC PCI bridge detected");
        return NULL;
    }

    ppcdevice = FindPciDevFunc(devfuncnum);
    cardNumber = 0;

    if (!(gfxdevice = FindPciDevice(VENDOR_3DFX, DEVICE_VOODOO45, 0)))
    {
        gfxdevice = FindPciDevice(VENDOR_3DFX, DEVICE_VOODOO3, 0);
        gfxnotv45 = 1;
    }

    if (!(gfxdevice))
    {
        while (atiList[cardNumber])
        {
            if (gfxdevice = FindPciDevice(VENDOR_ATI, atiList[cardNumber], 0))
            {
                gfxisati = 1;
                break;
            }
            if (gfxdevice)
            {
                break;
            }
        cardNumber += 1;
        }
    }

    if (!gfxdevice)
    {
        PrintCrtErr(myConsts, "No supported VGA card detected");
        return NULL;
    }

    if (gfxisati)
    {
        myConsts->ic_gfxMem = gfxdevice->pd_ABaseAddress0;
        myConsts->ic_gfxSize = -((gfxdevice->pd_Size0)&-16L);
        myConsts->ic_gfxConfig = gfxdevice->pd_ABaseAddress2;
        myConsts->ic_gfxType = VENDOR_ATI;
    }
    else
    {
        myConsts->ic_gfxMem = gfxdevice->pd_ABaseAddress2;
        myConsts->ic_gfxSize = -(((gfxdevice->pd_Size2)<<1)&-16L);
        myConsts->ic_gfxConfig = 0;
        myConsts->ic_gfxType = VENDOR_3DFX;
        if (gfxnotv45)
        {
            myConsts->ic_gfxSubType = DEVICE_VOODOO3;
        }
        else
        {
            myConsts->ic_gfxSubType = DEVICE_VOODOO45;
        }
    }

    if ((!(medflag)) && (myConsts->ic_gfxMem >>31))
    {
        PrintCrtErr(myConsts, "PPCPCI environment not set in ENVARC:Mediator");
    }

    getENVs(myConsts);

    //Add ATI Radeon memory as extra memory for K1/M1
    //Need to have Voodoo as primary VGA output
    //As the list has twice the memory name, we search it twice
    //Findname only returns first in the list

    myConsts->ic_startBAT = 0;
    myConsts->ic_sizeBAT = 0;

    for (i=0; i<1; i++)
    {
        if (!(pcimemDMAnode = (struct MemHeader *)FindName(&SysBase->MemList, "pcidma memory")))
        {
            break;
        }
        if (!(pcimemDMAnode->mh_Node.ln_Succ))
        {
            break;
        }

        memPrio = -20;

        if (!(gfxisati))
        {
            testSize = 0x2000000;
            testLen = ((ULONG)pcimemDMAnode->mh_Upper)-((ULONG)pcimemDMAnode->mh_Lower);

            for (n=0; n<4; n++)
            {
                if (testLen <= testSize)
                {
                    break;
                }

                testSize = (testSize<<1);
            }

            if (0 < n < 4)
            {
                memPrio = -15;
                myConsts->ic_startBAT = (((ULONG)pcimemDMAnode->mh_Lower) & (-(testSize)));
                myConsts->ic_sizeBAT = testSize;
            }
        }

        Disable();
        Remove((struct Node *)pcimemDMAnode);
        pcimemDMAnode->mh_Node.ln_Pri = memPrio;
        Enqueue(&SysBase->MemList, (struct Node *)pcimemDMAnode);
        Enable();
    }

    switch (ppcdevice->pd_DeviceID)
    {
        case DEVICE_HARRIER:
        {
            cardData = SetupHarrier(myConsts, devfuncnum, ppcdevice);
            break;
        }

        case DEVICE_MPC8343E:
        {
            cardData = SetupKiller(myConsts, devfuncnum, ppcdevice);
            break;
        }

        case DEVICE_MPC107:
        {
            cardData = SetupMPC107(myConsts, devfuncnum, ppcdevice);
            break;
        }

        default:
        {
            PrintCrtErr(myConsts, "Error setting up PPC card");
            return NULL;
        }
    }

    if (!(cardData))
    {
        return NULL;
    }

    for (i=0; i<0xEC0000; i++)
    {
        status = cardData->id_Status;

        switch (status)
        {
            case 0x426f6f6e:        //Boon
            {
                break;
            }
            case 0x45727231:        //Err1
            {
                PrintCrtErr(myConsts, "Error during MMU setup of PPC");
                return NULL;
            }
            case 0x45727232:        //Err2
            {
                PrintCrtErr(myConsts, "No memory detected on the PPC card");
                return NULL;
            }
            case 0x45727233:        //Err3
            {
                PrintCrtErr(myConsts, "Memory corruption detected during setup");
                return NULL;
            }
            case 0x45727234:        //Err4
            {
                PrintCrtErr(myConsts, "General PPC setup error");
                return NULL;
            }
        }
        if (status == 0x426f6f6e)
        {
            break;
        }
    }

    if (i == 0xEC0000)
    {
        switch (status)
        {
            case 0x496e6974:        //Init
            case 0x426f6f6e:        //Boon
            {
                PrintCrtErr(myConsts, "PowerPC CPU possibly crashed during setup");
                return NULL;
            }
#if 0
            case 0xabcdabcd:
            {
                PrintCrtErr(myConsts, "PowerPC CPU not responding");
                return NULL;
            }
#endif
            default:
            {
                PrintCrtErr(myConsts, "PowerPC CPU not responding");
                return NULL;
            }
        }
    }

    myZeroPage = (struct PPCZeroPage*)cardData->id_MemBase;

    nameSpace = AllocVec(16L, MEMF_PUBLIC|MEMF_CLEAR|MEMF_REVERSE);

    if (!(nameSpace))
    {
        PrintCrtErr(myConsts, "General memory allocation error");
        return NULL;
    }

    CopyMemQuick("ppc memory", nameSpace, 16L);

    myPPCMemHeader = (struct MemHeader*)((ULONG)myZeroPage + MEM_GAP);
    myPPCMemHeader->mh_Node.ln_Type = NT_MEMORY;
    myPPCMemHeader->mh_Node.ln_Pri = 1;
    myPPCMemHeader->mh_Node.ln_Name = nameSpace;
    myPPCMemHeader->mh_First = (struct MemChunk*)((ULONG)myPPCMemHeader + sizeof(struct MemHeader));

    bytesFree = (cardData->id_MemSize - myZeroPage->zp_PageTableSize - MEM_GAP - sizeof(struct MemHeader));

    myPPCMemHeader->mh_First->mc_Next = NULL;
    myPPCMemHeader->mh_First->mc_Bytes = bytesFree;

    myPPCMemHeader->mh_Free = bytesFree;
    myPPCMemHeader->mh_Upper = (APTR)((ULONG)myZeroPage + (cardData->id_MemSize) - (myZeroPage->zp_PageTableSize));
    myPPCMemHeader->mh_Lower = (APTR)((ULONG)myPPCMemHeader + sizeof(struct MemHeader));
    myPPCMemHeader->mh_Attributes = MEMF_PUBLIC|MEMF_FAST|MEMF_PPC;


    Disable();

    Enqueue(&SysBase->MemList, (struct Node*)myPPCMemHeader);

    PowerPCBase = mymakeLibrary(myConsts);

    Enable();

    PrintError(myConsts, "Test completed!");

    CleanUp(myConsts);

    return PowerPCBase;
}

static const struct Resident RomTag =
{
    RTC_MATCHWORD,
    (struct Resident *) &RomTag,
    (struct Resident *) &RomTag+1,
    RTW_NEVER,
    VERSION,
    NT_LIBRARY,
    LIB_PRIORITY,
    LIBNAME,
    VSTRING,
    (APTR)&LibInit
};


/*
static const APTR LibVectors[] =
{
    LibOpen,
    LibClose,
    LibExpunge,
    LibReserved,
    (APTR) -1
};

*/

ULONG doENV(struct InternalConsts* myConsts, UBYTE* envstring)
{
    LONG res;
    ULONG buffer;
    struct DosLibrary *DOSBase = myConsts->ic_DOSBase;

    res = GetVar((CONST_STRPTR)envstring, (STRPTR)&buffer, sizeof(ULONG), GVF_GLOBAL_ONLY);

    if (res < 0)
    {
        buffer = 0;
    }

    return buffer;
}

void getENVs(struct InternalConsts* myConsts)
{
    myConsts->ic_env1 = 0;
    myConsts->ic_env2 = 0;
    myConsts->ic_env3 = 0;

    myConsts->ic_env1 |= (doENV(myConsts,"sonnet/EnEDOMem") << 24);
    myConsts->ic_env1 |= (doENV(myConsts,"sonnet/Debug") << 16);
    myConsts->ic_env1 |= (doENV(myConsts,"sonnet/EnAlignExc") << 8);
    myConsts->ic_env1 |= doENV(myConsts,"sonnet/DisL2Cache");

    myConsts->ic_env2 |= (doENV(myConsts,"sonnet/DisL2Flush") <<24);
    myConsts->ic_env2 |= (doENV(myConsts,"sonnet/EnPageSeup") << 16);
    myConsts->ic_env2 |= (doENV(myConsts,"sonnet/EnDAccessExc") << 8);
    myConsts->ic_env2 |= doENV(myConsts,"sonnet/DisHunkPatch");

    myConsts->ic_env3 |= (doENV(myConsts,"sonnet/SetCMemDiv") <<24);
    myConsts->ic_env3 |= (doENV(myConsts,"sonnet/SetCPUComm") << 16);
    myConsts->ic_env3 |= (doENV(myConsts,"sonnet/EnStackPatch") << 8);

    return;
}

ULONG readmemLong(ULONG Base, ULONG offset)
{
    ULONG res;
    res = *((ULONG*)(Base + offset));
    return res;
}

void resetKiller(struct InternalConsts* myConsts, ULONG configBase, ULONG ppcmemBase)
{
    ULONG res;

    struct ExecBase* SysBase = myConsts->ic_SysBase;

    CacheClearU();

    writememLong(configBase, IMMR_RPR, 0x52535445); //"RSTE"

    res = readmemLong(configBase, IMMR_RCER);

    writememLong(configBase, IMMR_RCR, res);

    for (res=0; res<0x10000; res++);

    writememLong(ppcmemBase, 0x1f0, 0); //debugdebug??

    res = readmemLong(configBase, IMMR_RSR);

    writememLong(configBase, IMMR_RSR, res);

    return;
}

void writememLong(ULONG Base, ULONG offset, ULONG value)
{
    *((ULONG*)(Base + offset)) = value;
    return;
}

struct InitData* SetupKiller(struct InternalConsts* myConsts,
                             ULONG devfuncnum, struct PciDevice* ppcdevice)
{
    UWORD res;
    ULONG ppcmemBase, configBase, fakememBase, vgamemBase, winSize, startAddress;
    ULONG mySegSize;
    ULONG mySegPointer;
    struct InitData* killerData;

    struct PciBase* MediatorPCIBase = myConsts->ic_PciBase;
    struct ExecBase* SysBase = myConsts->ic_SysBase;

    res = (ReadConfigurationWord(devfuncnum, PCI_OFFSET_COMMAND)
          | BUSMASTER_ENABLE);

    WriteConfigurationWord(devfuncnum, PCI_OFFSET_COMMAND, res);

    if (!(ppcmemBase = AllocPCIBAR(0xfc, 1, ppcdevice)))
    {
        PrintCrtErr(myConsts, "Could not allocate sufficient PCI memory");
        return FALSE;
    }

    configBase = ppcdevice->pd_ABaseAddress0;

    writememLong(configBase, IMMR_PIBAR0, (ppcmemBase >> 12));

    writememLong(configBase, IMMR_PITAR0, 0);
    writememLong(configBase, IMMR_PIWAR0,
                PIWAR_EN|PIWAR_PF|PIWAR_RTT_SNOOP|
                PIWAR_WTT_SNOOP|PIWAR_IWS_64MB);

    writememLong(configBase, IMMR_POCMR0, 0);
    writememLong(configBase, IMMR_POCMR1, 0);
    writememLong(configBase, IMMR_POCMR2, 0);
    writememLong(configBase, IMMR_POCMR3, 0);
    writememLong(configBase, IMMR_POCMR4, 0);
    writememLong(configBase, IMMR_POCMR5, 0);

    vgamemBase = (myConsts->ic_gfxMem & (~(1<25)));

    startAddress = (MediatorPCIBase->pb_MemAddress);
    writememLong(configBase, IMMR_PCILAWBAR1,
                (startAddress+0x60000000));

    winSize = POCMR_EN|POCMR_CM_128MB;

    if (!(myConsts->ic_gfxMem & (~(1<25))))
    {
        if (myConsts->ic_gfxSize & (~(1<28)))
        {
            winSize = POCMR_EN|POCMR_CM_256MB;
        }
        else if (myConsts->ic_gfxSize & (~(1<26)))
        {
            winSize = POCMR_EN|POCMR_CM_64MB;
        }
    }
    writememLong(configBase, IMMR_PCILAWAR1, (LAWAR_EN|LAWAR_512MB));

    if (myConsts->ic_gfxSize & (~(1<27)))
    {
        if (myConsts->ic_gfxMem & (~(1<27)))
        {
            startAddress += myConsts->ic_gfxSize;
        }
    }
    
    writememLong(configBase, IMMR_POBAR0, (startAddress >> 12));
    writememLong(configBase, IMMR_POTAR0, (vgamemBase >> 12));
    writememLong(configBase, IMMR_POCMR0, winSize);

    if (myConsts->ic_gfxConfig)
    {
        writememLong(configBase, IMMR_POBAR2, ((myConsts->ic_gfxConfig + 0x60000000) >> 12));
        writememLong(configBase, IMMR_POTAR2, (myConsts->ic_gfxConfig >> 12));
        writememLong(configBase, IMMR_POCMR2, (POCMR_EN|POCMR_CM_64KB));
    }

    if (myConsts->ic_sizeBAT)
    {

        winSize = POCMR_EN|POCMR_CM_256MB;

        if (!(myConsts->ic_sizeBAT & (~(1<28))))
        {
            winSize = POCMR_EN|POCMR_CM_64MB;
            if (!(myConsts->ic_sizeBAT & (~(1<26))))
            {
                winSize = POCMR_EN|POCMR_CM_128MB;
            }
        }

        writememLong(configBase, IMMR_POBAR1, ((myConsts->ic_startBAT + 0x60000000) >> 12));
        writememLong(configBase, IMMR_POTAR1, (myConsts->ic_startBAT >> 12));
        writememLong(configBase, IMMR_POCMR1, winSize);
    }

    writememLong(ppcmemBase, 0xfc, 0x48000000);
    writememLong(ppcmemBase, 0x100, 0x4bfffffc);

    resetKiller(myConsts, configBase, ppcmemBase);

    fakememBase = ppcmemBase + OFFSET_ZEROPAGE;
    killerData = ((struct InitData*)(fakememBase + OFFSET_KERNEL));

    mySegPointer = (*((ULONG*)(myConsts->ic_SegList << 2)) << 2);
    mySegSize = *((ULONG*)(mySegPointer - 4));

    CopyMemQuick((const APTR)(mySegPointer+4), (APTR)(killerData), mySegSize);

    killerData->id_Status        = 0xabcdabcd;
    killerData->id_MemBase       = ppcmemBase;
    killerData->id_MemSize       = 0x4000000;
    killerData->id_GfxMemBase    = myConsts->ic_gfxMem;
    killerData->id_GfxMemSize    = myConsts->ic_gfxSize;
    killerData->id_GfxType       = myConsts->ic_gfxType;
    killerData->id_GfxSubType    = myConsts->ic_gfxSubType;
    killerData->id_GfxConfigBase = myConsts->ic_gfxConfig;
    killerData->id_Environment1  = myConsts->ic_env1;
    killerData->id_Environment2  = myConsts->ic_env2;
    killerData->id_Environment3  = myConsts->ic_env3;
    killerData->id_StartBat      = myConsts->ic_startBAT;
    killerData->id_SizeBat       = myConsts->ic_sizeBAT;

    writememLong(ppcmemBase, 0x100, 0x48012f00);
    writememLong(configBase, IMMR_IMMRBAR, IMMR_ADDR_DEFAULT);

    resetKiller(myConsts, configBase, ppcmemBase);

    writememLong(configBase, IMMR_SIMSR_L, SIMSR_L_MU);

    return killerData;
}
struct InitData* SetupHarrier(struct InternalConsts* myConsts,
                              ULONG devfuncnum, struct PciDevice* ppcdevice)
{
    return NULL;
}

struct InitData* SetupMPC107(struct InternalConsts* myConsts,
                             ULONG devfuncnum, struct PciDevice* ppcdevice)
{
    return NULL;
}

void PrintCrtErr(struct InternalConsts* myConsts, UBYTE* crterrtext)
{
    PrintError(myConsts, crterrtext);
    CleanUp(myConsts);
    return;
}

void PrintError(struct InternalConsts* myConsts, UBYTE* errortext)
{
    struct IntuitionBase* IntuitionBase;
    struct EasyStruct MyReq =
        {
        (ULONG)sizeof(struct EasyStruct),
        (ULONG)0,
        (UBYTE*)LIBNAME,
        errortext,
        (UBYTE*)"Continue",
        };

    struct ExecBase* SysBase = myConsts->ic_SysBase;

    if (!(IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library",33L)))
    {
        Alert(0x84010000);
        return;
    }

    EasyRequestArgs(NULL, &MyReq, NULL, 0L);

    CloseLibrary((struct Library*)IntuitionBase);

    return;
}

