using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game
{
	public static class GameString
	{
		public struct CharMapping
		{
			public string Chars;
			public byte Code;

			public CharMapping(string ch, byte code)
			{
				Chars = ch;
				Code = code;
			}
		}

		private static CharMapping[] s_Mapping = new CharMapping[]
		{
			new CharMapping(" ", 0x00),
			new CharMapping("À", 0x01),
			new CharMapping("Á", 0x02),
			new CharMapping("Â", 0x03),
			new CharMapping("Ç", 0x04),
			new CharMapping("È", 0x05),
			new CharMapping("É", 0x06),
			new CharMapping("Ê", 0x07),
			new CharMapping("Ë", 0x08),
			new CharMapping("Ì", 0x09),
			new CharMapping("Î", 0x0B),
			new CharMapping("Ï", 0x0C),
			new CharMapping("Ò", 0x0D),
			new CharMapping("Ó", 0x0E),
			new CharMapping("Ô", 0x0F),
			new CharMapping("Œ", 0x10),
			new CharMapping("Ù", 0x11),
			new CharMapping("Ú", 0x12),
			new CharMapping("Û", 0x13),
			new CharMapping("Ñ", 0x14),
			new CharMapping("ß", 0x15),
			new CharMapping("à", 0x16),
			new CharMapping("á", 0x17),
			new CharMapping("ç", 0x19),
			new CharMapping("è", 0x1A),
			new CharMapping("é", 0x1B),
			new CharMapping("ê", 0x1C),
			new CharMapping("ë", 0x1D),
			new CharMapping("ì", 0x1E),
			new CharMapping("î", 0x20),
			new CharMapping("ï", 0x21),
			new CharMapping("ò", 0x22),
			new CharMapping("ó", 0x23),
			new CharMapping("ô", 0x24),
			new CharMapping("œ", 0x25),
			new CharMapping("ù", 0x26),
			new CharMapping("ú", 0x27),
			new CharMapping("û", 0x28),
			new CharMapping("ñ", 0x29),
			new CharMapping("º", 0x2A),
			new CharMapping("ª", 0x2B),
			new CharMapping("SUPER_ER", 0x2C),
			new CharMapping("&", 0x2D),
			new CharMapping("+", 0x2E),
			new CharMapping("LV" , 0x34),
			new CharMapping("=", 0x35),
			new CharMapping(";", 0x36),
			new CharMapping("¿", 0x51),
			new CharMapping("¡", 0x52),
			new CharMapping("PK" , 0x53),
			new CharMapping("MN" , 0x54),
			new CharMapping("PO" , 0x55),
			new CharMapping("KE" , 0x56),
			new CharMapping("BL" , 0x57),
			new CharMapping("O" , 0x58),
			new CharMapping("CK" , 0x59),
			new CharMapping("Í", 0x5A),
			new CharMapping("%", 0x5B),
			new CharMapping("(", 0x5C),
			new CharMapping(")", 0x5D),
			new CharMapping("â", 0x68),
			new CharMapping("í", 0x6F),
			new CharMapping("UNK_SPACER", 0x77),
			new CharMapping("UP_ARROW", 0x79),
			new CharMapping("DOWN_ARROW", 0x7A),
			new CharMapping("LEFT_ARROW", 0x7B),
			new CharMapping("RIGHT_ARROW", 0x7C),
			new CharMapping("SUPER_E", 0x84),
			new CharMapping("<", 0x85),
			new CharMapping(">", 0x86),
			new CharMapping("SUPER_RE", 0xA0),
			new CharMapping("0", 0xA1),
			new CharMapping("1", 0xA2),
			new CharMapping("2", 0xA3),
			new CharMapping("3", 0xA4),
			new CharMapping("4", 0xA5),
			new CharMapping("5", 0xA6),
			new CharMapping("6", 0xA7),
			new CharMapping("7", 0xA8),
			new CharMapping("8", 0xA9),
			new CharMapping("9", 0xAA),
			new CharMapping("!", 0xAB),
			new CharMapping("?", 0xAC),
			new CharMapping(".", 0xAD),
			new CharMapping("-", 0xAE),
			new CharMapping("·", 0xAF),
			new CharMapping("…", 0xB0),
			new CharMapping("“", 0xB1),
			new CharMapping("”", 0xB2),
			new CharMapping("‘", 0xB3),
			new CharMapping("’", 0xB4),
			new CharMapping("\"", 0xB4),
			new CharMapping("♂", 0xB5),
			new CharMapping("♀", 0xB6),
			new CharMapping("¥", 0xB7),
			new CharMapping(",", 0xB8),
			new CharMapping("×", 0xB9),
			new CharMapping("/", 0xBA),
			new CharMapping("A", 0xBB),
			new CharMapping("B", 0xBC),
			new CharMapping("C", 0xBD),
			new CharMapping("D", 0xBE),
			new CharMapping("E", 0xBF),
			new CharMapping("F", 0xC0),
			new CharMapping("G", 0xC1),
			new CharMapping("H", 0xC2),
			new CharMapping("I", 0xC3),
			new CharMapping("J", 0xC4),
			new CharMapping("K", 0xC5),
			new CharMapping("L", 0xC6),
			new CharMapping("M", 0xC7),
			new CharMapping("N", 0xC8),
			new CharMapping("O", 0xC9),
			new CharMapping("P", 0xCA),
			new CharMapping("Q", 0xCB),
			new CharMapping("R", 0xCC),
			new CharMapping("S", 0xCD),
			new CharMapping("T", 0xCE),
			new CharMapping("U", 0xCF),
			new CharMapping("V", 0xD0),
			new CharMapping("W", 0xD1),
			new CharMapping("X", 0xD2),
			new CharMapping("Y", 0xD3),
			new CharMapping("Z", 0xD4),
			new CharMapping("a", 0xD5),
			new CharMapping("b", 0xD6),
			new CharMapping("c", 0xD7),
			new CharMapping("d", 0xD8),
			new CharMapping("e", 0xD9),
			new CharMapping("f", 0xDA),
			new CharMapping("g", 0xDB),
			new CharMapping("h", 0xDC),
			new CharMapping("i", 0xDD),
			new CharMapping("j", 0xDE),
			new CharMapping("k", 0xDF),
			new CharMapping("l", 0xE0),
			new CharMapping("m", 0xE1),
			new CharMapping("n", 0xE2),
			new CharMapping("o", 0xE3),
			new CharMapping("p", 0xE4),
			new CharMapping("q", 0xE5),
			new CharMapping("r", 0xE6),
			new CharMapping("s", 0xE7),
			new CharMapping("t", 0xE8),
			new CharMapping("u", 0xE9),
			new CharMapping("v", 0xEA),
			new CharMapping("w", 0xEB),
			new CharMapping("x", 0xEC),
			new CharMapping("y", 0xED),
			new CharMapping("z", 0xEE),
			new CharMapping("▶", 0xEF),
			new CharMapping(":", 0xF0),
			new CharMapping("Ä", 0xF1),
			new CharMapping("Ö", 0xF2),
			new CharMapping("Ü", 0xF3),
			new CharMapping("ä", 0xF4),
			new CharMapping("ö", 0xF5),
			new CharMapping("ü", 0xF6),
			//new CharMapping(TALL_PLUS, FC 0C FB
			new CharMapping("$", 0xFF),
		};
	
		public static string GetStringFromCode(byte code)
		{
			foreach(var map in s_Mapping)
			{
				if (map.Code == code)
					return map.Chars;
			}

			return "!?!";
        }

        public static string ConvertBytes(byte[] src)
		{
			return ConvertBytes(src, 0, 0);
        }

        public static string ConvertBytes(byte[] src, uint offset, uint length)
		{
			StringBuilder builder = new StringBuilder();
			int i = 0;

			foreach (var code in src.Skip((int)offset))
			{
				// Escape char
				if (code == 0xFF)
					break;

				builder.Append(GetStringFromCode(code));

				if (++i >= length && length > 0)
					break;
			}

			return builder.ToString();
		}
	}
}
