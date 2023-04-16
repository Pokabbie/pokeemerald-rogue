using RogueAssistantNET.Assistant.Utilities;
using RogueAssistantNET.Game;
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
		private byte[] m_TempBuffer = new byte[2048];

        public MultiplayerClientBehaviour(string address, int port)
		{
			m_Client = new TcpClient();
			m_Client.NoDelay = true;

			m_Hostname = address;
			m_Port = port;
		}

		public bool IsConnecting
		{
			get => m_State == ConnectionState.Connecting;
		}

		public bool IsDisconnected
		{
			get => m_State == ConnectionState.Disconnected;
		}

		public void OnAttach(RogueAssistant assistant)
		{
			m_PlayerSync = assistant.FindOrCreateBehaviour<NetPlayerSyncBehaviour>();
			m_PlayerSync.RemoveAllOnlinePlayers();

			Console.WriteLine($"== Openning Client ({m_Hostname}:{m_Port}) ==");

			Task.Run(() =>
			{
				if (TryConnect(assistant))
                {
                    //m_Client.Client.Blocking = false;
                    m_State = ConnectionState.PostConnect;
					assistant.Connection.SendReliable(GameCommandCode.BeginMultiplayerClient);
					m_PlayerSync.RefreshLocalPlayer(assistant.Connection);
				}
				else
				{
					m_Client.Close();
					m_Client = null;
					m_State = ConnectionState.Disconnected;
					assistant.RemoveBehaviour(this);
				}
			});

		}

		public void OnDetach(RogueAssistant assistant)
		{
			m_PlayerSync.RemoveAllOnlinePlayers();

			assistant.Connection.SendReliable(GameCommandCode.EndMultiplayer);
		}

		public void OnUpdate(RogueAssistant assistant)
		{
			NetworkPacketBatch batch = new NetworkPacketBatch();

            switch (m_State)
			{
				case ConnectionState.Active:
					{
                        var asyncArgs = new SocketAsyncEventArgs();
                        asyncArgs.SetBuffer(m_TempBuffer, 0, m_TempBuffer.Length);

						// Read
						//
                        if (!m_Client.Client.ReceiveAsync(asyncArgs))
                            OnTcpClientRecieve(asyncArgs);

						batch.Push(new NetworkPacket(NetworkChannel.NetPlayerProfile, m_PlayerSync.LocalPlayer.ProfileData));
						batch.Push(new NetworkPacket(NetworkChannel.NetPlayerState, m_PlayerSync.LocalPlayer.StateData));
					}
					batch.Send(m_Client.Client);
					break;

				case ConnectionState.PostConnect:
					{
						m_State = ConnectionState.Active;

                        batch.Push(new NetworkPacket(NetworkChannel.NetPlayerProfile, m_PlayerSync.LocalPlayer.ProfileData));
                        batch.Push(new NetworkPacket(NetworkChannel.NetPlayerState, m_PlayerSync.LocalPlayer.StateData));
					}
					batch.Send(m_Client.Client);
					break;

				case ConnectionState.Disconnected:
					assistant.RemoveBehaviour(this);
					break;
			}

        }

		private bool TryConnect(RogueAssistant assistant)
		{
			Console.WriteLine($"Connecting...");

			try
			{
				m_Client.Connect(m_Hostname, m_Port);
			}
			catch (Exception ex) 
			{
				Console.Error.WriteLine($"Failed to connect: {ex.Message}");
				return false;
			}


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
			Console.WriteLine($"Connected!");
			return true;
		}

		private void OnTcpClientRecieve(SocketAsyncEventArgs asyncArgs)
		{
			int profileId = 0;
			int stateId = 0;
			var batch = NetworkPacketBatch.From(asyncArgs.Buffer, asyncArgs.Offset, asyncArgs.BytesTransferred);

            foreach (var packet in batch.Packets)
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
