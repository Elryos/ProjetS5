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
#include "util.h"
#include "debug.h"

#define MASK_RM 0b1111 << 0
#define MASK_IMMEDIATE 0b1 << 4
#define MASK_SHIFT 0b11 << 5
#define MASK_RS_IMMEDIATE 0b11111 << 7
#define MASK_RS_REGISTER 0b1111 << 8
// #define LSL 0b00
// #define LSR 0b01
// #define ASR 0b10
// #define ROR 0b11

int shifter_operand(arm_core p, uint32_t ins) {
	uint8_t Rm = arm_read_register(p, ins & MASK_RM >> 0);
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
			return 1;
	}
	return 0;
}

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}