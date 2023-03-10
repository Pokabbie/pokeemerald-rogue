using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant.Utilities
{
	public enum NetworkChannel
	{
		Default,
		NetPlayerProfile,
		NetPlayerState
	}

	public class NetworkPacket
	{
		private NetworkChannel m_Channel;
		private byte[] m_Content;

		public NetworkPacket(NetworkChannel channel, byte[] content)
		{
			m_Channel = channel;
			m_Content = content;
		}

		public NetworkChannel Channel
		{
			get => m_Channel;
		}

		public byte[] Content
		{
			get => m_Content;
		}

		public byte[] ToBinaryBlob()
		{
			using (MemoryStream stream = new MemoryStream())
			{
				using (BinaryWriter writer = new BinaryWriter(stream))
				{
					writer.Write((byte)m_Channel);
					writer.Write(m_Content.Length);
					writer.Write(m_Content);
				}

				return stream.ToArray();
			}
		}

		public static IEnumerable<NetworkPacket> ParsePackets(byte[] data, int offset, int length)
		{
			using (MemoryStream stream = new MemoryStream(data, offset, length, false))
			{
				using (BinaryReader reader = new BinaryReader(stream))
				{
					NetworkChannel channel = (NetworkChannel)reader.ReadByte();

					int contentLength = reader.ReadInt32();
					byte[] content = reader.ReadBytes(contentLength);

					yield return new NetworkPacket(channel, content);
				}
			}
		}
	}
}
