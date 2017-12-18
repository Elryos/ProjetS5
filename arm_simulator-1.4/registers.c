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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>

struct registers_data {
	uint32_t r[37];
	uint8_t mode;
};

registers registers_create() {
    registers r = malloc(sizeof(r));
    return r;
}

void registers_destroy(registers r) {
	free(r);
}

uint8_t get_mode(registers r) {
    return r->mode;
} 

int current_mode_has_spsr(registers r) {
    return r->mode != USR && r->mode != SYS;

}

int in_a_privileged_mode(registers r) {
    return r->mode != USR;
}
#define USR 0x10
#define SYS 0x1f
#define SVC 0x13
#define ABT 0x17
#define UND 0x1b
#define IRQ 0x12
#define FIQ 0x11
int mode_offset(uint8_t mode) {
	if (USR) return 0;
	if (SYS) return 0;
	if (SVC) return 3;
	if (ABT) return 5;
	if (UND) return 7;
	if (IRQ) return 9;
	if (FIQ) return 16;

}

uint32_t read_register(registers r, uint8_t reg) {
    if (reg == 13 || reg == 14) {
    	return r->r[reg+mode_offset(r->mode)];
    } else if (8 <= reg && reg <= 12 && r->mode==FIQ) {
    	return r->r[reg+16];
    } else {
    	return r->r[reg];
   	}
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    return r->r[reg];
}

uint32_t read_cpsr(registers r) {
    return r->r[31];
}

uint32_t read_spsr(registers r) {
    if (SVC) return r->r[32];
	if (ABT) return r->r[33];
	if (UND) return r->r[34];
	if (IRQ) return r->r[35];
	if (FIQ) return r->r[36];
}

void write_register(registers r, uint8_t reg, uint32_t value) {
	if (reg == 13 || reg == 14) {
    	r->r[reg+mode_offset(r->mode)]=value;
    } else if (8 <= reg && reg <= 12 && r->mode==FIQ) {
    	r->r[reg+16]=value;
    } else {
    	r->r[reg]=value;
   	}
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
	r->r[reg]=value;
}

void write_cpsr(registers r, uint32_t value) {
	r->r[31]=value;
}

void write_spsr(registers r, uint32_t value) {
    if (SVC) r->r[32] = value;
	if (ABT) r->r[33] = value;
	if (UND) r->r[34] = value;
	if (IRQ) r->r[35] = value;
	if (FIQ) r->r[36] = value;
}