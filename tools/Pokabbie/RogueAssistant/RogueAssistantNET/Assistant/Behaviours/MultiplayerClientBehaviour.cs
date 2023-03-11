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
	public class MultiplayerClientBehaviour : IRogueAssistantBehaviour
	{
		private NetPlayerSyncBehaviour m_PlayerSync;
		private string m_Hostname;
		private int m_Port;

		private TcpClient m_Client;
		private List<NetPlayerData> m_NetworkPlayers = new List<NetPlayerData>();

		public MultiplayerClientBehaviour(string address, int port)
		{
			m_Client = new TcpClient();
			m_Hostname = address;
			m_Port = port;
		}


		public void OnAttach(RogueAssistant assistant)
		{
			m_PlayerSync = assistant.FindOrCreateBehaviour<NetPlayerSyncBehaviour>();
			m_PlayerSync.RefreshLocalPlayer(assistant.Connection);

			Console.WriteLine($"== Openning Client ({m_Hostname}:{m_Port}) ==");
			m_Client.Connect(m_Hostname, m_Port);

			var asyncArgs = new SocketAsyncEventArgs();
			asyncArgs.SetBuffer(new byte[2048], 0, 2048);
			asyncArgs.Completed += OnTcpClientRecieve;
			m_Client.Client.ReceiveAsync(asyncArgs);

			NetworkPacket profilePacket = new NetworkPacket(NetworkChannel.NetPlayerProfile, m_PlayerSync.LocalPlayer.ProfileData);
			NetworkPacket statePacket = new NetworkPacket(NetworkChannel.NetPlayerState, m_PlayerSync.LocalPlayer.StateData);

			m_Client.Client.Send(profilePacket.ToBinaryBlob());
			m_Client.Client.Send(statePacket.ToBinaryBlob());
		}

		public void OnDetach(RogueAssistant assistant)
		{
		}

		public void OnUpdate(RogueAssistant assistant)
		{
			NetworkPacket statePacket = new NetworkPacket(NetworkChannel.NetPlayerState, m_PlayerSync.LocalPlayer.StateData);
			m_Client.Client.Send(statePacket.ToBinaryBlob());
		}

		private void OnTcpClientRecieve(object sender, SocketAsyncEventArgs asyncArgs)
		{
			Socket socket = (Socket)sender;

			int profileId = 0;
			int stateId = 0;
			foreach (var packet in NetworkPacket.ParsePackets(asyncArgs.Buffer, asyncArgs.Offset, asyncArgs.BytesTransferred))
			{
				switch (packet.Channel)
				{
					case NetworkChannel.NetPlayerProfile:
						{
							NetPlayerData playerData = GetPlayerData(profileId);
							playerData.ProfileData = packet.Content;
							++profileId;
							break;
						}

					case NetworkChannel.NetPlayerState:
						{
							NetPlayerData playerData = GetPlayerData(stateId);
							playerData.StateData = packet.Content;
							++stateId;
							break;
						}
				}
			}

			socket.ReceiveAsync(asyncArgs);
		}

		public NetPlayerData GetPlayerData(int id)
		{
			while(id >= m_NetworkPlayers.Count)
			{
				NetPlayerData player = new NetPlayerData();
				m_NetworkPlayers.Add(player);
				m_PlayerSync.AddOnlinePlayer(player);
			}

			return m_NetworkPlayers[id];
		}
	}
}
