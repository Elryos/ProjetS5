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

#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "arm_instruction.h"
#include "util.h"
#include "debug.h"

#define MASK_OPCODE 0b1111 << 21
#define MASK_I 0b1 << 25
#define MASK_RN 0b1111 << 16
#define MASK_RD 0b1111 << 12
#define MASK_STATUS 0b1 << 20
#define MASK_MAJREG 0b11 << 23


int arm_data_processing(arm_core p, uint32_t ins) {
    
    uint32_t Value_Rn = arm_read_register(p, (ins & MASK_RN) >> 16);
    uint8_t Rd = (ins & MASK_RD) >> 12;
    uint32_t Value_Shifter;
    uint32_t Res;
    uint8_t shifter_carry_out=0;
    

    // LECTURE DE RM, SI IMMEDIATE OU SHIFT OPERAND
    if ((ins & MASK_I) >> 25)
    	Value_Shifter = ror(ins & 0xFF, ((ins >> 8) & 0xF) * 2);
    else 
    	Value_Shifter = shifter_operand(p, ins, &shifter_carry_out);

    
    // CALCUL DE L'OPERATION CORRESPONDANTE
    switch ((ins & MASK_OPCODE) >> 21) { 
    	case (AND) :
            Res = Value_Rn & Value_Shifter;
    		break;
    	case (EOR) :
            Res = Value_Rn ^ Value_Shifter; 
    		break;
    	case (SUB) :
            Res = Value_Rn - Value_Shifter ;  
            Value_Shifter*=-1;
            break;
        case (RSB) :
            Res = Value_Shifter - Value_Rn ;    
            Value_Rn*=-1;
            break;
        case (ADD) :
            Res = Value_Rn + Value_Shifter ;   
            break;
        case (ADC) :
            Res = Value_Rn + Value_Shifter + get_bit(arm_read_cpsr(p), C);
            break;
        case (SBC) :
            Res = Value_Rn - Value_Shifter - !(get_bit(arm_read_cpsr(p), C));
            Value_Shifter*=-1;
            break;
        case (RSC) :
            Res = Value_Shifter - Value_Rn - !(get_bit(arm_read_cpsr(p), C));
            Value_Rn*=-1;            
            break;
        case (TST) :
            // CAS PARTICULIER MRS
            if (get_bit(ins,20)) arm_miscellaneous(p,ins);
            Res = Value_Rn & Value_Shifter;
            break;
        case (TEQ) :
            // CAS PARTICULIER MSR
            if (get_bit(ins,20)) arm_miscellaneous(p,ins);
            Res = Value_Rn ^ Value_Shifter;
            break;
        case (CMP) :
            // CAS PARTICULIER MRS
            if (!get_bit(ins,20)) arm_miscellaneous(p,ins);
            Res = Value_Rn - Value_Shifter;
            Value_Shifter*=-1;
            break;
        case (CMN) :
            // CAS PARTICULIER MSR
            if (get_bit(ins,20)) arm_miscellaneous(p,ins);
    		Res = Value_Rn + Value_Shifter;
    		break;
    	case (ORR) :
    		Res = Value_Rn | Value_Shifter;
    		break;
    	case (MOV) :
    		Res = Value_Shifter;
    		break;
    	case (BIC) :
    		Res = Value_Rn & ~(Value_Shifter) ;
    		break;
    	case (MVN) :
    		Res = ~(Value_Shifter);
    		break;
    	default :
    		break;
    }


    // RESULTAT DANS RD SAUF SI TST, TEQ, CMP, CMN
    if (((ins & MASK_MAJREG) >> 23) != 0b10)
    	arm_write_register(p,Rd,Res);


    // MISE A JOUR DES FLAGS
    if ((ins & MASK_STATUS) >> 20) {
    	if (Rd==PC && arm_current_mode_has_spsr(p)) {
    		arm_write_cpsr(p,arm_read_spsr(p));
    	} else {
            uint8_t a = get_bit(Value_Rn, 31);
            uint8_t b = get_bit(Value_Shifter, 31);
            uint8_t r = get_bit(Res, 31);

	        uint32_t cpsr = arm_read_cpsr(p);

	        change_bit(&cpsr, N, get_bit(Res, 31));
	        change_bit(&cpsr, Z, Res==0);
            if (!(ins & MASK_I) >> 25) change_bit(&cpsr, C, shifter_carry_out);
	        
	        if ((SUB <= ((ins & MASK_OPCODE)) >> 21) && (((ins & MASK_OPCODE) >> 21) <= CMN)) {
	            if (!shifter_carry_out) change_bit(&cpsr, C, ((a && b) || ((!(r)) && (a!=b))));
	            change_bit(&cpsr, V, ((a==b) && (b != r)));
	        }

	        arm_write_cpsr(p, cpsr);
	    }
	}


    /* PRINT DE TEST*/
    printf("r%i <- (", Rd);
    if ((((ins & MASK_OPCODE) >> 21) != MOV) && (((ins & MASK_OPCODE) >> 21) != MVN)) printf("%i", Value_Rn);
    printf(" %s ",arm_get_opcode_name(((ins & MASK_OPCODE) >> 21)));
    printf("%i) = %i\n", Value_Shifter, Res);
	uint32_t cpsr = arm_read_cpsr(p);	   
	printf("N=%i  Z=%i  C=%i  V=%i\n", cpsr >> N & 1,cpsr >> Z & 1,cpsr >> C & 1,cpsr >> V & 1);
	/* PRINT DE TEST*/

    return 0;
}