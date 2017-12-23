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

void print_codeop(uint8_t code) {
    switch (code) { 
        case (AND) :
            printf(" ADD ");
            break;
        case (EOR) :
            printf(" EOR ");
            break;
        case (SUB) :
            printf(" SUB ");         
            break;
        case (RSB) :
            printf(" RSB ");           
            break;
        case (ADD) :
            printf(" ADD ");           
            break;
        case (ADC) :
            printf(" ADC ");          
            break;
        case (SBC) :
            printf(" SBC ");          
            break;
        case (RSC) :
            printf(" RSC ");          
            break;
        case (TST) :
            printf(" TST ");         
            break;
        case (TEQ) :
            printf(" TEQ ");          
            break;
        case (CMP) :
            printf(" CMP ");          
            break;
        case (CMN) :
            printf(" CMN ");         
            break;
        case (ORR) :
            printf(" ORR ");          
            break;
        case (MOV) :
            printf("MOV ");            
            break;
        case (BIC) :
            printf(" BIC ");            
            break;
        case (MVN) :
            printf("MVN ");
            break;
        default :
            printf("%i", code);
            break;
    }
}

void print_flags(arm_core p) {
	uint32_t cpsr = arm_read_cpsr(p);
	uint8_t n = cpsr >> N & 1;
    uint8_t z = cpsr >> Z & 1;
    uint8_t c = cpsr >> C & 1;
    uint8_t v = cpsr >> V & 1;
	printf("N=%i ; Z=%i; C=%i, V=%i\n", n,z,c,v);
}

void change_bit(uint32_t * s, uint8_t n, uint8_t val) {
	*s = (*s & ~(1 << n)) | (val << n);
}

int arm_data_processing(arm_core p, uint32_t ins) {
    uint64_t Value_Rn = arm_read_register(p, (ins & MASK_RN) >> 16);
    uint8_t Rd = (ins & MASK_RD) >> 12;
    uint64_t Value_Shifter;
    uint64_t Res;
    uint8_t force_carry;
    
    if ((ins & MASK_I) >> 25) {
    	Value_Shifter = ror(ins & 0xFF, ((ins >> 8) & 0xF) * 2);
    } else {
    	Value_Shifter = shifter_operand(p, ins, &force_carry);
    }
    
    switch ((ins & MASK_OPCODE) >> 21) { 
    	case (AND) :
            Res = Value_Rn & Value_Shifter;
    		arm_write_register(p, Rd, Value_Rn & Value_Shifter);
    		break;
    	case (EOR) :
            Res = Value_Rn ^ Value_Shifter;
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
    		Res = Value_Rn + Value_Shifter + get_bit(arm_read_cpsr(p), C);
    		arm_write_register(p,Rd,Res);
    		break;
    	case (SBC) :
    		Res = Value_Rn - Value_Shifter - !(get_bit(arm_read_cpsr(p), C));
    		arm_write_register(p,Rd,Res);
    		break;
    	case (RSC) :
    		Res = Value_Shifter - Value_Rn - !(get_bit(arm_read_cpsr(p), C));
    		arm_write_register(p,Rd,Res);
    		break;
    	case (TST) :
    		Res = Value_Rn & Value_Shifter;
    		break;
    	case (TEQ) :
    		Res = Value_Rn ^ Value_Shifter;
    		break;
    	case (CMP) :
    		Res = Value_Rn - Value_Shifter;
    		break;
    	case (CMN) :
    		Res = Value_Rn + Value_Shifter;
    		break;
    	case (ORR) :
    		Res = Value_Rn | Value_Shifter;
    		arm_write_register(p,Rd,Res);
    		break;
    	case (MOV) :
    		Res = Value_Shifter;
    		arm_write_register(p,Rd,Res);
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


    // MISE A JOUR DES FLAGS
    if ((ins & MASK_STATUS) >> 20) {
        uint32_t cpsr = arm_read_cpsr(p);
    
        change_bit(&cpsr, N, get_bit(Res, 31));
        change_bit(&cpsr, Z, Res==0);
        if ((SUB <= ((ins & MASK_OPCODE)) >> 21) && (((ins & MASK_OPCODE) >> 21) <= CMN)) {
            change_bit(&cpsr, C, get_bit(Res, 32));
            change_bit(&cpsr, V, (get_bit(Value_Rn, 31) == get_bit(Value_Shifter, 31)) && (get_bit(Value_Rn, 31) != get_bit(Res, 31)));
        }
        if (force_carry) cpsr=set_bit(cpsr, C);

        arm_write_cpsr(p, cpsr);
    }

    /* PRINT DE TEST*/
    printf("r%i <- (", Rd);
    if ((((ins & MASK_OPCODE) >> 21) != MOV) && (((ins & MASK_OPCODE) >> 21) != MVN)) printf("%li", Value_Rn);
    print_codeop((ins & MASK_OPCODE) >> 21);
    printf("%li) = %li\n", Value_Shifter, Res);
    print_flags(p);
	/* PRINT DE TEST*/

    return 0;
}