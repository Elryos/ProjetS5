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
#include "util.h"
#include "debug.h"

#define LS 0b1 <<20
#define BYTE 0b1 <<22
#define HALF 0b1001 <<4
#define ADR_MOD 0b11001 <<21

#define IMM_PRE 0b01
#define REG_SCA_PRE 0b11
#define IMM_POS 0b00
#define REG_SCA_POS 0b10

int arm_load_store(arm_core p, uint32_t ins) {	
	/*int res;
	if ((ins & HALF >> 4) == 0b1001){
		if(ins & LS >>20){
			res=arm_LDRH(p,ins);
		}
		else{
			res=arm_STRH(p,ins);
		}
	}
	else{
	    switch(ins & BYTE >>22){
	    	case(1) :
	    		res=arm_LDRB_STRB(p,ins);
	    		break;
	    	case(0) :
	    		res=arm_LDR_STR(p,ins);
	    		break;
	    }
	}*/
    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}

/*int arm_LDR_STR (arm_core p,uint32_t ins){
	switch(ins & ADR_MOD){
		case(IMM_PRE) :
			
	}
}
*/