using RogueAssistantNET.Game;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant
{
	public class RogueAssistant
	{
		private const string c_RogueHandshake1 = "3to8UEaoManH7wB4lKlLRgywSHHKmI0g";
		private const string c_RogueHandshake2 = "Em68TrzBAFlyhBCOm4XQIjGWbdNhuplY";

		private GameConnection m_Connection = null;
		private Thread m_MainThread = null;
		private ReaderWriterLock m_BehaviourLock = new ReaderWriterLock();

		private List<IRogueAssistantBehaviour> m_Behaviours = new List<IRogueAssistantBehaviour>();
		private ConcurrentQueue<IRogueAssistantBehaviour> m_BehavioursToAdd = new ConcurrentQueue<IRogueAssistantBehaviour>();
		private ConcurrentQueue<IRogueAssistantBehaviour> m_BehavioursToRemove = new ConcurrentQueue<IRogueAssistantBehaviour>();

		private bool m_IsFirstUpdate = true;
		private string m_DisconnectionMessage = null;

		private RogueAssistant(TcpClient client)
		{
			m_Connection = new GameConnection(client);
			m_MainThread = Thread.CurrentThread;
		}

		public GameConnection Connection
		{
			get => m_Connection;
		}

		public bool HasInitialised
		{
			get => m_Connection != null && m_Connection.HasInitialised;
        }

        public bool HasDisconnected
        {
            get => m_DisconnectionMessage != null;
        }

        public string DisconnectionMessage
        {
            get => m_DisconnectionMessage;
        }

        public IEnumerable<IRogueAssistantBehaviour> ActiveBehaviours
		{
			get
			{
				m_BehaviourLock.AcquireReaderLock(10000);
				var behaviours = m_Behaviours.ToArray();
				m_BehaviourLock.ReleaseReaderLock();

				return behaviours;
			}
		}


		public void AddBehaviour(IRogueAssistantBehaviour behaviour)
		{
			if (m_MainThread == Thread.CurrentThread)
			{
				m_BehaviourLock.AcquireWriterLock(10000);
				m_Behaviours.Add(behaviour);
				m_BehaviourLock.ReleaseWriterLock();

				if (m_Connection != null && m_Connection.HasInitialised)
					behaviour.OnAttach(this);
			}
			else
			{
				m_BehavioursToAdd.Enqueue(behaviour);
			}
		}

		public void RemoveBehaviour(IRogueAssistantBehaviour behaviour)
		{
			m_BehavioursToRemove.Enqueue(behaviour);
		}

		public T FindBehaviour<T>()
		{
			m_BehaviourLock.AcquireReaderLock(10000);
			T output = m_Behaviours.Where((b) => b is T).Select((b) => (T)b).FirstOrDefault();
			m_BehaviourLock.ReleaseReaderLock();
			return output;
        }

        public bool HasBehaviour<T>()
		{
			m_BehaviourLock.AcquireReaderLock(10000);
			bool output = m_Behaviours.Where((b) => b is T).Any();
			m_BehaviourLock.ReleaseReaderLock();
			return output;
		}

        public T FindOrCreateBehaviour<T>() where T : IRogueAssistantBehaviour, new()
		{
			T behaviour = FindBehaviour<T>();

			if (behaviour == null)
			{
				behaviour = new T();
				AddBehaviour(behaviour);
			}

			return behaviour;
		}

		public void Update()
		{
			if (HasDisconnected)
				return;

			try
			{
				while (m_BehavioursToAdd.Count != 0)
				{
					if(m_BehavioursToAdd.TryDequeue(out IRogueAssistantBehaviour behaviour))
						AddBehaviour(behaviour);
				}

				m_Connection.Update();

				while (m_BehavioursToRemove.Count != 0)
				{
					if (m_BehavioursToRemove.TryDequeue(out IRogueAssistantBehaviour behaviour))
					{
						behaviour.OnDetach(this);

						m_BehaviourLock.AcquireWriterLock(10000);
						m_Behaviours.Remove(behaviour);
						m_BehaviourLock.ReleaseWriterLock();
					}
				}

				m_BehaviourLock.AcquireReaderLock(10000);
				var behavioursToUpdate = m_Behaviours.ToArray();
				m_BehaviourLock.ReleaseReaderLock();

				if (m_IsFirstUpdate && m_Connection.HasInitialised)
				{
					m_IsFirstUpdate = false;

					foreach (var behaviour in behavioursToUpdate)
						behaviour.OnAttach(this);
				}

				foreach (var behaviour in behavioursToUpdate)
				{
					behaviour.OnUpdate(this);
				}
			}
			catch(Exception e)
            {
                m_DisconnectionMessage = e.Message;
#if DEBUG
                m_DisconnectionMessage += "\n[" + e.GetType().Name + "]\n" + e.StackTrace;
#endif
			}
		}

		public static bool TryAccept(TcpClient client, out RogueAssistant assistant)
		{
			assistant = null;

			Console.WriteLine($"Checking incoming connection...");
			byte[] buffer = new byte[4096];


			// Check first handshake
			int readCount = client.Client.Receive(buffer);
			string handshake1 = Encoding.ASCII.GetString(buffer, 0, readCount);

			if (handshake1 != c_RogueHandshake1)
				return false;

			// Send 2nd continue signal
			int count = Encoding.ASCII.GetBytes("con", 0, 3, buffer, 0);
			client.Client.Send(buffer, 0, count, SocketFlags.None);

			readCount = client.Client.Receive(buffer);
			string handshake2 = Encoding.ASCII.GetString(buffer, 0, readCount);

			if (handshake2 != c_RogueHandshake2)
				return false;

			assistant = new RogueAssistant(client);
			return true;
		}
	}
}
