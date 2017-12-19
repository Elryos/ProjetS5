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

#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

#define MASK_RM 0b1111 << 0
#define MASK_IMMEDIATE 0b1 << 4
#define MASK_SHIFT 0b11 << 5
#define MASK_RS_IMMEDIATE 0b11111 << 7
#define MASK_RS_REGISTER 0b1111 << 8

#define MASK_OPCODE 0b1111 << 25
#define MASK_RN 0b1111 << 16
#define MASK_RD 0b1111 << 12
#define MASK_STATUS 0b1 << 20

#define AND 0b0000
#define EOR 0b0001
#define SUB 0b0010
#define RSB 0b0011
#define ADD 0b0100
#define ADC 0b0101
#define SBC 0b0110
#define RSC 0b0111
#define TST 0b1000
#define TEQ 0b1001 
#define CMP 0b1010 
#define CMN 0b1011
#define ORR 0b1100
#define MOV 0b1101
#define BIC 0b1110
#define MVN 0b1111


uint32_t shifter_operand(arm_core p, uint32_t ins) {
	uint32_t Rm = arm_read_register(p, ins & MASK_RM >> 0);
	uint32_t Rs;

	if (ins & MASK_IMMEDIATE >> 4) {
		Rs = ins & MASK_RS_IMMEDIATE >> 7;
	} else {
		Rs = arm_read_register(p, ins & MASK_RS_REGISTER >> 8);
	}

	switch (ins & MASK_SHIFT >> 5) {
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

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    uint32_t Rn = arm_read_register(p, ins & MASK_RN >> 16);
    uint8_t Rd = ins & MASK_RD >> 12;
    uint32_t Rm = shifter_operand(p, ins);

    switch (ins & MASK_OPCODE >> 25) {
    	case (AND) :
    		arm_write_register(p, Rd, Rn & Rm);
    		break;
    	case (EOR) :
    		arm_write_register(p, Rd, Rn | Rm);
    		break;
    	case (SUB) :
    		break;
    	case (RSB) :
    		break;
    	case (ADD) :
    		break;
    	case (ADC) :
    		break;
    	case (SBC) :
    		break;
    	case (RSC) :
    		break;
    	case (TST) :
    		break;
    	case (TEQ) :
    		break;
    	case (CMP) :
    		break;
    	case (CMN) :
    		break;
    	case (ORR) :
    		break;
    	case (MOV) :
    		break;
    	case (BIC) :
    		break;
    	case (MVN) :
    		break;
    	default :
    		break;
    }

    if (ins & MASK_STATUS >> 20) {
    	// arm_write_cpsr(p, ???);
    }
    return 0;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}