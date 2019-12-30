#include <efi.h>
#include <efilib.h>

int funca() {
	return 0;
}

EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
	Print(L"aaaa\n");
	EFI_STATUS Status;
	int t = funca();
	UINTN Index;
	EFI_INPUT_KEY Key;
	CHAR16 StrBuffer[3] = {0};
	SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Index);
	Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
	StrBuffer[0] = Key.UnicodeChar;
	StrBuffer[1] = '\n';
	Print(StrBuffer);
	SystemTable->ConOut->OutputString(SystemTable->ConOut, StrBuffer);
	return Status;
} 

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello, world!\n");
    return UefiMain(ImageHandle, SystemTable);
}


