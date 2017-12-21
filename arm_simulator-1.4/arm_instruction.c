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

#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"

#define MASK_RM 0b1111 << 0
#define MASK_IMMEDIATE 0b1 << 4
#define MASK_SHIFT 0b11 << 5
#define MASK_RS_IMMEDIATE 0b11111 << 7
#define MASK_RS_REGISTER 0b1111 << 8

#define MASK_COND 0b1111 << 28
#define MASK_N 0b1 << 31
#define MASK_Z 0b1 << 30
#define MASK_C 0b1 << 29
#define MASK_V 0b1 << 28
#define MASK_TYPE 0b111 << 25

#define DATA_PROCESSING_SHIFT 0b000
#define DATA_PROCESSING_IMMEDIATE 0b001
#define LOAD_STORE 0b010
#define LOAD_STORE_MULTIPLE 0b100
#define BRANCH 0b101
#define COPROCESSOR_OTHERS_SWI 0b111


static int arm_execute_instruction(arm_core p) {
    
    printf("VOILA\n");
    uint32_t ins;
    uint32_t cpsr = arm_read_cpsr(p);
    uint8_t n = (cpsr & MASK_N )>> 31;
    uint8_t z = (cpsr & MASK_Z )>> 30;
    uint8_t c = (cpsr & MASK_C )>> 29;
    uint8_t v = (cpsr & MASK_V )>> 28;

    if (arm_fetch(p, &ins)) return 1;

    printf("%i", (ins & MASK_COND) >> 28);

    switch ((ins & MASK_COND) >> 28) {
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
    	default : 
    		break;
    }

    printf("%i\n", (ins & MASK_TYPE) >> 25);
    switch ((ins & MASK_TYPE) >> 25) {
    	case(DATA_PROCESSING_SHIFT) :
    		// CAS PARTICULIER LDRH, STRH
    		if ((ins >> 4 & 1) && (ins >> 7 & 1)) {
    			arm_load_store(p,ins);
            } else {
                printf("DATA PROC\n");
                arm_data_processing(p, ins);
            }
    		break;
    	case(DATA_PROCESSING_IMMEDIATE)	:
    		arm_data_processing(p, ins);
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
    		return UNDEFINED_INSTRUCTION;
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


uint32_t shifter_operand(arm_core p, uint32_t ins) {
    uint32_t Rm = arm_read_register(p, (ins & MASK_RM) >> 0);
    uint32_t Rs;

    if ((ins & MASK_IMMEDIATE) >> 4) {
        Rs = (ins & MASK_RS_IMMEDIATE) >> 7;
    } else {
        Rs = arm_read_register(p, (ins & MASK_RS_REGISTER) >> 8);
    }

    switch ((ins & MASK_SHIFT) >> 5) {
        case (LSL) :
            return Rm << Rs;
        case (LSR) :
            return Rm >> Rs;
        case (ASR) :
            return asr(Rm, Rs);
        case (ROR) :
            return ror(Rm, Rs);
        default :
            break;
    }
    return 0;
}