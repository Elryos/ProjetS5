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

#define MASK_RM 0b1111 << 0
#define MASK_BY_REGISTER 0b1 << 4
#define MASK_SHIFT 0b11 << 5
#define MASK_RS_IMMEDIATE 0b11111 << 7
#define MASK_RS_REGISTER 0b1111 << 8

#define MASK_COND 0b1111 << 28
#define MASK_N 0b1 << 31
#define MASK_Z 0b1 << 30
#define MASK_C 0b1 << 29
#define MASK_V 0b1 << 28
#define MASK_TYPE 0b111 << 25


static int arm_execute_instruction(arm_core p) {
    
    uint32_t ins;
    uint32_t cpsr = arm_read_cpsr(p);
    uint8_t n = (cpsr & MASK_N )>> 31;
    uint8_t z = (cpsr & MASK_Z )>> 30;
    uint8_t c = (cpsr & MASK_C )>> 29;
    uint8_t v = (cpsr & MASK_V )>> 28;

    int result = arm_fetch(p, &ins);
    if (result) return result;

//------------------------------------------------------- Test des differentes conditions
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
    		if (!v) return 0;
    		break;
    	case (VC) :
    		if (v) return 0;
    		break;
    	case (HI) :
    		if (!(c && !z)) return 0;
    		break;
    	case (LS) :
    		if (!(!c || z)) return 0;
    		break;
    	case (GE) :
    		if (n!=v) return 0;
    		break;
    	case (LT) :
    		if (n==v) return 0;
    		break;
    	case (GT) :
    		if (!(!z && (n==v))) return 0;
    		break;
    	case (LE) :
    		if (!(z || n!=v)) return 0;
    		break;
    	case (AL) :
    		break;
    	default : 
    		break;
    }

//------------------------------------------------------- Test du type d'instructions
    switch ((ins & MASK_TYPE) >> 25) {
    	case(DATA_PROCESSING_SHIFT) :
    		// CAS PARTICULIER LDRH, STRH
    		if ((ins >> 4 & 1) && (ins >> 7 & 1)) {
    			arm_load_store(p,ins);
            } else {
                arm_data_processing(p, ins);
            }
    		break;
    	case(DATA_PROCESSING_IMMEDIATE)	:                  //Immediat et shift testés à l'intérieur de data_processing
    		arm_data_processing(p, ins);
    		break;
        case(LOAD_STORE_IMMEDIATE) :                       //Immediat et shift  testés à l'intérieur de load_store
            arm_load_store(p, ins);
            break;
    	case(LOAD_STORE_REGISTER) :
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

//------------------------------------------------------- Fonction pour chaque étape
int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}

//------------------------------------------------------- Addressing Mode 1
uint32_t shifter_operand(arm_core p, uint32_t ins, uint8_t * carry_out) {
    uint32_t Rm = arm_read_register(p, (ins & MASK_RM) >> 0);
    uint32_t shift;

    if ((ins & MASK_BY_REGISTER) >> 4) {
        shift = arm_read_register(p, (ins & MASK_RS_REGISTER) >> 8);
    } else {
        shift = (ins & MASK_RS_IMMEDIATE) >> 7;
    }

    // A shift by 32 is encoded by shift_imm == 0 (for LSL and ASR)
    if (!shift && get_bit(ins,6) && get_bit(ins,4)) shift = 32;
    
    if (!shift) *carry_out = get_bit(arm_read_cpsr(p), C);


    switch ((ins & MASK_SHIFT) >> 5) {
        case (LSL) :
            if (shift) *carry_out = get_bit(Rm, 32 - shift);
            return Rm << shift;
        case (LSR) :
            if (shift) *carry_out = get_bit(Rm, shift - 1);
            return Rm >> shift;
        case (ASR) :
            *carry_out = get_bit(Rm, (int32_t) shift - 1);
            return asr(Rm, (int32_t) shift - 1);
        case (ROR) :
            if (shift) {
                *carry_out = get_bit(Rm, shift - 1);
                return ror(Rm, shift);
            } else {
                /*RRX*/
                *carry_out = Rm & 1;
                return Rm >> 1 | ((get_bit(arm_read_cpsr(p), C) << 31));
            }
        default :
            return 0;
    }
}