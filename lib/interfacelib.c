#include <stdio.h>
#include "../src/emul_ppc.h"

#pragma pack(push, 1)
typedef struct {
    uint16_t environsVersion;
    uint16_t machineType;
    uint16_t systemVersion;
    uint16_t processor;
    uint8_t hasFPU;
    uint8_t hasColorQD;
    uint16_t keyBoardType;
    uint16_t atDrvrVersNum;
    uint16_t sysVRefNum;
} SysEnvRec;

typedef struct {
    uint8_t privates[76];
    uint32_t randSeed;
    /* wip */
} QDGlobals;

#pragma pack(pop)

void emul_GetApplLimit(emul_ppc_state *cpu)
{
    cpu->r[3] = 0x0130;
    printf("GetApplLimit() -> %08X\n", cpu->r[3]);
}

void emul_SetApplLimit(emul_ppc_state *cpu)
{
    printf("SetApplLimit(%08X)\n", cpu->r[3]);
}

void emul_MaxApplZone(emul_ppc_state *cpu)
{
    printf("MaxApplZone()\n");
}

void emul_MoreMasters(emul_ppc_state *cpu)
{
    printf("MoreMasters()\n");
}

void emul_SysEnvirons(emul_ppc_state *cpu)
{
    uint32_t versionRequested = PPC_ARG_UINT(cpu, 1);
    SysEnvRec *theWorld = PPC_ARG_PTR(cpu, 2);

    printf("SysEnvirons(versionRequested=%d, theWorld=%p [0x%08X])\n", versionRequested, (void *)theWorld, PPC_ARG_UINT(cpu, 2));

    theWorld->environsVersion = 0xFFFF;
    theWorld->machineType = 0xFFFF;
    theWorld->systemVersion = 0xFFFF;
    theWorld->processor = 0xFFFF;
    theWorld->hasFPU = 0;
    theWorld->hasColorQD = 1;

    PPC_RETURN_UINT(cpu, 0);
}
