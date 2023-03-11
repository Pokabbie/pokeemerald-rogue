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
		private enum ConnectionState
		{
			Connecting,
			PostConnect,
			Active,
			Disconnected
		}

		private NetPlayerSyncBehaviour m_PlayerSync;
		private string m_Hostname;
		private int m_Port;

		private TcpClient m_Client;
		private ConnectionState m_State;
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

			Task.Run(() =>
			{
				if (TryConnect(assistant))
				{
					m_State = ConnectionState.Active;
				}
				else
				{
					m_Client.Close();
					m_Client = null;
					m_State = ConnectionState.Disconnected;
				}
			});

		}

		public void OnDetach(RogueAssistant assistant)
		{
		}

		public void OnUpdate(RogueAssistant assistant)
		{
			switch(m_State)
			{
				case ConnectionState.Active:
					{
						NetworkPacket statePacket = new NetworkPacket(NetworkChannel.NetPlayerState, m_PlayerSync.LocalPlayer.StateData);
						m_Client.Client.Send(statePacket.ToBinaryBlob());
					}
					break;

				case ConnectionState.PostConnect:
					{
						m_State = ConnectionState.Active;

						var asyncArgs = new SocketAsyncEventArgs();
						asyncArgs.SetBuffer(new byte[2048], 0, 2048);
						asyncArgs.Completed += OnTcpClientRecieve;
						m_Client.Client.ReceiveAsync(asyncArgs);

						NetworkPacket profilePacket = new NetworkPacket(NetworkChannel.NetPlayerProfile, m_PlayerSync.LocalPlayer.ProfileData);
						NetworkPacket statePacket = new NetworkPacket(NetworkChannel.NetPlayerState, m_PlayerSync.LocalPlayer.StateData);

						m_Client.Client.Send(profilePacket.ToBinaryBlob());
						m_Client.Client.Send(statePacket.ToBinaryBlob());
					}
					break;

				case ConnectionState.Disconnected:
					assistant.RemoveBehaviour(this);
					break;
			}
		}

		private bool TryConnect(RogueAssistant assistant)
		{
			Console.WriteLine($"Connecting...");
			byte[] buffer = new byte[4096];

			bool VerifyString(string input)
			{
				int count = Encoding.ASCII.GetBytes(input, 0, input.Length, buffer, 0);
				m_Client.Client.Send(buffer, 0, count, SocketFlags.None);

				int readCount = m_Client.Client.Receive(buffer);
				string response = Encoding.ASCII.GetString(buffer, 0, readCount);

				return response == "OK";
			}

			// Check handshakes
			if (!VerifyString(MultiplayerServerBehaviour.c_Handshake1) || !VerifyString(MultiplayerServerBehaviour.c_Handshake2))
			{
				Console.WriteLine($"Rejected (Failed to verify)");
				return false;
			}

			// Check game version
			if (!VerifyString(assistant.Connection.Header.GameEdition.ToString()))
			{
				Console.WriteLine($"Rejected (Incompatible GameEdition)");
				return false;
			}

			// accepted
			return true;
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
