#include <Uefi.h>
//#include <Library/PcdLib.h>
//#include <Library/UefiLib.h>
//#include <Library/UefiApplicationEntryPoint.h>

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *st) {
	st->ConOut->OutputString(st->ConOut, L"I am SB\n");
	return EFI_SUCCESS;
}
