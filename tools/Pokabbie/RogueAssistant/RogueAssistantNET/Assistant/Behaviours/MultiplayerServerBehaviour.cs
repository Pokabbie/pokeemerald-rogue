using ENet;
using RogueAssistantNET.Assistant.Utilities;
using RogueAssistantNET.Game;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Data.Common;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant.Behaviours
{
	public class MultiplayerServerBehaviour : IRogueAssistantBehaviour
    {
        public const int c_DefaultPort = 20010;

        public const string c_Handshake1 = "8Tr3tolLRgywSHHKmI0gGWbdNhuplYhB";
		public const string c_Handshake2 = "lyCOm4XQIj8UEaoManH7wB4lKEm6zBAF"; 

		private NetPlayerSyncBehaviour m_PlayerSync;
		private TcpListenerServer m_Server;
		private int m_Port;

		private ConcurrentQueue<TcpClient> m_NewConnections = new ConcurrentQueue<TcpClient>();
		private List<TcpClient> m_ActiveConnections = new List<TcpClient>();
		private Dictionary<Socket, NetPlayerData> m_ConnectionPlayerData = new Dictionary<Socket, NetPlayerData>();
        private byte[] m_TempBuffer = new byte[2048];

        public MultiplayerServerBehaviour(int port)
		{
			m_Server = new TcpListenerServer(IPAddress.Any, port);
            m_Port = port;

        }

		public bool ReachedMaxPlayerCount
		{
			get => m_ActiveConnections.Count >= m_PlayerSync.MaxPlayerCount - 1;
		}

		public int Port
		{
			get => m_Port;
		}

		public void OnAttach(RogueAssistant assistant)
		{
			Console.WriteLine($"== Openning Server ==");
			m_PlayerSync = assistant.FindOrCreateBehaviour<NetPlayerSyncBehaviour>();

			if(m_Server.Open(1))
			{
				assistant.Connection.SendReliable(GameCommandCode.BeginMultiplayerHost);
				m_PlayerSync.RefreshLocalPlayer(assistant.Connection);
			}
			else
			{
				assistant.RemoveBehaviour(this);
			}
		}

		public void OnDetach(RogueAssistant assistant)
		{
			assistant.Connection.SendReliable(GameCommandCode.EndMultiplayer);
		}

		public void OnUpdate(RogueAssistant assistant)
		{
			if(m_Server.TryAccept(out TcpClient newClient))
			{
				Task.Run(() =>
				{
					if (TryAccept(assistant, newClient))
						m_NewConnections.Enqueue(newClient);
					else
						newClient.Close();
				});
			}

			if(m_NewConnections.Count != 0 && m_NewConnections.TryDequeue(out newClient))
			{
                //newClient.Client.Send(new NetworkPacket(NetworkChannel.NetPlayerProfile, m_PlayerSync.LocalPlayer.ProfileData).ToBinaryBlob());
				m_ActiveConnections.Add(newClient);
			}

			foreach(var client in m_ActiveConnections)
            {
				// Read
				//
                var asyncArgs = new SocketAsyncEventArgs();
                asyncArgs.SetBuffer(m_TempBuffer, 0, m_TempBuffer.Length);

				if (!client.Client.ReceiveAsync(asyncArgs))
					OnTcpClientRecieve(client, asyncArgs);

				// Send
				//
                NetworkPacketBatch batch = new NetworkPacketBatch();

                PushPlayerData(batch, m_PlayerSync.LocalPlayer);

				foreach(var kvp in m_ConnectionPlayerData)
				{
					if(kvp.Key != client.Client)
					{
                        PushPlayerData(batch, kvp.Value);
					}
				}

				batch.Send(client.Client);
            }
		}

		private void PushPlayerData(NetworkPacketBatch batch, NetPlayerData player)
        {
            batch.Push(new NetworkPacket(NetworkChannel.NetPlayerProfile, player.ProfileData));
            batch.Push(new NetworkPacket(NetworkChannel.NetPlayerState, player.StateData));
		}

		private void OnTcpClientRecieve(TcpClient client, SocketAsyncEventArgs asyncArgs)
        {
            var batch = NetworkPacketBatch.From(asyncArgs.Buffer, asyncArgs.Offset, asyncArgs.BytesTransferred);

            foreach (var packet in batch.Packets)
            {
				switch(packet.Channel)
				{
					case NetworkChannel.NetPlayerProfile:
						{
							NetPlayerData playerData = GetPlayerData(client.Client);
							playerData.ProfileData = packet.Content;
							break;
						}

					case NetworkChannel.NetPlayerState:
						{
							NetPlayerData playerData = GetPlayerData(client.Client);
							playerData.StateData = packet.Content;
							break;
						}
				}
			}
		}

		public bool TryAccept(RogueAssistant assistant, TcpClient client)
		{
			Console.WriteLine($"Player connecting...");
			client.NoDelay = true;

			client.ReceiveTimeout = 15 * 1000;
			client.SendTimeout = 15 * 1000;

			if (ReachedMaxPlayerCount)
			{
				Console.WriteLine($"Rejecting (Reached Max Players)");
				return false;
			}

			byte[] buffer = new byte[4096];

			bool VerifyString(string match)
			{
				int readCount = client.Client.Receive(buffer);
				string incoming = Encoding.ASCII.GetString(buffer, 0, readCount);

				if (incoming != match)
					return false;

				// Send 2nd continue signal
				int count = Encoding.ASCII.GetBytes("OK", 0, 2, buffer, 0);
				client.Client.Send(buffer, 0, count, SocketFlags.None);
				return true;
			}

			// Check handshakes
			if(!VerifyString(c_Handshake1) || !VerifyString(c_Handshake2))
			{
				Console.WriteLine($"Rejecting (Failed to verify)");
				return false;
			}

			// Check game version
			if (!VerifyString(assistant.Connection.Header.GameEdition.ToString()))
			{
				Console.WriteLine($"Rejecting (Incompatible GameEdition)");
				return false;
			}

			Console.WriteLine($"Player connected!");
			//.Client.Blocking = false;
            return true;
		}

		public NetPlayerData GetPlayerData(Socket socket)
		{
			NetPlayerData player;
			if (m_ConnectionPlayerData.TryGetValue(socket, out player))
				return player;

			player = new NetPlayerData();
			m_ConnectionPlayerData.Add(socket, player);
			m_PlayerSync.AddOnlinePlayer(player);

			return player;
		}
	}
}
