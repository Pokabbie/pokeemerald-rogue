using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
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
	}

	public class NetworkPacketBatch
	{
		private List<NetworkPacket> m_Packets = new List<NetworkPacket>();

		public IEnumerable<NetworkPacket> Packets
		{
			get => m_Packets;
		}

		public void Push(NetworkPacket packet)
		{
			m_Packets.Add(packet);
        }

		public void Send(Socket socket)
        {
			if (m_Packets.Count != 0)
			{
				List<byte> data = new List<byte>();

				foreach (var blob in m_Packets.Select((p) => p.ToBinaryBlob()))
					data.AddRange(blob);

				socket.Send(data.ToArray());
			}
        }
        public static NetworkPacketBatch From(byte[] data, int offset, int length)
        {
			NetworkPacketBatch batch = new NetworkPacketBatch();

            using (MemoryStream stream = new MemoryStream(data, offset, length, false))
            {
                using (BinaryReader reader = new BinaryReader(stream))
                {
					while (stream.Position < stream.Length)
					{

						NetworkChannel channel = (NetworkChannel)reader.ReadByte();

						int contentLength = reader.ReadInt32();
						byte[] content = reader.ReadBytes(contentLength);

						batch.Push(new NetworkPacket(channel, content));
					}
				}
			}

			return batch;
        }
    }
}
