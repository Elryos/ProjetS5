/*
Armator - simulateur de jeu d'instruction ARMv5T ?but p?agogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G??ale GNU publi? par la Free Software
Foundation (version 2 ou bien toute autre version ult?ieure choisie par vous).

Ce programme est distribu?car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp?ifique. Reportez-vous ?la
Licence Publique G??ale GNU pour plus de d?ails.

Vous devez avoir re? une copie de la Licence Publique G??ale GNU en m?e
temps que ce programme ; si ce n'est pas le cas, ?rivez ?la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
?ats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B?iment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H?es
*/

#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>

registers registers_create() {
    uint32_t * r = malloc(sizeof(uint32_t[37]));
    return r;
}

void registers_destroy(registers r) {
	free(r);
}

uint32_t read_cpsr(registers r) {
    return r[31];
}

uint8_t get_mode(registers r) {
    return read_cpsr(r) & 0b11111;
} 

int current_mode_has_spsr(registers r) {
    return get_mode(r) != USR && get_mode(r) != SYS;
}

int in_a_privileged_mode(registers r) {
    return get_mode(r) != USR;
}

int mode_offset(uint8_t mode) {
	if (mode==USR) return 0;
	if (mode==SYS) return 0;
	if (mode==SVC) return 3;
	if (mode==ABT) return 5;
	if (mode==UND) return 7;
	if (mode==IRQ) return 9;
	if (mode==FIQ) return 16;
	return 0;
}

uint32_t read_register(registers r, uint8_t reg) {
    if (reg == 13 || reg == 14 ||  (8 <= reg && reg <= 12 && get_mode(r)==FIQ)) {
    	return r[reg+mode_offset(get_mode(r))];
    } else {
    	return r[reg];
   	}
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    return r[reg];
}

uint32_t read_spsr(registers r) {
    uint8_t mode=get_mode(r);
    if (mode==SVC) return r[32];
	if (mode==ABT) return r[33];
	if (mode==UND) return r[34];
	if (mode==IRQ) return r[35];
	if (mode==FIQ) return r[36];
	return 0;
}

void write_register(registers r, uint8_t reg, uint32_t value) {
	if (reg == 13 || reg == 14 ||  (8 <= reg && reg <= 12 && get_mode(r)==FIQ)) {
    	r[reg+mode_offset(get_mode(r))]=value;
    } else {
    	r[reg]=value;
   	}
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
	r[reg]=value;
}

void write_cpsr(registers r, uint32_t value) {
	r[31]=value;
}

void write_spsr(registers r, uint32_t value) {
    uint8_t mode=get_mode(r);
    if (mode==SVC) r[32] = value;
	if (mode==ABT) r[33] = value;
	if (mode==UND) r[34] = value;
	if (mode==IRQ) r[35] = value;
	if (mode==FIQ) r[36] = value;
}