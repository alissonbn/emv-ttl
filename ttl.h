#include <WinSCard.h>
int ttl_connect(SCARDCONTEXT* context, SCARDHANDLE* scardH, DWORD* protocol);
int ttl_disconnect(SCARDCONTEXT context, SCARDHANDLE scardH);
int ttl_sendreceiveAPDU(SCARDHANDLE scardH, DWORD protocol, BYTE* input, DWORD inputLength, BYTE* output, DWORD* outputLength);