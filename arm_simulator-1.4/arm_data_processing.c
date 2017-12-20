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
#include "arm_instruction.h"
#include "util.h"
#include "debug.h"


#define MASK_I 0b1 << 25
#define MASK_C 0b1 << 29


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


uint32_t change_bit(uint32_t s, uint8_t n, uint8_t v) {
	return (s & ~(1 << n)) | (1 << n);
}

void flags_update(arm_core p, uint64_t res) {
	uint32_t cpsr = arm_read_cpsr(p);
	
	change_bit(cpsr, N, res >> 32 & 1);
	change_bit(cpsr, Z, res==0);
	change_bit(cpsr, C, res >> 31 & 1);
	change_bit(cpsr, V, (res >> 31 & 1) != (res >> 31 & 1));

	arm_write_cpsr(p, cpsr);

}

/* Decoding functions for different classes of instructions */
int arm_data_processing(arm_core p, uint32_t ins) {
    uint32_t Value_Rn = arm_read_register(p, ins & MASK_RN >> 16);
    uint8_t Rd = ins & MASK_RD >> 12;
    uint32_t Value_Shifter;
    if (ins & MASK_I >> 25) {
    	Value_Shifter = ror(ins & 0xFF, (ins >> 8 & 0xF) * 2);
    } else {
    	Value_Shifter = shifter_operand(p, ins);
    }

    uint32_t cpsr = arm_read_cpsr(p);
    uint8_t c = cpsr & MASK_C >> 29;
    uint64_t Res;

    switch (ins & MASK_OPCODE >> 25) { 
    	case (AND) :
    		arm_write_register(p, Rd, Value_Rn & Value_Shifter);
    		break;
    	case (EOR) :
    		arm_write_register(p, Rd, Value_Rn ^ Value_Shifter); 
    		break;
    	case (SUB) :
    		Res = Value_Rn - Value_Shifter ; 
           	arm_write_register(p,Rd,Res);
    		break;
    	case (RSB) :
    		Res = Value_Shifter - Value_Rn ; 
           	arm_write_register(p,Rd,Res);
    		break;
    	case (ADD) :
            Res = Value_Rn + Value_Shifter ; 
           	arm_write_register(p,Rd,Res);
    		break;
    	case (ADC) :
    		Res = Value_Rn + Value_Shifter + c;
    		arm_write_register(p,Rd,Res);
    		break;
    	case (SBC) :
    		Res = Value_Rn - Value_Shifter - !(c);
    		arm_write_register(p,Rd,Res);
    		break;
    	case (RSC) :
    		Res = Value_Shifter - Value_Rn - !(c); 
    		arm_write_register(p,Rd,Res);
    		break;
    	case (TST) : /*S forcement � 1 ici*/
    		/*Necessite maj des flags*/
    		Res = Value_Rn & Value_Shifter;
    		break;
    	case (TEQ) : /*SAME*/
    		Res = Value_Rn ^ Value_Shifter;
    		break;
    	case (CMP) : /*SAME*/
    		Res = Value_Rn - Value_Shifter;
    		break;
    	case (CMN) : /*SAME*/
    		Res = Value_Rn + Value_Shifter;
    		break;
    	case (ORR) :
    		Res = Value_Rn | Value_Shifter;
    		arm_write_register(p,Rd,Res);
    		break;
    	case (MOV) :
    		Res = Value_Shifter;
    		arm_write_register(p,Rd,Value_Shifter);
    		break;
    	case (BIC) :
    		Res = Value_Rn & ~(Value_Shifter) ;
    		arm_write_register(p,Rd,Res);
    		break;
    	case (MVN) :
    		Res = ~(Value_Shifter);
    		arm_write_register(p,Rd,Res);
    		break;
    	default :
    		break;
    }

    if (ins & MASK_STATUS >> 20) {
    	flags_update(p, Res);
    }
    return 0;
}

