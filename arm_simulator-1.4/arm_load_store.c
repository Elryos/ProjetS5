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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_data_processing.h"
#include "arm_instruction.h"
#include "util.h"
#include "debug.h"

#define MASK_LS 0b1 <<20
#define BYTE 0b1 <<22
#define IMM_REG 0b1 <<22
#define ADR_MOD 0b11001 <<21
#define U 0b1 <<23
#define Rn 0b1111 <<16
#define Rd 0b1111 <<12
#define offset_12 0xFFF
#define IMMH 0b1111 <<8
#define IMML 0b1111
#define Rm 0b1111
#define P 0b1 <<24

#define MASK_REGLIST 0b1

#define IMM 0b0
#define REG_SCA 0b1


int arm_LDR_STR (arm_core p,uint32_t ins){
	uint32_t address;

	if ((ins & U) >> 23){
		if(((ins & ADR_MOD) >> 25) == IMM){
			address = arm_read_register(p,(ins & Rn) >>16) + (ins & offset_12);
		}
		else{
			address = arm_read_register(p,(ins & Rn) >> 16) + shifter_operand(p,ins);
		}
	}
	else{
		if(((ins & ADR_MOD) >> 25) == IMM){
			address = arm_read_register(p,(ins & Rn) >>16) - (ins & offset_12);
		}
		else{
			address = arm_read_register(p,(ins & Rn) >> 16) - shifter_operand(p,ins);
		}
	}
	//printf("address %u \n", address);
	if((ins & BYTE) >>22){
		uint8_t bvalue;
		if((ins & MASK_LS) >>20){
			if(!arm_read_byte(p,address,&bvalue)){
				//printf("valeur b load %u \n", bvalue);
				arm_write_register(p,(ins & Rd) >>12,bvalue);
				return 0;
			}
			return 1;
		}
		else{
			//printf("%u \n", arm_read_register(p,(ins & Rd) >>12));
			bvalue=arm_read_register(p,(ins & Rd) >>12);
			//printf("valeur b store %u \n", bvalue);
			return arm_write_byte(p,address,bvalue);
		}
	}
	else{
		uint32_t value;
		if ((ins & MASK_LS) >>20){
			if (!arm_read_word(p,address,&value)){
				//printf("valeur load %u \n", value);
				arm_write_register(p,(ins & Rd) >>12,value);
				return 0;
			}
			return 1;
		}
		else{
			value = arm_read_register(p,(ins & Rd) >>12);
			//printf("valeur store %u \n", value);
			return arm_write_word(p,address,value);
		}
	}		
}

int arm_LDRH_STRH (arm_core p,uint32_t ins){
	uint32_t address;
	if ((ins & U) >> 23){
		if((ins & IMM_REG) >> 22){
			address = arm_read_register(p,(ins & Rn) >> 16) + (((ins & IMMH) >> 4) | (ins & IMML));
		}
		else{
			address = arm_read_register(p,(ins & Rn) >> 16) + arm_read_register(p,ins & Rm);
		}
	}
	else{
		if ((ins & IMM_REG) >> 22){
			address = arm_read_register(p,(ins & Rn) >> 16) - (((ins & IMMH) >> 4) | (ins & IMML));
		}
		else{
			address = arm_read_register(p,(ins & Rn) >> 16) - arm_read_register(p,ins & Rm);
		}
	}
	uint16_t value;
	if ((ins & MASK_LS) >> 20){
		if (!arm_read_half(p,address,&value)){
			arm_write_register(p,(ins & Rd) >>12,value);
			return 0;
		}
		return 1;
	}
	else {
		value = arm_read_register(p,(ins & Rd) >>12);
		return arm_write_half(p,address,value);
	}
}

int arm_load_store(arm_core p, uint32_t ins) {	
	int res;
	if ((ins >> 4 & 1) && (ins >> 7 & 1)){
		printf("LDRH_STRH");
		res=arm_LDRH_STRH(p,ins);
	}
	else{
		res=arm_LDR_STR(p,ins);
	}
    return res;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
	int address;
	int i;
	uint32_t value;
	if ((ins & P) >> 24){
		if((ins & U) >> 23){
			address = arm_read_register(p,(ins & Rn) >> 16) + 4;
			for (i = 0;i <= 15;i++){
				if((ins >> i) & MASK_REGLIST){
					if((ins & MASK_LS)>>20){
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=+4;
				}
			}
			address=-4;
		}
		else{
			address = arm_read_register(p,(ins & Rn) >> 16);
			for (i = 15;i >= 0;i--){
				if((ins >> i) & MASK_REGLIST){
					if((ins & MASK_LS)>>20){
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=+4;
				}
			}
			address=-4;
		}	
	}

	else{

		if((ins & U) >> 23){
			address = arm_read_register(p,(ins & Rn) >> 16);
			for (i = 0;i <= 15;i++){
				if((ins >> i) & MASK_REGLIST){
					if((ins & MASK_LS)>>20){
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=+4;
				}
			}
			address=-8;
		}
		else{
			address = arm_read_register(p,(ins & Rn) >> 16) + 4;
			for (i = 15;i >= 0;i--){
				if((ins >> i) & MASK_REGLIST){
					if((ins & MASK_LS)>>20){
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=+4;
				}
			}
			address=-8;
		}	

	}

    return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}

