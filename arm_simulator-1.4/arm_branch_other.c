/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/

#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include <stdlib.h>

#define MASK_L 0b1 << 24
#define MASK_TARGET_ADR 0xFFFFFF

int32_t sign_extending(uint32_t x) {
	if (x & 1 << 23) return x | 0x3F000000;
	return x;
}

int arm_branch(arm_core p, uint32_t ins) {

    if (ins & MASK_L) {
        arm_write_register(p, 14, arm_read_register(p, 15)-4);
    }

    arm_write_register(p, 15, arm_read_register(p,15) + (sign_extending(ins & MASK_TARGET_ADR) << 2));
    return 0;
   }

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        /* Here we implement the end of the simulation as swi 0x123456 */
        if ((ins & 0xFFFFFF) == 0x123456)
            exit(0);
        return SOFTWARE_INTERRUPT;
    } 
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
    // uint32_t val, field_mask;
    
    if (get_bit(ins, 21)) {
        // //MSR
        // if (get_bit(25))
        //     val = ror(ins & 0xFF, ((ins >> 8) & 0xF) * 2);
        // else 
        //     val = arm_read_register(p, get_bits(ins,3,0));


    } else {
        //MRS
        if (get_bit(ins,22)) {
            if (!arm_current_mode_has_spsr(p)) return UNDEFINED_INSTRUCTION;
            arm_write_register(p, get_bits(ins,15,12), arm_read_spsr(p));
        } else {
            arm_write_register(p, get_bits(ins,15,12), arm_read_cpsr(p));
        }
    }

    return 0;
}