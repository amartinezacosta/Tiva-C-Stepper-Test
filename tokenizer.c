#include <stdbool.h>
#include <stdint.h>

static bool is_White(unsigned char c);
static bool is_Delim(unsigned char c, const unsigned char *delimiters);
static bool is_In(unsigned char c, const unsigned char *s);

uint8_t Token_get(const char *string, const unsigned char *delimiters, char *token){
	uint8_t count = 0;

	if(*string == 0){
		return 0;
	}

	while(is_White(*string)){						//ignore white spaces
		string++;
		count++;
	}

	if(is_In(*string, delimiters)){
		*token++ = *string++;						//if any delimiter is found point to the element
		count++;
		while(!is_Delim(*string, delimiters)){		//while the next delimiter is not found, continue
			*token++ = *string++;
			count++;
		}
	}

	*token = 0;
	return count;
}

static bool is_White(unsigned char c){
	if(c == ' ' || c == 9){
		return true;
	}

	return false;
}

static bool is_In(unsigned char c, const unsigned char *s){
	while(*s){
		if(*s == c){return true;}
		s++;
	}

	return false;
}

static bool is_Delim(unsigned char c, const unsigned char *delimiters){
	if(is_In(c, delimiters) || c == 9 || c == '\r' || c == 0){
		return true;
	}

	return false;
}


