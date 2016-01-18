#pragma once
/**
 * UTF8 <> UCS-2 (WCHAR) conversion methods
 * @author	Jorma Rebane
 * @date	30.11.2014
 */

/**
 * Encodes a WCHAR into an UTF8 sequence
 * @param wch Wide Char (UCS-2) to encode
 * @param dst Destination buffer at least [3] chars long
 * @return Number of bytes in the UTF8 sequence [1-3]
 */
int utf_encode(unsigned int wch, char* dst);


/**
 * Decodes an UTF8 sequence into a WCHAR
 * @param src Source UTF8 sequence
 * @param outch Destination WCHAR character
 * @return Number of bytes decoded from src: [1-4]
 */
int utf_decode(const char* src, unsigned* outch);