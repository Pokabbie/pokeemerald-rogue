using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace AutoCoordinator.Util
{
	public static class BigEndianBinaryReader
	{
        public static uint ReadUInt32BE(this BinaryReader reader)
        {
            return BitConverter.ToUInt32(reader.ReadBytes(sizeof(uint)).Reverse().ToArray(), 0);
        }
    }
}
