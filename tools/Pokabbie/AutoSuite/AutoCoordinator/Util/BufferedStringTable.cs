using System;
using System.Collections.Generic;
using System.Text;

namespace AutoCoordinator.Util
{
	public class BufferedStringTable<T>
	{
		public delegate string ResolveString(T id);

		private ResolveString m_ResolveMethod;
		private Dictionary<T, string> m_CachedResults = new Dictionary<T, string>();

		public BufferedStringTable(ResolveString method)
		{
			m_ResolveMethod = method;
		}

		public string GetString(T id)
		{
			string value;

			if (m_CachedResults.TryGetValue(id, out value))
				return value;

			value = m_ResolveMethod(id);
			m_CachedResults.Add(id, value);
			return value;
		}
	}
}
