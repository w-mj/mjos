#include <Uefi.h>

EFI_STATUS UefiMain(IN EFI_HANDLE imageHandle, IN EFI_SYSTEM_TABLE *systemTable) {
	systemTable->ConOut->OutputString(systemTable->ConOut, L"Hello World!\n");
	return EFI_SUCCESS;
}
