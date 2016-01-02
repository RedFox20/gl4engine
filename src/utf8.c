#include "utf8.h"
#include <stdlib.h>

// this is used to subtract 0xC0 and 0xA0 after they have been shifted
// to save us from using lots of bitmasks
// @note And don't worry - this is evaluated as a const expression :)
static const unsigned _utf_decode_magic[] = {
	0x00000000U, 0x00003080U, 0x000E2080U, 0x03C82080U
 };
 
// a temporary typedef for unsigned chars
typedef unsigned char uc;
typedef unsigned int ui;
 
// (!) This doesn't work well with system specific codepages.
//		I guess it's only useful on linux?
// UTF8 encode sizes according to starting byte
// 192 - 193: invalid
// 245 - 255: invalid
static const char _utf_is_start[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0..31
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 32..63
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 64..95
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 96..127  ASCII
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 128..159  ANSI
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 160..191
	0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 192..223
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0  // 224..255
};
 
 
// [2..4] if a possible utf8 starting byte, [0] if just a regular byte
#define is_start(c) _utf_is_start[(uc)(c)]
#define is_trailing(c) (((uc)(c)&0xC0)==0x80) // true if this is a valid utf8 trailing byte
#define not_trailing(c) (((uc)(c)&0xC0)!=0x80) // true if this is an invalid utf8 trailing byte
 
#define UNICODE_INVALID 0xFFFDul // Unicode Character 'REPLACEMENT CHARACTER'
 

/**
 * Encodes a WCHAR into an UTF8 sequence
 * @param wch Wide Char (UCS-2) to encode
 * @param dst Destination buffer at least [3] chars long
 * @return Number of bytes in the UTF8 sequence [1-3]
 */
int utf_encode(unsigned int wch, char* dst)
 {
	if (wch < 0x80) { // < 128
		dst[0] = (char)wch;
		return 1;
 	}
	else if (wch < 0x800) { // < 2048
		dst[0] = 0xC0 + (wch>>6);			// | 1100 marks it as a 2-byte sequence
		dst[1] = 0x80 + (wch & 0x3F);		// | 1000 & 1011 (generic utf sequence)
		return 2;
	}
	else if (wch < 0x10000) { // < 65536
		dst[0] = 0xE0 + (wch>>12);			// | 1110 marks it as a 3-byte sequence
		dst[1] = 0x80 + ((wch>>6) & 0x3F);	// | 1000 & 1011 (generic utf sequence)
		dst[2] = 0x80 + (wch & 0x3F);		// | 1000 & 1011 (generic utf sequence)
		return 3;
	}
	// we don't support 4-byte UTF sequences, so we encode invalid character instead
	dst[0] = (uc)0xEF; // UNICODE_INVALID 'REPLACEMENT CHARACTER' EF BF BD
	dst[1] = (uc)0xBF;
	dst[2] = (uc)0xBD;
	return 3;
 }


/**
 * Decodes an UTF8 sequence into a WCHAR
 * @param src Source UTF8 sequence
 * @param outch Destination WCHAR character
 * @return Number of bytes decoded from src: [1-4]
 */
int utf_decode(const char* src, unsigned* outch)
{
	const uc ch = *src;
	const uc chsz = is_start(ch); // is this a starting sequence?
	if (!chsz) { // just a regular char
		*outch = (ui)ch; 
		return 1;
	}
	const uc ch1 = src[1];
	if (not_trailing(ch1)) // a codepage char, we need the OS to decode it:
		return mbtowc((wchar_t*)outch, src, 1);
	// this decodes an encoded UTF-8 codepoint into the supported WCHAR format
	// Example UCS8 src[110x xxxx][10yy yyyy] -> outch[0000xxxx xxyyyyyy]
	switch (chsz) {
	default:
	case 0: // 0 and 1 are not actually used, but it speeds the switch up
	case 1:
	case 2: *outch = (ui)((((ui)ch<<6) + ch1) - _utf_decode_magic[1]); return 2;
	case 3: *outch = (ui)((((((ui)ch<<6) + ch1)<<6) + (uc)src[2]) - _utf_decode_magic[2]); return 3;
	case 4: *outch = UNICODE_INVALID; return 4; // we don't support 4-byte UTF sequences, since they go beyond 16-bits
	}
}

//-----------------------------------------------------------------------------