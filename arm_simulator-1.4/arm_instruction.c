/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/

#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"

#define MASK_COND 0b1111 << 28
#define MASK_N 0b1 << 31
#define MASK_Z 0b1 << 30
#define MASK_C 0b1 << 29
#define MASK_V 0b1 << 28
#define MASK_TYPE 0b111 << 25

#define EQ 0b0000	// Equal / equals zero	Z
#define NE 0b0001	// Not equal	!Z
#define CS 0b0010   // / HS	Carry set / unsigned higher or same	C
#define CC 0b0011   // / LO	Carry clear / unsigned lower	!C
#define MI 0b0100	// Minus / negative	N
#define PL 0b0101   // Plus / positive or zero	!N
#define VS 0b0110   // Overflow	V
#define VC 0b0111   // No overflow	!V
#define HI 0b1000	// Unsigned higher	C and !Z
#define LS 0b1001	// Unsigned lower or same	!C or Z
#define GE 0b1010	// Signed greater than or equal	N == V
#define LT 0b1011	// Signed less than	N != V
#define GT 0b1100	// Signed greater than	!Z and (N == V)
#define LE 0b1101   // Signed less than or equal	Z or (N != V)
#define AL 0b1110   // Always (default)	any
#define ARM_COPROCESSOR_LOAD_STORE 0b1111

#define DATA_PROCESSING_SHIFT 0b000
#define DATA_PROCESSING_IMMEDIATE 0b001
#define LOAD_STORE 0b010
#define LOAD_STORE_MULTIPLE 0b100
#define BRANCH 0b101
#define COPROCESSOR_OTHERS_SWI 0b111

static int arm_execute_instruction(arm_core p) {
    uint32_t ins;
    uint32_t cpsr = arm_read_cpsr(p);
    uint8_t n = cpsr & MASK_N >> 31;
    uint8_t z = cpsr & MASK_Z >> 30;
    uint8_t c = cpsr & MASK_C >> 29;
    uint8_t v = cpsr & MASK_V >> 28;

    if (arm_fetch(p, &ins)) return 1;
    
    switch (ins && MASK_COND >> 28) {
    	case (EQ) :
    		if (!z) return 0;
    		break;
    	case (NE) :
    		if (z) return 0;
    		break;
    	case (CS) :
    		if (!c) return 0;
    		break;
    	case (CC) :
    		if (c) return 0;
    		break;
    	case (MI) :
    		if (!n) return 0;
    		break;
    	case (PL) :
    		if (n) return 0;
    		break;
    	case (VS) :
    		if (!z) return 0;
    		break;
    	case (VC) :
    		if (z) return 0;
    		break;
    	case (HI) :
    		if (!(c && !z)) return 0;
    		break;
    	case (LS) :
    		if (!(!c && z)) return 0;
    		break;
    	case (GE) :
    		if (n!=v) return 0;
    		break;
    	case (LT) :
    		if (n==v) return 0;
    		break;
    	case (GT) :
    		if (!(z && (n==v))) return 0;
    		break;
    	case (LE) :
    		if (!(z || n!=v)) return 0;
    		break;
    	case (AL) :
    		break;
    	case (ARM_COPROCESSOR_LOAD_STORE) :
    		arm_miscellaneous(p, ins);
    		return 0;
    	default : 
    		return 2;
    		break;
    }


    switch (ins & MASK_TYPE >> 25) {
    	case(DATA_PROCESSING_SHIFT) :
    		// CAS PARTICULIER LDRH, STRH
    		if ((ins >> 4 & 1) && (ins >> 7 & 1)) {
    			arm_load_store(p,ins);
    		} else {
    			arm_data_processing_shift(p, ins);
    		}
    		break;
    	case(DATA_PROCESSING_IMMEDIATE)	:
    		arm_data_processing_immediate_msr(p, ins);
    		break;
    	case(LOAD_STORE) :
    		arm_load_store(p, ins);
    		break;
    	case(LOAD_STORE_MULTIPLE) : 
    		arm_load_store_multiple(p, ins);
    		break;
    	case(BRANCH) :
    		arm_branch(p, ins);
    		break;
    	case(COPROCESSOR_OTHERS_SWI) :
    		arm_coprocessor_others_swi(p, ins);
    		break;
    	default :
    		return 3;
    		break;
    }
    return 0;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
