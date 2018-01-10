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
	uint8_t is_P = get_bit(ins, 24);
	uint8_t is_W = get_bit(ins, 21);
	uint32_t val;
	int res;

	uint32_t address = arm_read_register(p, Rn);

	if (is_P) address+= addressing_mode(p, ins);

	if (is_load) {
		// LOAD
		if        (is_half) {
			arm_read_half(p, address, (uint16_t *) &val);
		} else if (is_byte) {
			arm_read_byte(p, address, (uint8_t *) &val);
		} else {//(is_word)
			arm_read_word(p, address, &val);
		}

		if (Rd==PC) val&=0xFFFFFFFE;
		if (!is_P && is_W ) res = arm_write_usr_register(p,Rd,val);	else res = arm_write_register(p,Rd,val);

	} else {
		// STORE
		if (!is_P && is_W ) val = arm_read_usr_register(p, Rd);	else val = arm_read_register(p, Rd);

		if        (is_half) {
			res = arm_write_half(p, address, (uint16_t) val);
		} else if (is_byte) {
			res = arm_write_byte(p, address, (uint8_t) val);
		} else {//(is_word)
			res = arm_write_word(p, address, val);
		}	
	}

	if (!is_P) address+= addressing_mode(p, ins);

	if (!is_P || is_W) arm_write_register(p, Rn, address);

	return res;
}


int arm_load_store_multiple(arm_core p, uint32_t ins) {
	uint8_t Rn = get_bits(ins, 19, 16);
	uint8_t is_load = get_bit(ins, 20);
	uint8_t is_incr = get_bit(ins,23);
	uint8_t is_before = get_bit(ins,24);
	uint8_t write_back = get_bit(ins,21);
	uint8_t Number_Of_Set_Bits = 0;
	uint32_t val;	
	int Ri;
	int res;
	
	uint32_t start_address = arm_read_register(p, Rn);
	uint32_t address = start_address;

	if (!is_incr)
		for (Ri=0;Ri<16;Ri++) 
			if((ins >> Ri) & 1) 
				address-=4;

	if (is_incr == is_before) address+=4;

	for (Ri=0;Ri<16;Ri++) {
		if ((ins >> Ri) & 1) {		
			if (is_load) {
				//LOAD
				arm_read_word(p, address, &val);
				if (Ri==PC) val&=0xFFFFFFFE;
				res = arm_write_register(p, Ri, val);
			} else {
				//STORE
				val = arm_read_register(p, Ri);
				res = arm_write_word(p,address,val);
			}

			//address = is_incr ? address+4 : address-4 ;
			address+=4;
			Number_Of_Set_Bits++;
		}
	}

	if (write_back) {
		if (is_incr) { 
			arm_write_register(p, Rn, start_address+Number_Of_Set_Bits*4);
		} else {
			arm_write_register(p, Rn, start_address-Number_Of_Set_Bits*4);
		}
	}

	return res;
}


int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}