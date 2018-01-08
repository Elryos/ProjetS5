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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_data_processing.h"
#include "arm_instruction.h"
#include "util.h"
#include "debug.h"

#define MASK_LS 0b1 <<20
#define MASK_BYTE 0b1 <<22
#define MASK_IMM_REG 0b1 <<22
#define MASK_ADR_MOD 0b1 <<25
#define MASK_U 0b1 <<23
#define MASK_P 0b1 <<24
#define MASK_Rn 0b1111 <<16
#define MASK_Rd 0b1111 <<12
#define offset_12 0xFFF
#define MASK_IMMH 0b1111 <<8
#define MASK_IMML 0b1111
#define MASK_Rm 0b1111

#define MASK_REGLIST 0b1

#define IMM 0b0
#define REG_SCA 0b1
#define P 0b1
#define W 0b1

//renvoie 0 si r�ussite, 1 si �chec.
int arm_LDR_STR (arm_core p,uint32_t ins){
	uint32_t address;

	if ((ins & MASK_U) >> 23){		// Test si l'on ajoute ou soustrait l'offset � l'adresse de base.		
		if(((ins & MASK_ADR_MOD) >> 25) == IMM){		// Test si l'offset est une valeur imm�diate ou une valeur stock�e dans un registre. 
			address = arm_read_register(p,(ins & MASK_Rn) >>16) + (ins & offset_12);
		}
		else{
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) + shifter_operand(p,ins);
		}
	}
	else{
		if(((ins & MASK_ADR_MOD) >> 25) == IMM){		// Test si l'offset est une valeur imm�diate ou une valeur stock�e dans un registre. 
			address = arm_read_register(p,(ins & MASK_Rn) >>16) - (ins & offset_12);
		}
		else{
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) - shifter_operand(p,ins);
		}
	}
	if (((ins >>24 & P) && (ins >>21 & W)) || (!(ins >>24 & P) && !(ins >>21 & W))){		//Test si l'adresse de base doit �tre "sauvegard�e" 
		arm_write_register(p,(ins & MASK_Rn) >> 16,address);
	}
	//printf("address %u \n", address);
	if((ins & MASK_BYTE) >>22){		//LDRB ou STRB
		uint8_t bvalue;
		if((ins & MASK_LS) >>20){		//LOAD ou STORE
			if(!arm_read_byte(p,address,&bvalue)){			// Si la lecture � l'adresse donn�e r�ussie on load dans rd
				//printf("valeur b load %u \n", bvalue);
				arm_write_register(p,(ins & MASK_Rd) >>12,bvalue);
				return 0;
			}
			return 1;
		}
		else{
			//printf("%u \n", arm_read_register(p,(ins & MASK_Rd) >>12));
			bvalue=arm_read_register(p,(ins & MASK_Rd) >>12);
			//printf("valeur b store %u \n", bvalue);
			return arm_write_byte(p,address,bvalue);
		}
	}
	else{
		uint32_t value;
		if ((ins & MASK_LS) >>20){		//LOAD ou STORE
			if (!arm_read_word(p,address,&value)){			// Si la lecture � l'adresse donn�e r�ussie on load dans rd
				//printf("valeur load %u \n", value);
				arm_write_register(p,(ins & MASK_Rd) >>12,value);
				return 0;
			}
			return 1;
		}
		else{
			value = arm_read_register(p,(ins & MASK_Rd) >>12);
			//printf("valeur store %u \n", value);
			return arm_write_word(p,address,value);
		}
	}		
}

int arm_LDRH_STRH (arm_core p,uint32_t ins){
	uint32_t address;
	if ((ins & MASK_U) >> 23){		// Test si l'on ajoute ou soustrait l'offset � l'adresse de base.
		if((ins & MASK_IMM_REG) >> 22){		//Test si l'offset est une valeur imm�diate ou contenue dans un registre.
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) + (((ins & MASK_IMMH) >> 4) | (ins & MASK_IMML));
		}
		else{
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) + arm_read_register(p,ins & MASK_Rm);
		}
	}
	else{
		if ((ins & MASK_IMM_REG) >> 22){		//Test si l'offset est une valeur imm�diate ou contenue dans un registre.
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) - (((ins & MASK_IMMH) >> 4) | (ins & MASK_IMML));
		}
		else{
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) - arm_read_register(p,ins & MASK_Rm);
		}
	}
	if (((ins >>24 & P) && (ins >>21 & W)) || (!(ins >>24 & P) && !(ins >>21 & W))){		//Test si l'adresse de base doit �tre "sauvegard�e" 
		arm_write_register(p,(ins & MASK_Rn) >> 16,address);
	}
	uint16_t value;
	if ((ins & MASK_LS) >> 20){		//LOAD ou STORE
		if (!arm_read_half(p,address,&value)){			//Si la lecture � l'adresse donn�e r�ussie on load dans rd.
			arm_write_register(p,(ins & MASK_Rd) >>12,value);
			return 0;
		}
		return 1;
	}
	else {
		value = arm_read_register(p,(ins & MASK_Rd) >>12);
		return arm_write_half(p,address,value);
	}
}

int arm_load_store(arm_core p, uint32_t ins) {	
	int res;
	if ((ins >> 4 & 1) && (ins >> 7 & 1)){
		//printf("LDRH_STRH");
		res=arm_LDRH_STRH(p,ins);
	}
	else{
		res=arm_LDR_STR(p,ins);
	}
    return res;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
	int address;
	int i;
	int nb_bits = 0;
	uint32_t value;
	if ((ins & MASK_P) >> 24){						//Test si l'on est en incr�mente ou d�cr�mente before
		if((ins & MASK_U) >> 23){					// Test si l'on ajoute ou soustrait l'offset � l'adresse de base.
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) + 4;
			for (i = 0;i <= 15;i++){				//parcours des 16 bit "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Ni�me bit est � 1 on traite le Ni�me registre
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}
		else{
			for (i = 0; i <= 15; i++){				//premier parcours pour conna�tre le nombre de registre � traiter.
				if ((ins >> i) & MASK_REGLIST){
					nb_bits++;
				}
			}
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) - (nb_bits*4);
			for (i = 0; i <= 15; i++){				//Parcours des bits "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Ni�me bit est � 1 on traite le Ni�me registre.
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}	
	}

	else{

		if((ins & MASK_U) >> 23){					// Test si l'on ajoute ou soustrait l'offset � l'adresse de base.
			address = arm_read_register(p,(ins & MASK_Rn) >> 16);
			for (i = 0;i <= 15;i++){				//Parcours des bits "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Ni�me bit est � 1 on traite le Ni�me registre.
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}
		else{
			for (i = 0; i <= 15; i++){				//premier parcours pour conna�tre le nombre de registre � traiter.
				if ((ins >> i) & MASK_REGLIST){
					nb_bits++;
				}
			}
			address = arm_read_register(p,(ins & MASK_Rn) >> 16) - (nb_bits * 4) + 4;
			for (i = 0;i <= 15;i++){				//Parcours des bits "registre"
				if((ins >> i) & MASK_REGLIST){		//Si le Ni�me bit est � 1 on traite le Ni�me registre.
					if((ins & MASK_LS)>>20){		//LOAD ou STORE
						if(!arm_read_word(p,address,&value)){
							arm_write_register(p,i,value);
						}
					}
					else{
						value = arm_read_register(p,i);
						arm_write_word(p,address,value);
					}
					address=address+4;
				}
			}
			address=address-4;
		}	

	}
	if ((ins >> 21) & W){							//Si le bit W est � 1 on sauvegarde Rn
		arm_write_register(p,(ins & MASK_Rn) >> 16,address);
	}

    return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}

