#include <Uefi.h>

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ih, IN EFI_SYSTEM_TABLE *st) {
	UINTN index;
	EFI_INPUT_KEY key;
	CHAR16 strBuffer[3] = {};
	st->BootServices->WaitForEvent(1, &st->ConIn->WaitForKey, &index);
	st->ConIn->ReadKeyStroke(st->ConIn, &key);
	strBuffer[0] = key.UnicodeChar;
	strBuffer[1] = '\n';
	st->ConOut->OutputString(st->ConOut, strBuffer);
	return EFI_SUCCESS;
}
