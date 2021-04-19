#include <stdint.h>
#ifndef _NTIMAGE_
#define _NTIMAGE_

//
// Image Formats
//
#define IMAGE_DOS_SIGNATURE                 0x5A4D
#define IMAGE_OS2_SIGNATURE                 0x454E
#define IMAGE_OS2_SIGNATURE_LE              0x454C
#define IMAGE_VXD_SIGNATURE                 0x454C
#define IMAGE_NT_SIGNATURE                  0x00004550

//
// Image architectures
//
#define IMAGE_FILE_MACHINE_AM33       0x1d3
#define IMAGE_FILE_MACHINE_AMD64      0x8664
#define IMAGE_FILE_MACHINE_ARM        0x1c0
#define IMAGE_FILE_MACHINE_ARMV7      0x1c4
#define IMAGE_FILE_MACHINE_EBC        0xebc
#define IMAGE_FILE_MACHINE_I386       0x14c
#define IMAGE_FILE_MACHINE_IA64       0x200
#define IMAGE_FILE_MACHINE_M32R       0x9041
#define IMAGE_FILE_MACHINE_MIPS16     0x266
#define IMAGE_FILE_MACHINE_MIPSFPU    0x366
#define IMAGE_FILE_MACHINE_MIPSFPU16  0x466
#define IMAGE_FILE_MACHINE_POWERPC    0x1f0
#define IMAGE_FILE_MACHINE_POWERPCFP  0x1f1
#define IMAGE_FILE_MACHINE_R4000      0x166
#define IMAGE_FILE_MACHINE_SH3        0x1a2
#define IMAGE_FILE_MACHINE_SH3E       0x01a4
#define IMAGE_FILE_MACHINE_SH3DSP     0x1a3
#define IMAGE_FILE_MACHINE_SH4        0x1a6
#define IMAGE_FILE_MACHINE_SH5        0x1a8
#define IMAGE_FILE_MACHINE_THUMB      0x1c2
#define IMAGE_FILE_MACHINE_WCEMIPSV2  0x169
#define IMAGE_FILE_MACHINE_R3000      0x162
#define IMAGE_FILE_MACHINE_R10000     0x168
#define IMAGE_FILE_MACHINE_ALPHA      0x184
#define IMAGE_FILE_MACHINE_ALPHA64    0x0284
#define IMAGE_FILE_MACHINE_AXP64      IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_CEE        0xC0EE
#define IMAGE_FILE_MACHINE_TRICORE    0x0520
#define IMAGE_FILE_MACHINE_CEF        0x0CEF

#pragma pack(push,1)
//
// DOS Image Header Format
//
typedef struct _IMAGE_DOS_HEADER {
  uint16_t e_magic;
  uint16_t e_cblp;
  uint16_t e_cp;
  uint16_t e_crlc;
  uint16_t e_cparhdr;
  uint16_t e_minalloc;
  uint16_t e_maxalloc;
  uint16_t e_ss;
  uint16_t e_sp;
  uint16_t e_csum;
  uint16_t e_ip;
  uint16_t e_cs;
  uint16_t e_lfarlc;
  uint16_t e_ovno;
  uint16_t e_res[4];
  uint16_t e_oemid;
  uint16_t e_oeminfo;
  uint16_t e_res2[10];
  int32_t e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;


//
// Export/Import Format
//
typedef struct  _IMAGE_EXPORT_DIRECTORY {
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t Name;
  uint32_t Base;
  uint32_t NumberOfFunctions;
  uint32_t NumberOfNames;
  uint32_t AddressOfFunctions;
  uint32_t AddressOfNames;
  uint32_t AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

//
// Resource Data Entry Format
//
typedef struct _IMAGE_RESOURCE_DATA_ENTRY {
  uint32_t OffsetToData;
  uint32_t Size;
  uint32_t CodePage;
  uint32_t Reserved;
} IMAGE_RESOURCE_DATA_ENTRY, *PIMAGE_RESOURCE_DATA_ENTRY;

//
// Load Configuration Directory Entry Format
//
typedef struct  {
  uint32_t Size;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t GlobalFlagsClear;
  uint32_t GlobalFlagsSet;
  uint32_t CriticalSectionDefaultTimeout;
  uint32_t DeCommitFreeBlockThreshold;
  uint32_t DeCommitTotalFreeThreshold;
  uint32_t LockPrefixTable;
  uint32_t MaximumAllocationSize;
  uint32_t VirtualMemoryThreshold;
  uint32_t ProcessHeapFlags;
  uint32_t ProcessAffinityMask;
  uint16_t CSDVersion;
  uint16_t Reserved1;
  uint32_t EditList;
  uint32_t SecurityCookie;
  uint32_t SEHandlerTable;
  uint32_t SEHandlerCount;
} IMAGE_LOAD_CONFIG_DIRECTORY32, *PIMAGE_LOAD_CONFIG_DIRECTORY32;

typedef struct  {
  uint32_t Size;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t GlobalFlagsClear;
  uint32_t GlobalFlagsSet;
  uint32_t CriticalSectionDefaultTimeout;
  uint64_t DeCommitFreeBlockThreshold;
  uint64_t DeCommitTotalFreeThreshold;
  uint64_t LockPrefixTable;
  uint64_t MaximumAllocationSize;
  uint64_t VirtualMemoryThreshold;
  uint64_t ProcessAffinityMask;
  uint32_t ProcessHeapFlags;
  uint16_t CSDVersion;
  uint16_t Reserved1;
  uint64_t EditList;
  uint64_t SecurityCookie;
  uint64_t SEHandlerTable;
  uint64_t SEHandlerCount;
} IMAGE_LOAD_CONFIG_DIRECTORY64, *PIMAGE_LOAD_CONFIG_DIRECTORY64;

#ifdef _WIN64
typedef IMAGE_LOAD_CONFIG_DIRECTORY64     IMAGE_LOAD_CONFIG_DIRECTORY;
typedef PIMAGE_LOAD_CONFIG_DIRECTORY64    PIMAGE_LOAD_CONFIG_DIRECTORY;
#else
typedef IMAGE_LOAD_CONFIG_DIRECTORY32     IMAGE_LOAD_CONFIG_DIRECTORY;
typedef PIMAGE_LOAD_CONFIG_DIRECTORY32    PIMAGE_LOAD_CONFIG_DIRECTORY;
#endif

//
// Base Relocation Format
//
typedef struct  _IMAGE_BASE_RELOCATION {
  uint32_t VirtualAddress;
  uint32_t SizeOfBlock;
} IMAGE_BASE_RELOCATION, *PIMAGE_BASE_RELOCATION;

//
// Resource Format
// not used for anything, but we could use it to store
// a boot logo like windows XP did.
//
typedef struct _IMAGE_RESOURCE_DIRECTORY {
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint16_t NumberOfNamedEntries;
  uint16_t NumberOfIdEntries;
} IMAGE_RESOURCE_DIRECTORY, *PIMAGE_RESOURCE_DIRECTORY;

#define IMAGE_RESOURCE_NAME_IS_STRING        0x80000000
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY     0x80000000

typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY
{
  union
  {
    struct
    {
      uint32_t NameOffset : 31;
      uint32_t NameIsString : 1;
    } DUMMYSTRUCTNAME;
    uint32_t Name;
    uint16_t Id;
  } DUMMYUNIONNAME;
  union
  {
    uint32_t OffsetToData;
    struct
    {
      uint32_t OffsetToDirectory : 31;
      uint32_t DataIsDirectory : 1;
    } DUMMYSTRUCTNAME2;
  } DUMMYUNIONNAME2;
} IMAGE_RESOURCE_DIRECTORY_ENTRY, *PIMAGE_RESOURCE_DIRECTORY_ENTRY;

typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING {
  uint16_t Length;
  int8_t NameString[1];
} IMAGE_RESOURCE_DIRECTORY_STRING, *PIMAGE_RESOURCE_DIRECTORY_STRING;

//
// Section Header Format
//
#define IMAGE_SIZEOF_SHORT_NAME              8
#define IMAGE_SIZEOF_SECTION_HEADER          40

typedef struct _IMAGE_SECTION_HEADER {
  uint8_t Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    uint32_t PhysicalAddress;
    uint32_t VirtualSize;
  } Misc;
  uint32_t VirtualAddress;
  uint32_t SizeOfRawData;
  uint32_t PointerToRawData;
  uint32_t PointerToRelocations;
  uint32_t PointerToLinenumbers;
  uint16_t NumberOfRelocations;
  uint16_t NumberOfLinenumbers;
  uint32_t Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

//
// Section Characteristics
//
#define IMAGE_SCN_CNT_CODE                   0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA       0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA     0x00000080

#define IMAGE_SCN_LNK_NRELOC_OVFL            0x01000000
#define IMAGE_SCN_MEM_DISCARDABLE            0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED             0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED              0x08000000
#define IMAGE_SCN_MEM_SHARED                 0x10000000
#define IMAGE_SCN_MEM_EXECUTE                0x20000000
#define IMAGE_SCN_MEM_READ                   0x40000000
#define IMAGE_SCN_MEM_WRITE                  0x80000000

//
// File Header Format
//
#define IMAGE_SIZEOF_FILE_HEADER             20

typedef struct _IMAGE_FILE_HEADER {
  uint16_t Machine;
  uint16_t NumberOfSections;
  uint32_t TimeDateStamp;
  uint32_t PointerToSymbolTable;
  uint32_t NumberOfSymbols;
  uint16_t SizeOfOptionalHeader;
  uint16_t Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

//
// File Characteristics
//
#define IMAGE_FILE_RELOCS_STRIPPED           0x0001
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008
#define IMAGE_FILE_AGGRESIVE_WS_TRIM         0x0010
#define IMAGE_FILE_LARGE_ADDRESS_AWARE       0x0020
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080
#define IMAGE_FILE_32BIT_MACHINE             0x0100
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400
#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800
#define IMAGE_FILE_SYSTEM                    0x1000
#define IMAGE_FILE_DLL                       0x2000
#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000

//
// Directory format.
//
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_DATA_DIRECTORY {
  uint32_t VirtualAddress;
  uint32_t Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

//
// Optional Header Formats
//
typedef struct _IMAGE_OPTIONAL_HEADER {
  uint16_t Magic;
  uint8_t MajorLinkerVersion;
  uint8_t MinorLinkerVersion;
  uint32_t SizeOfCode;
  uint32_t SizeOfInitializedData;
  uint32_t SizeOfUninitializedData;
  uint32_t AddressOfEntryPoint;
  uint32_t BaseOfCode;
  uint32_t BaseOfData;
  uint32_t ImageBase;
  uint32_t SectionAlignment;
  uint32_t FileAlignment;
  uint16_t MajorOperatingSystemVersion;
  uint16_t MinorOperatingSystemVersion;
  uint16_t MajorImageVersion;
  uint16_t MinorImageVersion;
  uint16_t MajorSubsystemVersion;
  uint16_t MinorSubsystemVersion;
  uint32_t Win32VersionValue;
  uint32_t SizeOfImage;
  uint32_t SizeOfHeaders;
  uint32_t CheckSum;
  uint16_t Subsystem;
  uint16_t DllCharacteristics;
  uint32_t SizeOfStackReserve;
  uint32_t SizeOfStackCommit;
  uint32_t SizeOfHeapReserve;
  uint32_t SizeOfHeapCommit;
  uint32_t LoaderFlags;
  uint32_t NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

// we don't need this.
// *************************************
// typedef struct _IMAGE_ROM_OPTIONAL_HEADER {
//   USHORT Magic;
//   UCHAR MajorLinkerVersion;
//   UCHAR MinorLinkerVersion;
//   ULONG SizeOfCode;
//   ULONG SizeOfInitializedData;
//   ULONG SizeOfUninitializedData;
//   ULONG AddressOfEntryPoint;
//   ULONG BaseOfCode;
//   ULONG BaseOfData;
//   ULONG BaseOfBss;
//   ULONG GprMask;
//   ULONG CprMask[4];
//   ULONG GpValue;
// } IMAGE_ROM_OPTIONAL_HEADER, *PIMAGE_ROM_OPTIONAL_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
  uint16_t Magic;
  uint8_t MajorLinkerVersion;
  uint8_t MinorLinkerVersion;
  uint32_t SizeOfCode;
  uint32_t SizeOfInitializedData;
  uint32_t SizeOfUninitializedData;
  uint32_t AddressOfEntryPoint;
  uint32_t BaseOfCode;
  uint64_t ImageBase;
  uint32_t SectionAlignment;
  uint32_t FileAlignment;
  uint16_t MajorOperatingSystemVersion;
  uint16_t MinorOperatingSystemVersion;
  uint16_t MajorImageVersion;
  uint16_t MinorImageVersion;
  uint16_t MajorSubsystemVersion;
  uint16_t MinorSubsystemVersion;
  uint32_t Win32VersionValue;
  uint32_t SizeOfImage;
  uint32_t SizeOfHeaders;
  uint32_t CheckSum;
  uint16_t Subsystem;
  uint16_t DllCharacteristics;
  uint64_t SizeOfStackReserve;
  uint64_t SizeOfStackCommit;
  uint64_t SizeOfHeapReserve;
  uint64_t SizeOfHeapCommit;
  uint32_t LoaderFlags;
  uint32_t NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

//
// Format Identifier Magics
//
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC      0x10b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC      0x20b
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC       0x107

#ifdef _WIN64
typedef IMAGE_OPTIONAL_HEADER64             IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER64            PIMAGE_OPTIONAL_HEADER;
#define IMAGE_NT_OPTIONAL_HDR_MAGIC         IMAGE_NT_OPTIONAL_HDR64_MAGIC
#else
typedef IMAGE_OPTIONAL_HEADER32             IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER32            PIMAGE_OPTIONAL_HEADER;
#define IMAGE_NT_OPTIONAL_HDR_MAGIC         IMAGE_NT_OPTIONAL_HDR32_MAGIC
#endif

//
// Nt Header Formats
//
typedef struct _IMAGE_NT_HEADERS64 {
  uint32_t Signature;
  IMAGE_FILE_HEADER FileHeader;
  IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64;

typedef struct _IMAGE_NT_HEADERS {
  uint32_t Signature;
  IMAGE_FILE_HEADER FileHeader;
  IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32;

#ifdef _WIN64
typedef IMAGE_NT_HEADERS64                  IMAGE_NT_HEADERS;
#else
typedef IMAGE_NT_HEADERS32                  IMAGE_NT_HEADERS;
#endif

#ifndef _NTDDK_

typedef struct _IMAGE_NT_HEADERS *PIMAGE_NT_HEADERS32;
typedef struct _IMAGE_NT_HEADERS64 *PIMAGE_NT_HEADERS64;

#ifdef _WIN64
typedef PIMAGE_NT_HEADERS64                 PIMAGE_NT_HEADERS;
#else
typedef PIMAGE_NT_HEADERS32                 PIMAGE_NT_HEADERS;
#endif

#endif /* _NTDDK_ */

//
// Retreives the first image section header from the Nt Header
//
#define IMAGE_FIRST_SECTION( NtHeader )                \
  ((PIMAGE_SECTION_HEADER) ((ULONG_PTR)(NtHeader) +    \
   FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +  \
   ((NtHeader))->FileHeader.SizeOfOptionalHeader))

//
// Subsystems
//
#define IMAGE_SUBSYSTEM_UNKNOWN                         0 
#define IMAGE_SUBSYSTEM_NATIVE                          1
#define IMAGE_SUBSYSTEM_WINDOWS_GUI                     2
#define IMAGE_SUBSYSTEM_WINDOWS_CUI                     3
#define IMAGE_SUBSYSTEM_OS2_CUI                         5
#define IMAGE_SUBSYSTEM_POSIX_CUI                       7
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS                  8
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI                  9
#define IMAGE_SUBSYSTEM_EFI_APPLICATION                 10
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER         11
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER              12
#define IMAGE_SUBSYSTEM_EFI_ROM                         13
#define IMAGE_SUBSYSTEM_XBOX                            14
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION        16
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG               17

//
// Dll Characteristics
//
#define IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA        0x0020
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE           0x0040
#define IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY        0x0080
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT              0x0100
#define IMAGE_DLLCHARACTERISTICS_NO_ISOLATION           0x0200
#define IMAGE_DLLCHARACTERISTICS_NO_SEH                 0x0400
#define IMAGE_DLLCHARACTERISTICS_NO_BIND                0x0800
#define IMAGE_DLLCHARACTERISTICS_APPCONTAINER           0x1000
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER             0x2000
#define IMAGE_DLLCHARACTERISTICS_GUARD_CF               0x4000
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE  0x8000

//
// Loader Flags
//
#define IMAGE_LOADER_FLAGS_COMPLUS       0x00000001
#define IMAGE_LOADER_FLAGS_SYSTEM_GLOBAL 0x01000000

//
// Directory Entry Specifiers
//
#define IMAGE_DIRECTORY_ENTRY_EXPORT          0
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8
#define IMAGE_DIRECTORY_ENTRY_TLS             9
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11
#define IMAGE_DIRECTORY_ENTRY_IAT            12
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14

//
// Import Formats
//
typedef struct _IMAGE_IMPORT_BY_NAME {
  uint16_t Hint;
  uint8_t Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA64 {
  union {
    uint64_t ForwarderString;
    uint64_t Function;
    uint64_t Ordinal;
    uint64_t AddressOfData;
  } u1;
} IMAGE_THUNK_DATA64, *PIMAGE_THUNK_DATA64;

typedef struct _IMAGE_THUNK_DATA32 {
  union {
    uint32_t ForwarderString;
    uint32_t Function;
    uint32_t Ordinal;
    uint32_t AddressOfData;
  } u1;
} IMAGE_THUNK_DATA32, *PIMAGE_THUNK_DATA32;

#define IMAGE_ORDINAL_FLAG64 0x8000000000000000ULL
#define IMAGE_ORDINAL_FLAG32 0x80000000
#define IMAGE_ORDINAL64(Ordinal) (Ordinal & 0xffff)
#define IMAGE_ORDINAL32(Ordinal) (Ordinal & 0xffff)
#define IMAGE_SNAP_BY_ORDINAL64(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG64) != 0)
#define IMAGE_SNAP_BY_ORDINAL32(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG32) != 0)

//
// Thread Local Storage (TLS)
//
// typedef
// VOID
// (NTAPI *PIMAGE_TLS_CALLBACK) (
//   PVOID DllHandle,
//   ULONG Reason,
//   PVOID Reserved);

typedef struct _IMAGE_TLS_DIRECTORY64 {
  uint64_t StartAddressOfRawData;
  uint64_t EndAddressOfRawData;
  uint64_t AddressOfIndex;
  uint64_t AddressOfCallBacks;
  uint32_t SizeOfZeroFill;
  uint32_t Characteristics;
} IMAGE_TLS_DIRECTORY64, *PIMAGE_TLS_DIRECTORY64;

typedef struct _IMAGE_TLS_DIRECTORY32 {
  uint32_t StartAddressOfRawData;
  uint32_t EndAddressOfRawData;
  uint32_t AddressOfIndex;
  uint32_t AddressOfCallBacks;
  uint32_t SizeOfZeroFill;
  uint32_t Characteristics;
} IMAGE_TLS_DIRECTORY32, *PIMAGE_TLS_DIRECTORY32;

#ifdef _WIN64
#define IMAGE_ORDINAL_FLAG              IMAGE_ORDINAL_FLAG64
#define IMAGE_ORDINAL(Ordinal)          IMAGE_ORDINAL64(Ordinal)
typedef IMAGE_THUNK_DATA64              IMAGE_THUNK_DATA;
typedef PIMAGE_THUNK_DATA64             PIMAGE_THUNK_DATA;
#define IMAGE_SNAP_BY_ORDINAL(Ordinal)  IMAGE_SNAP_BY_ORDINAL64(Ordinal)
typedef IMAGE_TLS_DIRECTORY64           IMAGE_TLS_DIRECTORY;
typedef PIMAGE_TLS_DIRECTORY64          PIMAGE_TLS_DIRECTORY;
#else
#define IMAGE_ORDINAL_FLAG              IMAGE_ORDINAL_FLAG32
#define IMAGE_ORDINAL(Ordinal)          IMAGE_ORDINAL32(Ordinal)
typedef IMAGE_THUNK_DATA32              IMAGE_THUNK_DATA;
typedef PIMAGE_THUNK_DATA32             PIMAGE_THUNK_DATA;
#define IMAGE_SNAP_BY_ORDINAL(Ordinal)  IMAGE_SNAP_BY_ORDINAL32(Ordinal)
typedef IMAGE_TLS_DIRECTORY32           IMAGE_TLS_DIRECTORY;
typedef PIMAGE_TLS_DIRECTORY32          PIMAGE_TLS_DIRECTORY;
#endif

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
  union {
    uint32_t Characteristics;
    uint32_t OriginalFirstThunk;
  } DUMMYUNIONNAME;
  uint32_t TimeDateStamp;
  uint32_t ForwarderChain;
  uint32_t Name;
  uint32_t FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR, *PIMAGE_IMPORT_DESCRIPTOR;

#pragma pack(pop)

#endif /* _NTIMAGE_ */
