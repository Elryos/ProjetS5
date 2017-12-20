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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_data_processing.h"
#include "arm_instruction.h"
#include "util.h"
#include "debug.h"

#define LS 0b1 <<20
#define BYTE 0b1 <<22
#define IMM_REG 0b1 <<22
#define HALF 0b1001 <<4
#define ADR_MOD 0b11001 <<21
#define U 0b1 <<23
#define Rn 0b1111 <<16
#define Rd 0b1111 <<12
#define offset_12 0xFFF
#define IMMH 0b1111 <<8
#define IMML 0b1111
#define Rm 0b1111

#define IMM 0b0
#define REG_SCA 0b1


int arm_LDR_STR (arm_core p,uint32_t ins){
	uint32_t address;

	if (ins & U >> 23){
		if((ins & ADR_MOD >> 25) == IMM){
			address = arm_read_register(p,ins & Rn >>20) + (ins & offset_12);
		}
		else{
			address = arm_read_register(p,ins & Rn >> 20) + shifter_operand(p,ins);
		}
	}
	else{
		if((ins & ADR_MOD >> 25) == IMM){
			address = arm_read_register(p,ins & Rn >>20) - (ins & offset_12);
		}
		else{
			address = arm_read_register(p,ins & Rn >> 20) - shifter_operand(p,ins);
		}
	}
	arm_write_register(p,ins & Rn >>16,address);
	if(ins & BYTE >>22){
		uint8_t bvalue;
		if(ins & LS >>20){
			if(arm_read_byte(p,address,&bvalue)){
				arm_write_register(p,ins & Rd >>12,bvalue);
				return 0;
			}
			return 1;
		}
		else{
			bvalue=arm_read_register(p,ins & Rd >>12);
			return arm_write_byte(p,address,bvalue);
		}
	}
	else{
		uint32_t value;
		if (ins & LS >>20){
			if (arm_read_word(p,address,&value)){
				arm_write_register(p,ins & Rd >>12,value);
				return 0;
			}
			return 1;
		}
		else{
			value = arm_read_register(p,ins & Rd >>12);
			return arm_write_word(p,address,value);
		}
	}		
}

int arm_LDRH_STRH (arm_core p,uint32_t ins){
	uint32_t address;
	if (ins & U >> 23){
		if(ins & IMM_REG >> 22){
			address = arm_read_register(p,ins & Rn >> 20) + (ins & IMMH >> 4) + (ins & IMML);
		}
		else{
			address = arm_read_register(p,ins & Rn >> 20) + arm_read_register(p,ins & Rm);
		}
	}
	else{
		if (ins & IMM_REG >> 22){
			address = arm_read_register(p,ins & Rn >> 20) - (ins & IMMH >> 4) + (ins & IMML);
		}
		else{
			address = arm_read_register(p,ins & Rn >> 20) - arm_read_register(p,ins & Rm);
		}
	}
	arm_write_register(p,ins & Rn >>16,address);
	uint16_t value;
	if (ins & LS >> 20){
		if (arm_read_half(p,address,&value)){
			arm_write_register(p,ins & Rd >>12,value);
			return 0;
		}
		return 1;
	}
	else {
		value = arm_read_register(p,ins & Rd >>12);
		return arm_write_half(p,address,value);
	}
}

int arm_load_store(arm_core p, uint32_t ins) {	
	int res;
	if ((ins & HALF >> 4) == 0b1001){
		res=arm_LDRH_STRH(p,ins);
	}
	else{
		res=arm_LDR_STR(p,ins);
	}
    return res;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {

    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}

