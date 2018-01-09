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

#define MASK_LS 0b1 <<20
#define MASK_BYTE 0b1 <<22
#define MASK_IMM_REG 0b1 <<22
#define MASK_ADR_MOD 0b1 <<25
#define MASK_U 0b1 <<23
#define MASK_P 0b1 <<24
#define MASK_Rn 0b1111 <<16
#define MASK_Rd 0b1111 <<12
#define offset_12 0xFFF
#define MASK_IMMH 0b1111 <<8
#define MASK_IMML 0b1111
#define MASK_Rm 0b1111

#define MASK_REGLIST 0b1

//#define IMM 0b0

#define REG_SCA 0b1
#define P 0b1

uint32_t addressing_mode(arm_core p, uint32_t ins) {
	uint8_t is_half = !get_bit(ins, 26);
	uint8_t is_shift = get_bit(ins, 25);
	uint8_t is_immedHL = get_bit(ins, 22);
	uint8_t U = get_bit(ins, 23);
	uint8_t Rm = get_bits(ins,3,0);
	uint32_t res;

	if (is_shift) {
		uint8_t c;
		res = shifter_operand(p,ins,&c);
	} else if (is_half) {
		if (is_immedHL) {
			res = (get_bits(ins,11,8) << 4 | get_bits(ins,3,0));
		} else {
			res = arm_read_register(p, Rm);
		}
	} else {
		res=get_bits(ins,11,0);
	}

	if (!U) res*=-1;
	
	return res; 
}


int arm_load_store(arm_core p, uint32_t ins) {
	uint8_t Rn = get_bits(ins, 19, 16);
	uint8_t Rd = get_bits(ins, 15, 12);
	uint8_t is_load = get_bit(ins, 20);
	uint8_t is_half = get_bit(ins,7) && get_bit(ins,4);
	uint8_t is_byte = get_bit(ins, 22);
	/*uint8_t P = get_bit(ins, 24);
	uint8_t W = get_bit(ins, 21);*/
	
	uint32_t address = arm_read_register(p, Rn) + addressing_mode(p, ins);;

	// SAUVEGARDE, P, W ? ? ? ???

	if (is_load) {
		// LOAD
		if (is_half) {
			uint16_t val;
			arm_read_half(p, address, &val);
			return arm_write_register(p, Rd, val);
		} else if (is_byte) {
			uint8_t val;
			arm_read_byte(p, address, &val);
			return arm_write_register(p, Rd, val);
		} else {
			uint32_t val;
			arm_read_word(p, address, &val);
			return arm_write_register(p, Rd, val);
		}


	} else {
		// STORE
		uint32_t val = arm_read_register(p, Rd);
		if (is_half) {
			return arm_write_half(p, address, (uint16_t) val);
		} else if (is_byte) {
			return arm_write_byte(p, address, (uint8_t) val);
		} else {
			return arm_write_word(p, address, val);
		}	
	}
}


int arm_load_store_multiple(arm_core p, uint32_t ins) {
	int address;
	int i;
	int nb_bits = 0;
	uint32_t value;
	if ((ins & MASK_P) >> 24){						//Test si l'on est en incrémente ou décrémente before
		if((ins & MASK_U) >> 23){					// Test si l'on ajoute ou soustrait l'offset à l'adresse de base.
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) + 4;
			for (i = 0;i <= 15;i++){				//parcours des 16 bit "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Nième bit est à 1 on traite le Nième registre
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}
		else{
			for (i = 0; i <= 15; i++){				//premier parcours pour connaître le nombre de registre à traiter.
				if ((ins >> i) & MASK_REGLIST){
					nb_bits++;
				}
			}
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) - (nb_bits*4);
			for (i = 0; i <= 15; i++){				//Parcours des bits "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Nième bit est à 1 on traite le Nième registre.
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}	
	}

	else{

		if((ins & MASK_U) >> 23){					// Test si l'on ajoute ou soustrait l'offset à l'adresse de base.
			address = arm_read_register(p,(ins & MASK_Rn) >> 16);
			for (i = 0;i <= 15;i++){				//Parcours des bits "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Nième bit est à 1 on traite le Nième registre.
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}
		else{
			for (i = 0; i <= 15; i++){				//premier parcours pour connaître le nombre de registre à traiter.
				if ((ins >> i) & MASK_REGLIST){
					nb_bits++;
				}
			}
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) - (nb_bits * 4) + 4;
			for (i = 0;i <= 15;i++){				//Parcours des bits "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Nième bit est à 1 on traite le Nième registre.
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}	

	}
	if ((ins >> 21)){							//Si le bit W est à 1 on sauvegarde Rn
		arm_write_register(p,(ins & MASK_Rn) >> 16,address);
	}

    return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}