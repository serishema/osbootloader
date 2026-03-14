#pragma once
#include <stdint.h>
typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;

typedef struct {
	void* BaseAddress;
	uint64_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} Framebuffer;

typedef struct {
    Framebuffer fb;
    PSF1_FONT *console_font;
	void* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
	void *StartOfKernelHeap;
	uint64_t InitalHeapSize;
} KERNEL_PARAMETER_BLOCK;