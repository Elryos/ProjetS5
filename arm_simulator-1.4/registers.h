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
#ifndef __REGISTERS_H__
#define __REGISTERS_H__
#include <stdint.h>

typedef uint32_t * registers;

registers registers_create();
void registers_destroy(registers r);

uint8_t get_mode(registers r);
int current_mode_has_spsr(registers r);
int in_a_privileged_mode(registers r);

uint32_t read_register(registers r, uint8_t reg);
uint32_t read_usr_register(registers r, uint8_t reg);
uint32_t read_cpsr(registers r);
uint32_t read_spsr(registers r);
void write_register(registers r, uint8_t reg, uint32_t value);
void write_usr_register(registers r, uint8_t reg, uint32_t value);
void write_cpsr(registers r, uint32_t value);
void write_spsr(registers r, uint32_t value);

#endif

/*
0	r0
1	r1
2	r2
3	r3
4	r4
5	r5
6	r6
7	r7
8	r8
9	r9
10	r10
11	r11
12	r12
13	SP
14	LR
15	PC
16 	CPSR
17	SP_SVC
18	LR_SVC
19	SP_ABT
20	LR_ABT
21	SP_UND
22	LR_UND
23	SP_IRQ
24	LR_IRQ
25	r8_FIQ
26	r9_FIQ
27	r10_FIQ
28	r11_FIQ
29	r12_FIQ
30	SP_FIQ
31	LR_FIQ
32	SPSR_SVC
33	SPSR_ABT
34	SPSR_UND
35	SPSR_IRQ
36 	SPSR_FIQ
*/