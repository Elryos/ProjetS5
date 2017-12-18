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

uint32_t read_register(registers r, uint8_t reg) {
    uint32_t value=0;
    return value;
	/*ICI*/
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    return r->r[reg];
}

uint32_t read_cpsr(registers r) {
    return r->r[16];
}

uint32_t read_spsr(registers r) {
    uint32_t value=0;
    return value;
	/*ICI*/
}

void write_register(registers r, uint8_t reg, uint32_t value) {
	/*ICI*/

}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
	r->r[reg]=value;
}

void write_cpsr(registers r, uint32_t value) {
	r->r[16]=value;
}

void write_spsr(registers r, uint32_t value) {
	/*ICI*/
}
