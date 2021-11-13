#include <WinSCard.h>

typedef struct{
	BYTE CLA;
	BYTE INS;
	BYTE P1;
	BYTE P2;
	BYTE P3;
}C_TPDU;

int ttl_connect(SCARDCONTEXT* context, SCARDHANDLE* scardH, DWORD* protocol){
	int result = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, context);
	LPTSTR readers = NULL;
	DWORD cchR = SCARD_AUTOALLOCATE;

	if(result != SCARD_S_SUCCESS){
		//printf("SCardEstablishContext result = %d", result);
		return -1;
	}

	result = SCardListReaders(*context, NULL, (LPTSTR)&readers, &cchR);

	result = SCardConnect(*context, readers, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, scardH, protocol);

	return result;
}

int ttl_disconnect(SCARDCONTEXT context ,SCARDHANDLE scardH){
	//!!!Liberar a memória alocada com  SCARD_AUTOALLOCATE no SCardListReaders!!!
	//SCardFreeMemory(context, readers);
	return SCardDisconnect(scardH, SCARD_LEAVE_CARD);
}

int ttl_sendreceiveAPDU(SCARDHANDLE scardH, DWORD protocol, BYTE* input, DWORD inputLength, BYTE* output, DWORD* outputLength){
	C_TPDU* tpdu = (C_TPDU*)input;
	DWORD IOoutputLength = *outputLength;
	int result = 0;
	memset(output, 0, *outputLength);
	
	if(inputLength < 4){
		//error
	}else if(inputLength == 4){
		//case 1 command
		result = SCardTransmit(scardH, (protocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1), input, inputLength, NULL,  output, &IOoutputLength);
	}else if(inputLength == 5){
		//case 2 command
		result = SCardTransmit(scardH, (protocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1), input, sizeof(C_TPDU), NULL,  output, &IOoutputLength);
		if(IOoutputLength > 0){
			if(output[0] == 0x6C){
				BYTE* b = (BYTE*)malloc(inputLength);
				memcpy(b, input, inputLength);
				b[4] = output[1];
				IOoutputLength = *outputLength;
				memset(output, 0, *outputLength);
				result = SCardTransmit(scardH, (protocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1), b, inputLength, NULL,  output, &IOoutputLength);
				*outputLength = IOoutputLength;
				free(b);
			}
		}
	}else if(inputLength > 5){
		//case 3 and 4 command
		int lc = input[4];
		if(lc + 5 == inputLength){
			//case 3
		}else if(lc + 5 < inputLength){
			//case 4
			result = SCardTransmit(scardH, (protocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1), input, sizeof(C_TPDU) + lc, NULL,  output, &IOoutputLength);
			if(IOoutputLength > 0){
				if(IOoutputLength > 0 && output[0] == 0x61){
					DWORD Licc = output[1];
					C_TPDU tpdu;
					tpdu.CLA = 0x00;
					tpdu.INS = 0xC0;
					tpdu.P1 = 00;
					tpdu.P2 = 00;
					tpdu.P3 = Licc;
					memset(output, 0, *outputLength);
					IOoutputLength = *outputLength;
					result = SCardTransmit(scardH, (protocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1), (BYTE*)&tpdu, sizeof(C_TPDU), NULL,  output, &IOoutputLength);
					*outputLength = IOoutputLength;
				}
			}
		}
	}
	return result;
}