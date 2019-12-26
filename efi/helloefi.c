#include <efi.h>
#include <efilib.h>
 
//EFI_STATUS
//EFIAPI
//efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
//{
//    InitializeLib(ImageHandle, SystemTable);
//    Print(L"Hello, world!\n");
//    return EFI_SUCCESS;
//}
//
//
EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
	EFI_STATUS Status;
	UINTN Index;
	EFI_INPUT_KEY Key;
	CHAR16 StrBuffer[3] = {0};
	SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Index);
	Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
	StrBuffer[0] = Key.UnicodeChar;
	StrBuffer[1] = '\n';
	SystemTable->ConOut->OutputString(SystemTable->ConOut, StrBuffer);
	return Status;
}
