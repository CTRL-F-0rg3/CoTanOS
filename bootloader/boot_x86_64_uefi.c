#include <efi.h>
#include <efilib.h>

#define KERNEL_LOAD_ADDR 0x100000ULL
#define BOOT_INFO_MAGIC  0x424F4F5436342121ULL

typedef struct {
    UINT64 magic;
    UINT32 mem_lower;
    UINT32 mem_upper;
    UINT64 fb_ptr;
    UINT32 fb_width;
    UINT32 fb_height;
    UINT32 _pad;
} boot_info_t;

extern void jump_to_kernel(UINT64 entry, UINT64 boot_info, UINT64 system_table);

EFI_STATUS efi_main(EFI_HANDLE ih, EFI_SYSTEM_TABLE *st) {
    InitializeLib(ih, st);
    Print(L"[CoTanOS UEFI] bootloader start\n");

    EFI_LOADED_IMAGE *li = NULL;
    EFI_STATUS s = uefi_call_wrapper(BS->HandleProtocol, 3,
                                     ih, &LoadedImageProtocol, (void**)&li);
    if (EFI_ERROR(s)) { Print(L"HandleProtocol: %r\n", s); return s; }

    EFI_FILE *root = LibOpenRoot(li->DeviceHandle);
    if (!root) { Print(L"LibOpenRoot fail\n"); return EFI_LOAD_ERROR; }

    EFI_FILE *kf = NULL;
    s = uefi_call_wrapper(root->Open, 5, root, &kf, L"kernel.bin",
                          EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(s)) { Print(L"kernel.bin not found: %r\n", s); return s; }

    EFI_FILE_INFO *fi = LibFileInfo(kf);
    UINT64 size = fi->FileSize;
    FreePool(fi);
    Print(L"[CoTanOS UEFI] kernel.bin = %lu bytes\n", size);

    EFI_PHYSICAL_ADDRESS dst = KERNEL_LOAD_ADDR;
    UINTN pages = (size + 0xFFF) / 0x1000;
    s = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress,
                          EfiLoaderData, pages, &dst);
    if (EFI_ERROR(s)) { Print(L"AllocatePages: %r\n", s); return s; }

    UINTN rd = size;
    s = uefi_call_wrapper(kf->Read, 3, kf, &rd, (void*)KERNEL_LOAD_ADDR);
    if (EFI_ERROR(s) || rd != size) {
        Print(L"Read fail: %r\n", s); return EFI_LOAD_ERROR;
    }
    uefi_call_wrapper(kf->Close, 1, kf);
    Print(L"[CoTanOS UEFI] loaded @0x%lx\n", KERNEL_LOAD_ADDR);

    boot_info_t *bi = AllocatePool(sizeof(boot_info_t));
    bi->magic     = BOOT_INFO_MAGIC;
    bi->mem_lower = 640;
    bi->mem_upper = 0;
    bi->fb_ptr    = 0;
    bi->fb_width  = 0;
    bi->fb_height = 0;
    bi->_pad      = 0;

    UINTN msz = 0, mkey = 0, dsz = 0;
    UINT32 dver = 0;
    uefi_call_wrapper(BS->GetMemoryMap, 5, &msz, NULL, &mkey, &dsz, &dver);
    msz += 2 * dsz;
    EFI_MEMORY_DESCRIPTOR *mm = AllocatePool(msz);
    s = uefi_call_wrapper(BS->GetMemoryMap, 5, &msz, mm, &mkey, &dsz, &dver);
    if (EFI_ERROR(s)) { Print(L"GetMemoryMap: %r\n", s); return s; }

    s = uefi_call_wrapper(BS->ExitBootServices, 2, ih, mkey);
    if (EFI_ERROR(s)) { Print(L"ExitBootServices: %r\n", s); return s; }

    jump_to_kernel(KERNEL_LOAD_ADDR, (UINT64)bi, (UINT64)st);

    for (;;) { __asm__ __volatile__("hlt"); }
    return EFI_SUCCESS;
}
