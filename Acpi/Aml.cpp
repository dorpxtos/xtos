#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Log.h>
#include <Paging.h>
#include <Aml.h>

struct AmlProgram {
	uint8_t* code;
	uint32_t pos;
	uint32_t length;
};

/*

int AmlReadByte(AmlProgram* code) {
	int c = code->code[code->pos];
	code->pos++;
	return c;
}

int AmlParseLeadNameChar(AmlProgram* prog) {
	char c = AmlReadByte(prog);
	if (isalpha(c) || c == '_') {
		return 1;
	}
}

int AmlParseNameChar(AmlProgram* prog) {
	char c = AmlReadByte(prog);
	if (AmlParseLeadNameChar(prog) || c == '_') {
		return 1;
	}
}

int AmlParseNameSeg(AmlProgram* prog) {
	if (AmlParseLeadNameChar(prog)) {
		
	}
}

int AmlParseNamePath(AmlProgram* prog) {
	if (AmlParseNameSeg(prog)) {

	}
}

int AmlParseRootChar(AmlProgram* prog) {
	if (AmlParseNamePath(prog)) {

	}
}
*/

void AmlParse(uint8_t* code, size_t length) {
	AmlProgram prog;
	prog.code = code;
	prog.pos = 0;
	prog.length = length;
	/*if (AmlReadByte(&prog) == ROOT_CHAR) {
		AmlParseRootChar(&prog);
	}*/
	Log("Parsing AML @ %80X (Length: %d)", prog.code, prog.length);
}