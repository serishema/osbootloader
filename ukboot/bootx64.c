/** @file
  Brief Description of UEFI MyHelloWorld
  Detailed Description of UEFI MyHelloWorld
  Copyright for UEFI MyHelloWorld
  License for UEFI MyHelloWorld
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Guid/FileInfo.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/GraphicsOutput.h>
#include <intrin.h>

#include "ntimage.h"
#include "bootparams.h"

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

void PrintSerial(char *str) {
	int i = 0;
	while (str[i] != 0) {
		__outbyte(0x3F8,str[i]);
		i++;
	}
}

/* Page table definitions  */

typedef struct {
	uint64_t present : 1; // 1
	uint64_t rw : 1;    // 2
	uint64_t user : 1; // 3
	uint64_t pwt : 1;  // 4
	uint64_t pcd : 1;  // 5
	uint64_t accessed : 1; // 6
	uint64_t ignored0 : 1;  // 7
	uint64_t reserved0 : 1; // 8
	uint64_t ignored1 : 4; // 12 
	uint64_t pdpt_addr : 36; // 48
	uint64_t reserved1 : 4; // 52
	uint64_t ignored : 11; // 63
	uint64_t execute_disable : 1; // 64
} PML4_ENTRY; // 64 bits -- verified with SDM

typedef struct {
	uint64_t present : 1; // 1
	uint64_t rw : 1;  // 2
	uint64_t user : 1; // 3
	uint64_t pwt : 1; // 4
	uint64_t pcd : 1; // 5
	uint64_t accessed : 1; // 6  
	uint64_t ignored0 : 1; // 7
	uint64_t page_size : 1; // 8 
	uint64_t ignored1 : 4; // 12 
	uint64_t pd_addr : 36; // 48
	uint64_t reserved0 : 4; // 52
	uint64_t ignored2 : 11; // 63
	uint64_t execute_disable : 1; // 64 
} PDPTE_ENTRY; // 64 bits -- checked against SDM

typedef struct {
	uint64_t present : 1; // 1
	uint64_t rw : 1; // 2
	uint64_t user : 1;  // 3
	uint64_t pwt : 1; // 4
	uint64_t pcd : 1; // 5
	uint64_t accessed : 1; // 6
	uint64_t ignored0 : 1; // 7
	uint64_t page_size : 1; // 8 
	uint64_t ignored1 : 4; // 12
	uint64_t pt_addr : 36; // 48
	uint64_t reserved0 : 4; // 52
	uint64_t ignoreed1 : 11; // 63
	uint64_t execute_disabled : 1; // 64
} PDIR_ENTRY; // 64 bits.

typedef struct {
	uint64_t present : 1;  // 1
	uint64_t rw : 1; // 2
	uint64_t user : 1;  // 3
	uint64_t pwt : 1;  // 4
	uint64_t pcd : 1;  // 5 
	uint64_t accessed : 1; // 6
	uint64_t dirty : 1; // 7
	uint64_t pat : 1;  // 8 
	uint64_t global : 1; // 9
	uint64_t ignored0 : 3; // 12 
	uint64_t page_addr : 36; // 48 
	uint64_t reserved0 : 4; // 52 
	uint64_t ignored1 : 7;  // 59 
	uint64_t prot_key : 4;  // 63 
	uint64_t execute_disable : 1; // 64
} PT_ENTRY; // 64 bits 

// zeros a page of memory.
void ZeroPage(void* vaddr) {
	uint8_t *p = (uint8_t*)vaddr;
	for (int i = 0; i < 4096; i++) {
		p[i] = 0;
	}
}

/* Boot-time bitmap allocator */
uint8_t* freePageBitmap;
uint64_t freePageBitmapSize; // in bytes 

uint64_t freeMemory; // in Pages.
uint64_t segBase; 

#define PAGE_SIZE 4096

static uint8_t FreePageBitmapGet(uint64_t index) {
	if (index > freePageBitmapSize * 8) {
		return 0;
	}
	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0b10000000 >> bitIndex;

	return ((freePageBitmap[byteIndex] & bitIndexer) > 0);
}

static void FreePageBitmapSet(uint64_t index, uint8_t value) {
	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0b10000000 >> bitIndex;

	freePageBitmap[byteIndex] &= ~bitIndexer;
	if (value) {
		freePageBitmap[byteIndex] |= bitIndexer;
	}
}

void InitMmFromEfi(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapSize, uint64_t mMapDescrSize) {
	uint64_t mMapEntryCount = mMapSize / mMapDescrSize;

	void* largestFreeSegment = NULL;
	uint64_t largestFreeSegSize = 0; // In PAGES, Not bytes.

	for (int i = 0; i < mMapEntryCount; i++) {
		EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescrSize));
		if (desc->Type == 7) { // EfiConventionalMemory
			if (desc->NumberOfPages > largestFreeSegSize) {
				largestFreeSegment = (void*) desc->PhysicalStart;
				largestFreeSegSize = desc->NumberOfPages;
			}
		}
	}

	// initalize the bitmap at the base of the largest free memory
	// segment and set all pages in use.
	freePageBitmap = (uint8_t*) largestFreeSegment;
	freePageBitmapSize = largestFreeSegSize / 8 ; // each bit is a page 

	for (int i = 0; i < freePageBitmapSize; i++) {
		freePageBitmap[i] = 0xff; // set all bits
	}

	segBase = (uint64_t) freePageBitmap;
	uint64_t freePageBitmapSizeInPages = (freePageBitmapSize / PAGE_SIZE) + PAGE_SIZE;

	for (uint64_t i = freePageBitmapSizeInPages; i < largestFreeSegSize; i++) {
		FreePageBitmapSet(i, 0);  // mark the rest of the segment available.
	}

	freeMemory = largestFreeSegSize - freePageBitmapSizeInPages;
}
// note to self: I intend to keep these allocations
//               into the kernel when it's finished loading
//               so pass the base address of the segment
//               this allocates into the kernel via 
//               the bootparams struct.
void* MmAllocPhysicalPage() {
	uint64_t index = 0;
	uint64_t address = 0;
	for (;index < freePageBitmapSize * 8;index++) {
		if (FreePageBitmapGet(index)) {
			continue;
		} else {
			address = (index * PAGE_SIZE) + segBase;
			FreePageBitmapSet(index,1);
			freeMemory--;
			return (void*)address;
			break;
		}
	}
	return (void*)address;
}

/* Virtual Memory */
PML4_ENTRY* KernelPML4;

void MapMem(uint64_t phys,void* virt) {
	
	uint64_t va = (uint64_t)virt;
	uint64_t PML4Index = (va >> 39) & 0x1ff;
	uint64_t PDPTRIndex = (va >> 30) & 0x1ff;
	uint64_t PDIRIndex = (va >> 21) & 0x1ff;
	uint64_t PTIndex = (va >> 12) & 0x1ff;

	PDPTE_ENTRY* PDPTR = NULL;

	// PML4
	if (KernelPML4[PML4Index].present == 0) {
		PDPTR = (PDPTE_ENTRY*) MmAllocPhysicalPage();
		ZeroPage(PDPTR);
		KernelPML4[PML4Index].pdpt_addr = ((uint64_t)PDPTR) >> 12;
		KernelPML4[PML4Index].present = 1;
		KernelPML4[PML4Index].rw = 1;
	}
	else {
		PDPTR = (PDPTE_ENTRY*)(KernelPML4[PML4Index].pdpt_addr << 12);
	}
	PDIR_ENTRY* PDIR = NULL;
	// Page Directory Pointer Table 
	if (PDPTR[PDPTRIndex].present == 0) {
		PDIR = (PDIR_ENTRY*) MmAllocPhysicalPage();
		ZeroPage(PDIR);
		PDPTR[PDPTRIndex].pd_addr = ((uint64_t)PDIR) >> 12;
		PDPTR[PDPTRIndex].present = 1;
		PDPTR[PDPTRIndex].rw = 1;
	}
	else {
		PDIR = (PDIR_ENTRY*)(PDPTR[PDPTRIndex].pd_addr << 12);
	}
	PT_ENTRY* PTABLE = NULL;
	// Page Directory
	if (PDIR[PDIRIndex].present == 0) {
		PTABLE = (PT_ENTRY*) MmAllocPhysicalPage();
		ZeroPage(PTABLE);
		PDIR[PDIRIndex].pt_addr = ((uint64_t)PTABLE) >> 12;
		PDIR[PDIRIndex].present = 1;
		PDIR[PDIRIndex].rw = 1;
	}
	else {
		PTABLE = (PT_ENTRY*) (PDIR[PDIRIndex].pt_addr << 12);
	}
	
	// now the actual page table manipulation

	PTABLE[PTIndex].page_addr = (phys & 0xFFFFFF000) >> 12;
	PTABLE[PTIndex].present = 1;
	PTABLE[PTIndex].rw = 1;
}

void IdentityMapMemory(EFI_MEMORY_DESCRIPTOR* MemMap,uint64_t mMapSize,uint64_t mMapDescrSize) {
	uint64_t mMapEntryCount = mMapSize / mMapDescrSize;
	KernelPML4 = (PML4_ENTRY*) MmAllocPhysicalPage();
	ZeroPage(KernelPML4);

	for (int i = 0; i < mMapEntryCount; i++) {
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MemMap + (i * mMapDescrSize));
		for (uint64_t j = 0; j < desc->NumberOfPages; j++) {
			uint64_t a = ((uint64_t)desc->PhysicalStart) + ( j * 4096);
			MapMem(a, (void*)a);
		}
	}
	// don't call WriteCR3 yet.
	// we need to tidy up and call ExitBootServices first.
}

/* Ram Disk. 
   14/04/2021 We are going to need this to kick off user mode drivers 
              so I might as well do this now.
*/
typedef struct {
	uint16_t filename[12]; // only dos-compatible file names
	uint8_t *file_data;
	uint64_t file_size;
	uint64_t file_ptr; // this will only work for a single thread.
	struct RAMDISK_FILE* next;
} RAMDISK_FILE;

RAMDISK_FILE *BootTimeRamDisk = NULL;

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

void RamDiskAddFile(EFI_SYSTEM_TABLE* SysTable,EFI_HANDLE ImageHandle, CHAR16 *filename) {
	RAMDISK_FILE *newFile = NULL;
	Print(L"Adding %s to ramdisk\n",filename);
	if (BootTimeRamDisk == NULL) {
		Print(L"Empty Ramdisk case\n");
		SysTable->BootServices->AllocatePool(EfiLoaderData,sizeof(RAMDISK_FILE), (VOID**) &BootTimeRamDisk);
		newFile = BootTimeRamDisk;
	} else {
		RAMDISK_FILE *current = BootTimeRamDisk;
		Print(L"Existing Files Case\n");
		while (current->next != NULL) {
			current = (RAMDISK_FILE*) current->next;
		}
		SysTable->BootServices->AllocatePool(EfiLoaderData,sizeof(RAMDISK_FILE), (VOID**) &current->next);
		newFile = (RAMDISK_FILE*) current->next; 
	}

	newFile->next = NULL;
	newFile->file_data = NULL;

	EFI_FILE* EfiFileHandle = LoadFile(NULL,filename,ImageHandle,SysTable);
	if (EfiFileHandle == NULL) {
		Print(L"Failed to add %s to ramdisk\n",filename);
		while(1);
	}

  /* -=Todo=-: Check error state of all of these calls to UEFI. */
  EFI_STATUS status;
	UINTN FileInfoSize = 0;
	EFI_FILE_INFO* FileInfo;
  //GUID EfiFileInfoGuid = EFI_FILE_INFO_ID;
	status = EfiFileHandle->GetInfo(EfiFileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);
  Print(L"Get File Info status %d\n",status);
  Print(L"FileInfo Size = %d\n",FileInfoSize);
	SysTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
	status = EfiFileHandle->GetInfo(EfiFileHandle, &gEfiFileInfoGuid, &FileInfoSize, (void*)FileInfo);
  Print(L"Get File Info status %d\n",status);
  Print(L"Get File Info size %d\n",FileInfo->Size);
  Print(L"filename %s\n",FileInfo->FileName);
	SysTable->BootServices->AllocatePool(EfiLoaderData,FileInfo->FileSize,(void**) &newFile->file_data);
	newFile->file_size = FileInfo->FileSize;
	UINTN readSize = newFile->file_size;
  Print(L"File size %d\n",FileInfo->FileSize);
	status = EfiFileHandle->Read(EfiFileHandle,&readSize,newFile->file_data);
  Print(L"EFI Status from Read %d\n",status);
  Print(L"Bytes read %d\n",readSize);
	EfiFileHandle->Close(EfiFileHandle);

	/* copy the filename */

	int i = 0;
	while (filename[i] != 0) {
		newFile->filename[i] = filename[i];
		i++;
	}
	newFile->filename[i] = 0;
	newFile->file_ptr = 0;

	Print(L"Added %s to ramdisk\n",filename);
	
}

void RamDiskReadFile(CHAR16 *filename,uint64_t size,void *buffer) {
	/* find the file */
	RAMDISK_FILE* current = BootTimeRamDisk;
	Print(L"Reading file %s from ramdisk \n",filename);
	while (current->next != NULL) {
		if (StrCmp (current->filename,filename) == 0) {
			break; 
		}
	}
	if (StrCmp (current->filename,filename) != 0) {
		PrintSerial("File not found reading ramdisk.\r\n");
		return;
	}

	uint8_t *dest = (uint8_t*) buffer;
	uint8_t *data = (uint8_t*) current->file_data;
	uint64_t bytesRead = 0;
	while (bytesRead < size) {
		dest[bytesRead] = data[current->file_ptr];
		current->file_ptr++;
		bytesRead++;
	}
}

void RamDiskSetFilePos(CHAR16 *filename,uint64_t position) {
	/* find the file */
	RAMDISK_FILE* current = BootTimeRamDisk;
	Print(L"Setting file position to %u\n",position);
	while (current->next != NULL) {
		if (StrCmp (current->filename,filename) == 0) {
			break; 
		}
	}
	if (StrCmp (current->filename,filename) != 0) {
		PrintSerial("File not found setting file pointer on ramdisk.\r\n");
		return;
	}

	current->file_ptr = position;
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable){
	EFI_FILE* LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
  //GUID EfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
  //GUID EfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid , (void**)&FileSystem);

	if (Directory == NULL){
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS){
		return NULL;
	}
	return LoadedFile;

}

/* Load a linux console font. */
PSF1_FONT* LoadPSF1Font(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_FILE* font = LoadFile(Directory, Path, ImageHandle, SystemTable);
	if (font == NULL) return NULL;

	PSF1_HEADER* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);

	if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1){
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if (fontHeader->mode == 1) { //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF1_FONT* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);
	finishedFont->psf1_Header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont;

}

/* Framebuffer */
Framebuffer framebuffer;
Framebuffer InitializeGOP(EFI_SYSTEM_TABLE *SysTable){
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	status = SysTable->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);
	if(EFI_ERROR(status)){
		Print(L"Unable to locate GOP\n\r");
        while(1);
	}
	else
	{
		Print(L"GOP located\n\r");
	}

	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

	return framebuffer;
	
}


const char* EFI_MEMORY_TYPE_STRINGS[] = {
   "EfiReservedMemoryType",
   "EfiLoaderCode",
   "EfiLoaderData",
   "EfiBootServicesCode",
   "EfiBootServicesData",
   "EfiRuntimeServicesCode",
   "EfiRuntimeServicesData",
   "EfiConventionalMemory",
   "EfiUnusableMemory",
   "EfiACPIReclaimMemory",
   "EfiACPIMemoryNVS",
   "EfiMemoryMappedIO",
   "EfiMemoryMappedIOPortSpace",
   "EfiPalCode",
};

EFI_STATUS EFIAPI UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Loading...\r\n"); // EFI Applications use Unicode and CRLF, a la Windows

	RamDiskAddFile(SystemTable,ImageHandle,L"UK.EXE");

	Print(L"Returned from RamDiskAddFile\n");

  IMAGE_DOS_HEADER dosHeader;
  IMAGE_NT_HEADERS64 NtImageHeaders;

    // UINTN FileInfoSize;
	// EFI_FILE_INFO* FileInfo;
	// Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
	// SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
	// Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);
  KERNEL_PARAMETER_BLOCK kParams; 
  framebuffer = InitializeGOP(SystemTable);
  kParams.fb = framebuffer;
  PSF1_FONT* newFont = LoadPSF1Font(NULL, L"zap-light16.psf", ImageHandle, SystemTable);
  kParams.console_font = newFont;

  uint32_t DescriptorVersion;
	uint64_t mapKey;
	kParams.mMap = NULL;
	kParams.mMapSize = 0;
	kParams.mMapDescSize = 0;
  EFI_STATUS status = 0;
  status = SystemTable->BootServices->GetMemoryMap(&kParams.mMapSize,kParams.mMap,&mapKey,&kParams.mMapDescSize,&DescriptorVersion);
  Print(L"GetMemoryMap Status = %d\n");
  Print(L"Memory Map Size = %d\n",kParams.mMapSize);
  Print(L"Memory Descriptor size = %d\n",kParams.mMapDescSize);
  kParams.mMapSize += 4096; // allocate an additional page. According to the spec the memory allocation to hold the map may increase the size of the map.
	status = SystemTable->BootServices->AllocatePool(EfiLoaderData,kParams.mMapSize,(void**)&kParams.mMap);
	Print(L"AllocatePool status = %d\n");
	Print(L"AllocatePool pointer = %x\n",kParams.mMap);
	status = SystemTable->BootServices->GetMemoryMap(&kParams.mMapSize,kParams.mMap,&mapKey,&kParams.mMapDescSize,&DescriptorVersion);
    Print(L"GetMemoryMap status = %d\n",status);
  /* Dump the memory descriptors to check for problems. */
	Print(L"Memory Descriptor List:\n");
	uint64_t memoryDescriptorCount = kParams.mMapSize / kParams.mMapDescSize;
	for (uint64_t i = 0; i < memoryDescriptorCount;i++) {
		EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)kParams.mMap + (i * kParams.mMapDescSize));
		Print(L"%a ",EFI_MEMORY_TYPE_STRINGS[desc->Type]);
		Print(L"%x ",desc->PhysicalStart);
		Print(L"%u \n",desc->NumberOfPages);
	}

  /********************************************
   *  Exit Boot Services After this line.     *
   * ******************************************/
  //SystemTable->BootServices->ExitBootServices(ImageHandle,mapKey);

  UINTN size = sizeof(dosHeader);
	RamDiskReadFile(L"UK.EXE", sizeof(dosHeader), &dosHeader);
  RamDiskSetFilePos(L"UK.EXE",dosHeader.e_lfanew);
	//Kernel->SetPosition(Kernel, dosHeader.e_lfanew);
    //size = sizeof(NtImageHeaders);
  RamDiskReadFile(L"UK.EXE",sizeof(NtImageHeaders),&NtImageHeaders);
	//Kernel->Read(Kernel,&size,&NtImageHeaders);

  if (NtImageHeaders.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      Print(L"UK.EXE format is bad\n");
      return 0;
  }

  if (NtImageHeaders.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) {
        Print(L"Error UK.EXE must be compiled for x64\n");
  }

  Print(L"%d sections\n",NtImageHeaders.FileHeader.NumberOfSections);

  IMAGE_SECTION_HEADER* p_ImageSectionHeaders;
  size = sizeof(IMAGE_SECTION_HEADER) * NtImageHeaders.FileHeader.NumberOfSections;
  SystemTable->BootServices->AllocatePool(EfiLoaderData,sizeof(IMAGE_SECTION_HEADER) * NtImageHeaders.FileHeader.NumberOfSections, (void*)&p_ImageSectionHeaders);

	RamDiskReadFile(L"UK.EXE",size,p_ImageSectionHeaders);
    //Kernel->Read(Kernel,&size,p_ImageSectionHeaders);

	// dump the sections to the screen for debugging.
    // this will crash if a section name is 8 characters or more.
    for (int i = 0; i < NtImageHeaders.FileHeader.NumberOfSections;i++) {
        Print(L"Section %a at 0x%x\n",p_ImageSectionHeaders[i].Name,p_ImageSectionHeaders[i].VirtualAddress + NtImageHeaders.OptionalHeader.ImageBase);
    }
    //void *pSectionData;
	                         

    for (int i = 0; i < NtImageHeaders.FileHeader.NumberOfSections;i++) {
        int pages = (p_ImageSectionHeaders[i].Misc.VirtualSize + 0x1000 - 1) / 0x1000;
        void* pSectionAddress = (void*) (NtImageHeaders.OptionalHeader.ImageBase + p_ImageSectionHeaders[i].VirtualAddress);
        SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, pSectionAddress);
        RamDiskSetFilePos(L"UK.EXE",p_ImageSectionHeaders[i].PointerToRawData);
		//Kernel->SetPosition(Kernel,p_ImageSectionHeaders[i].PointerToRawData);
		size =  p_ImageSectionHeaders[i].SizeOfRawData;
		RamDiskReadFile(L"UK.EXE",size,pSectionAddress);
        //Kernel->Read(Kernel,&size,pSectionAddress);
    }
    uint64_t KernelStartAddress = NtImageHeaders.OptionalHeader.ImageBase + NtImageHeaders.OptionalHeader.AddressOfEntryPoint;
    Print(L"Kernel Loaded Successfully\n");
    int (*Kernel_Main)(KERNEL_PARAMETER_BLOCK *kParams) =  (int (*)(KERNEL_PARAMETER_BLOCK *kParams)) KernelStartAddress;



   
  Kernel_Main(&kParams);


  return EFI_SUCCESS;
}
