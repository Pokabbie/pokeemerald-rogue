using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game.Commands
{	
	public class CommandRequest
	{
		public delegate void Callback(CommandRequest req);
		public delegate void CallbackTyped<T>(T behaviour) where T : IConnectionCommandBehaviour;
		public delegate void CallbackTypedFull<T>(CommandRequest req, T behaviour) where T : IConnectionCommandBehaviour;

		private IConnectionCommandBehaviour m_Behaviour;
		private List<Callback> m_Listeners = new List<Callback>();

		public CommandRequest(IConnectionCommandBehaviour behaviour)
		{
			m_Behaviour = behaviour;
		}

		public void Then(Callback callback)
		{
			m_Listeners.Add(callback);
		}

		public void Then<T>(CallbackTyped<T> callback) where T : IConnectionCommandBehaviour
		{
			m_Listeners.Add((CommandRequest _) => callback(GetBehaviour<T>()));
		}

		public void Then<T>(CallbackTypedFull<T> callback) where T : IConnectionCommandBehaviour
		{
			m_Listeners.Add((CommandRequest req) => callback(req, GetBehaviour<T>()));
		}

		public T GetBehaviour<T>() where T : IConnectionCommandBehaviour
		{
			return (T)m_Behaviour;
		}

		public void Execute(GameConnection conn)
		{
			m_Behaviour.Execute(conn);

			foreach(var listener in m_Listeners)
				listener(this);
		}
	}
}
