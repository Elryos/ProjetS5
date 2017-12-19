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
#include "util.h"

#define MASK_TYPE 0b111 << 25
#define MASK_SHIFT 0b1 << 4

#define DATA_PROCESSING 0b000
#define LOAD_STORE 0b010
#define LOAD_STORE_MULTIPLE 0b100
#define BRANCH 0b101
#define COPROCESSOR_OTHERS_SWI 0b111

static int arm_execute_instruction(arm_core p) {
    uint32_t ins;

    if (arm_fetch(p, &ins)) return 1;
    
    switch (ins && MASK_TYPE >> 25) {
    	case(DATA_PROCESSING) :
    		if (ins && MASK_SHIFT >> 4) {
    			arm_data_processing_shift(p, ins);
    		} else {
    			arm_data_processing_immediate_msr(p, ins);
    		}
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
    		return 1;
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
