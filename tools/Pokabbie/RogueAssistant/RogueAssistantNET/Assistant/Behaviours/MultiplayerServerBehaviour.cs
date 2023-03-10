using RogueAssistantNET.Assistant.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant.Behaviours
{
	public class MultiplayerServerBehaviour : IRogueAssistantBehaviour
	{
		private NetPlayerSyncBehaviour m_PlayerSync;
		private TcpListenerServer m_Server;

		private List<TcpClient> m_ActiveConnections = new List<TcpClient>();
		private Dictionary<Socket, NetPlayerData> m_ConnectionPlayerData = new Dictionary<Socket, NetPlayerData>();

		public MultiplayerServerBehaviour(int port)
		{
			m_Server = new TcpListenerServer(IPAddress.Any, port);
		}

		public bool ReachedMaxPlayerCount
		{
			get => m_ActiveConnections.Count >= m_PlayerSync.MaxPlayerCount - 1;
		}

		public void OnAttach(RogueAssistant assistant)
		{
			Console.WriteLine($"== Openning Server ==");
			m_PlayerSync = assistant.FindOrCreateBehaviour<NetPlayerSyncBehaviour>();
			m_PlayerSync.RefreshLocalPlayer(assistant.Connection);

			m_Server.Open(1);
		}

		public void OnDetach(RogueAssistant assistant)
		{
		}

		public void OnUpdate(RogueAssistant assistant)
		{
			if(m_Server.TryAccept(out TcpClient newClient))
			{
				if (TryAccept(newClient))
				{
					m_ActiveConnections.Add(newClient);

					var asyncArgs = new SocketAsyncEventArgs();
					asyncArgs.SetBuffer(new byte[2048], 0, 2048);
					asyncArgs.Completed += OnTcpClientRecieve;

					newClient.Client.ReceiveAsync(asyncArgs);
				}
				else
					newClient.Close();
			}

			foreach(var client in m_ActiveConnections)
			{
				SendPlayerData(client.Client, m_PlayerSync.LocalPlayer);

				foreach(var kvp in m_ConnectionPlayerData)
				{
					if(kvp.Key != client.Client)
					{
						SendPlayerData(client.Client, kvp.Value);
					}
				}
			}
		}

		private void SendPlayerData(Socket socket, NetPlayerData player)
		{
			socket.Send(new NetworkPacket(NetworkChannel.NetPlayerProfile, player.ProfileData).ToBinaryBlob());
			socket.Send(new NetworkPacket(NetworkChannel.NetPlayerState, player.StateData).ToBinaryBlob());
		}

		private void OnTcpClientRecieve(object sender, SocketAsyncEventArgs asyncArgs)
		{
			Socket socket = (Socket)sender;

			foreach(var packet in NetworkPacket.ParsePackets(asyncArgs.Buffer, asyncArgs.Offset, asyncArgs.BytesTransferred))
			{
				switch(packet.Channel)
				{
					case NetworkChannel.NetPlayerProfile:
						{
							NetPlayerData playerData = GetPlayerData(socket);
							playerData.ProfileData = packet.Content;
							break;
						}

					case NetworkChannel.NetPlayerState:
						{
							NetPlayerData playerData = GetPlayerData(socket);
							playerData.StateData = packet.Content;
							break;
						}
				}
			}

			socket.ReceiveAsync(asyncArgs);
		}

		public bool TryAccept(TcpClient client)
		{
			if (ReachedMaxPlayerCount)
				return false;

			Console.WriteLine($"Player connecting...");
			// TODO - Verify this is a valid connection
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
