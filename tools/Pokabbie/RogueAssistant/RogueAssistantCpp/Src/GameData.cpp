#include "GameData.h"

namespace GameHelpers
{
	struct CharMapping
	{
		std::string m_ResolvedString;
		u8 m_CharacterCode;
	};

	static const CharMapping c_CharMapping[]
	{
		{ " ", 0x00 },
		{ "À", 0x01 },
		{ "Á", 0x02 },
		{ "Â", 0x03 },
		{ "Ç", 0x04 },
		{ "È", 0x05 },
		{ "É", 0x06 },
		{ "Ê", 0x07 },
		{ "Ë", 0x08 },
		{ "Ì", 0x09 },
		{ "Î", 0x0B },
		{ "Ï", 0x0C },
		{ "Ò", 0x0D },
		{ "Ó", 0x0E },
		{ "Ô", 0x0F },
		{ "Œ", 0x10 },
		{ "Ù", 0x11 },
		{ "Ú", 0x12 },
		{ "Û", 0x13 },
		{ "Ñ", 0x14 },
		{ "ß", 0x15 },
		{ "à", 0x16 },
		{ "á", 0x17 },
		{ "ç", 0x19 },
		{ "è", 0x1A },
		{ "é", 0x1B },
		{ "ê", 0x1C },
		{ "ë", 0x1D },
		{ "ì", 0x1E },
		{ "î", 0x20 },
		{ "ï", 0x21 },
		{ "ò", 0x22 },
		{ "ó", 0x23 },
		{ "ô", 0x24 },
		{ "œ", 0x25 },
		{ "ù", 0x26 },
		{ "ú", 0x27 },
		{ "û", 0x28 },
		{ "ñ", 0x29 },
		{ "º", 0x2A },
		{ "ª", 0x2B },
		{ "SUPER_ER", 0x2C },
		{ "&", 0x2D },
		{ "+", 0x2E },
		{ "LV" , 0x34 },
		{ "=", 0x35 },
		{ ";", 0x36 },
		{ "¿", 0x51 },
		{ "¡", 0x52 },
		{ "PK" , 0x53 },
		{ "MN" , 0x54 },
		{ "PO" , 0x55 },
		{ "KE" , 0x56 },
		{ "BL" , 0x57 },
		{ "O" , 0x58 },
		{ "CK" , 0x59 },
		{ "Í", 0x5A },
		{ "%", 0x5B },
		{ "(", 0x5C },
		{ " }", 0x5D },
		{ "â", 0x68 },
		{ "í", 0x6F },
		{ "UNK_SPACER", 0x77 },
		{ "UP_ARROW", 0x79 },
		{ "DOWN_ARROW", 0x7A },
		{ "LEFT_ARROW", 0x7B },
		{ "RIGHT_ARROW", 0x7C },
		{ "SUPER_E", 0x84 },
		{ "<", 0x85 },
		{ ">", 0x86 },
		{ "SUPER_RE", 0xA0 },
		{ "0", 0xA1 },
		{ "1", 0xA2 },
		{ "2", 0xA3 },
		{ "3", 0xA4 },
		{ "4", 0xA5 },
		{ "5", 0xA6 },
		{ "6", 0xA7 },
		{ "7", 0xA8 },
		{ "8", 0xA9 },
		{ "9", 0xAA },
		{ "!", 0xAB },
		{ "?", 0xAC },
		{ ".", 0xAD },
		{ "-", 0xAE },
		{ "·", 0xAF },
		{ "…", 0xB0 },
		{ "“", 0xB1 },
		{ "”", 0xB2 },
		{ "‘", 0xB3 },
		{ "’", 0xB4 },
		{ "\"", 0xB4 },
		{ "M", 0xB5 }, //{ "♂", 0xB5 },
		{ "F", 0xB6 }, //{ "♀", 0xB6 },
		{ "¥", 0xB7 },
		{ ",", 0xB8 },
		{ "×", 0xB9 },
		{ "/", 0xBA },
		{ "A", 0xBB },
		{ "B", 0xBC },
		{ "C", 0xBD },
		{ "D", 0xBE },
		{ "E", 0xBF },
		{ "F", 0xC0 },
		{ "G", 0xC1 },
		{ "H", 0xC2 },
		{ "I", 0xC3 },
		{ "J", 0xC4 },
		{ "K", 0xC5 },
		{ "L", 0xC6 },
		{ "M", 0xC7 },
		{ "N", 0xC8 },
		{ "O", 0xC9 },
		{ "P", 0xCA },
		{ "Q", 0xCB },
		{ "R", 0xCC },
		{ "S", 0xCD },
		{ "T", 0xCE },
		{ "U", 0xCF },
		{ "V", 0xD0 },
		{ "W", 0xD1 },
		{ "X", 0xD2 },
		{ "Y", 0xD3 },
		{ "Z", 0xD4 },
		{ "a", 0xD5 },
		{ "b", 0xD6 },
		{ "c", 0xD7 },
		{ "d", 0xD8 },
		{ "e", 0xD9 },
		{ "f", 0xDA },
		{ "g", 0xDB },
		{ "h", 0xDC },
		{ "i", 0xDD },
		{ "j", 0xDE },
		{ "k", 0xDF },
		{ "l", 0xE0 },
		{ "m", 0xE1 },
		{ "n", 0xE2 },
		{ "o", 0xE3 },
		{ "p", 0xE4 },
		{ "q", 0xE5 },
		{ "r", 0xE6 },
		{ "s", 0xE7 },
		{ "t", 0xE8 },
		{ "u", 0xE9 },
		{ "v", 0xEA },
		{ "w", 0xEB },
		{ "x", 0xEC },
		{ "y", 0xED },
		{ "z", 0xEE },
		{ "?", 0xEF }, // { "▶", 0xEF },
		{ ":", 0xF0 },
		{ "Ä", 0xF1 },
		{ "Ö", 0xF2 },
		{ "Ü", 0xF3 },
		{ "ä", 0xF4 },
		{ "ö", 0xF5 },
		{ "ü", 0xF6 },
		{ "$", 0xFF },
	};

	std::string ParseGameString(u8 const* str, size_t length)
	{
		std::string output;

		for (size_t i = 0; i < length; ++i)
		{
			u8 c = str[i];

			// This is NULL in game strings
			if (c == 0xFF)
				break;

			for (size_t j = 0; j < ARRAY_COUNT(c_CharMapping); ++j)
			{
				if (c_CharMapping[j].m_CharacterCode == c)
				{
					output += c_CharMapping[j].m_ResolvedString;
					break;
				}
			}
		}

		return output;
	}
}