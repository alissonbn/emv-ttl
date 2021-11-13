#include "stdafx.h"
#include <WinSCard.h>

void hexStringToByteArray(const char* input, int inputLength, BYTE* output){
	int aux, i = 0;
	for(i = 0; i < inputLength / 2; i++){
		sscanf_s(input + 2 * i, "%2X", &aux);
		output[i] = (BYTE)aux;
	}
}

#include "ttl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	LONG result = 0;

	SCARDCONTEXT context;
	SCARDHANDLE scardH;
	DWORD protocol;
	LPTSTR readers = NULL;
	DWORD cchR = SCARD_AUTOALLOCATE;
	//char* selectAPDU = "00A4040007A000000004306000"; //Master debito
	//char* selectAPDU = "00A4040007A000000004101000"; //Master credito
	char* selectAPDU = "00A4040007A000000003101000"; //Visa credito
	int apduSizeForBYTE = strlen(selectAPDU) / 2;
	BYTE *input = (BYTE*)malloc(apduSizeForBYTE);
	BYTE output[256];
	DWORD outputLength = sizeof(output);
	int i;

	result = ttl_connect(&context, &scardH, &protocol);
	printf("-> %s\n", selectAPDU); 
	
	
	memset(input, 0xFF, apduSizeForBYTE);
	hexStringToByteArray(selectAPDU, apduSizeForBYTE * 2, input);
	
	
	result = ttl_sendreceiveAPDU(scardH, protocol, input, apduSizeForBYTE, output, &outputLength);
	
	printf("<- ");
	for(i = 0; i < outputLength; i++){
		printf("%02X", output[i]); 
	}
	printf("\n");

	free(input);

	selectAPDU = "80A8000002830000";	
	printf("-> %s\n", selectAPDU);
	apduSizeForBYTE = strlen(selectAPDU) / 2;
	input = (BYTE*)malloc(apduSizeForBYTE);
	hexStringToByteArray(selectAPDU, apduSizeForBYTE * 2, input);
	memset(output, NULL, sizeof(output));
	outputLength = sizeof(output);
	result = ttl_sendreceiveAPDU(scardH, protocol, input, apduSizeForBYTE, output, &outputLength);

	printf("<- ");
	for(i = 0; i < outputLength; i++){
		printf("%02X", output[i]); 
	}
	printf("\n");

	free(input);

	selectAPDU = "00B2010C00";
	printf("-> %s\n", selectAPDU);
	apduSizeForBYTE = strlen(selectAPDU) / 2;
	input = (BYTE*)malloc(apduSizeForBYTE);
	hexStringToByteArray(selectAPDU, apduSizeForBYTE * 2, input);
	memset(output, NULL, sizeof(output));
	outputLength = sizeof(output);
	result = ttl_sendreceiveAPDU(scardH, protocol, input, apduSizeForBYTE, output, &outputLength);

	printf("<- ");
	for(i = 0; i < outputLength; i++){
		printf("%02X", output[i]); 
	}
	printf("\n");

	free(input);

	result = ttl_disconnect(context, scardH);
	getchar();
	return 0;
}

