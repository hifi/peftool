/*
 *  Copyright (C) 2019 Toni Spets <toni.spets@iki.fi>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <byteswap.h>

enum {
    PPC_FAULT_NONE  = 0,
    PPC_FAULT_EXIT  = 1,
    PPC_FAULT_INST  = 2,
    PPC_FAULT_MEM   = 3
};

typedef struct {
    // registers
    uint32_t r[32];
    double fr[32];
    uint32_t lr, ctr;
    uint32_t cr, xer;
    uint32_t fpscr;
    uint32_t pc;

    // RAM accessible by the CPU
    void *ram;
    uint32_t ram_size;

    // fault code if execution cannot continue
    uint32_t fault;
} emul_ppc_state;

#define PPC_PTR_INT(cpu, addr) (uint32_t *)((uint8_t *)(cpu)->ram + (addr))

#define PPC_ARG_INT(cpu, i) (int32_t)cpu->r[2 + i]
#define PPC_ARG_SHORT(cpu, i) (int16_t)cpu->r[2 + i]
#define PPC_ARG_PTR(cpu, i) (void *)((uint8_t *)cpu->ram + cpu->r[2 + i])
#define PPC_RETURN_INT(cpu, i) cpu->r[3] = (int32_t)i; return;

// FIXME: swap only when host is little endian
#define PPC_INT64 bswap_64
#define PPC_INT bswap_32
#define PPC_SHORT bswap_16

void emul_ppc_init(emul_ppc_state *cpu);
int emul_ppc_run(emul_ppc_state *cpu, int step);
void emul_ppc_dump(emul_ppc_state *cpu);
