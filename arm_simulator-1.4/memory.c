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

#include <stdlib.h>
#include "memory.h"
#include "arm_constants.h"
#include "util.h"

struct memory_data {
    uint8_t * m;
    size_t size;
    int is_big_endian;
};

memory memory_create(size_t size, int is_big_endian) {
    memory mem = malloc(sizeof(struct memory_data));
    mem->m = malloc(size);
    mem->size = size;
    mem->is_big_endian = is_big_endian;
    return mem;
}

size_t memory_get_size(memory mem) {
    return mem->size;
}

void memory_destroy(memory mem) {
    free(mem->m);
    free(mem);
}

int is_not_valid_address(memory mem, uint32_t address) {
    return (address >= memory_get_size(mem));
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
    if (is_not_valid_address(mem,address)) return DATA_ABORT;

    *value = mem->m[address];
    return 0;
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value) {
    if (is_not_valid_address(mem,address)) return DATA_ABORT;

    if (mem->is_big_endian) {
         *value = mem->m[address] << 8 | mem->m[address+1];
    } else {
         *value = mem->m[address+1] << 8 | mem->m[address];
    }
    return 0;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value) {
    if (is_not_valid_address(mem,address)) return DATA_ABORT;

    if (mem->is_big_endian) {
        *value = mem->m[address] << 24 | mem->m[address+1] << 16 | mem->m[address+2] << 8 | mem->m[address+3];
    } else {
        *value = mem->m[address+3] << 24 | mem->m[address+2] << 16 | mem->m[address+1] << 8 | mem->m[address];
    }
    return 0;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    if (is_not_valid_address(mem,address)) return DATA_ABORT;
    
    mem->m[address]=value;
    return 0;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value) {
    if (is_not_valid_address(mem,address)) return DATA_ABORT;
    if (address%2) return DATA_ABORT;

    if (mem->is_big_endian) {
        mem->m[address] = value >> 8;
        mem->m[address+1] = value;
    } else {
        mem->m[address] = value;
        mem->m[address+1] = value >> 8;
    }
    return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value) {
    if (is_not_valid_address(mem,address)) return DATA_ABORT;
    if (address%4) return DATA_ABORT;

    if (mem->is_big_endian) {
        mem->m[address] = value >> 24;
        mem->m[address+1] = value >> 16;
        mem->m[address+2] = value >> 8;
        mem->m[address+3] = value;
    } else {
        mem->m[address] = value;
        mem->m[address+1] = value >> 8;
        mem->m[address+2] = value >> 16;
        mem->m[address+3] = value >> 24;
    }
    return 0;
}