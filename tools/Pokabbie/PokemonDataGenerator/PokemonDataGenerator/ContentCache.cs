using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator
{
	public static class ContentCache
	{
		public static readonly string c_CacheFolder = Path.GetFullPath("content_cache");
		public static readonly string c_ResourcesFolder = Path.GetFullPath("../../Resources");

		private static string UriToCachePath(string uri)
		{
			if (uri.StartsWith(c_CacheFolder, StringComparison.CurrentCultureIgnoreCase))
			{
				return uri.ToLower();
			}
			else if (uri.StartsWith("res://", StringComparison.CurrentCultureIgnoreCase))
			{
				return uri.Replace("res://", c_ResourcesFolder + "/").ToLower();
			}
			else
			{
				string keyName = "F_" + Path.GetFileName(uri).ToLower()
					.Replace("://", "__")
					.Replace("?", "Q")
					.Replace("=", "E")
					.Replace("c", "c")
					.Replace(":", "S")
					.Replace(";", "S");

				string basePath = Path.GetDirectoryName(uri).ToLower()
					.Replace("://", "__")
					.Replace("?", "Q")
					.Replace("=", "E")
					.Replace("c", "c")
					.Replace(":", "S")
					.Replace(";", "S");

				string path = Path.Combine(c_CacheFolder, basePath, keyName);

				string baseDir = Path.GetDirectoryName(path);
				Directory.CreateDirectory(baseDir);

				return path;
			}
		}

		public static string GetWriteableCachePath(string path)
		{
			return UriToCachePath(path);
		}

		public static string GetHttpContent(string uri)
		{
			string cachePath = UriToCachePath(uri);
			if (File.Exists(cachePath))
				return File.ReadAllText(cachePath);

			using (HttpClient web = new HttpClient())
			{
				var task = web.GetStringAsync(uri);
				task.Wait();

				File.WriteAllText(cachePath, task.Result);
				return task.Result;
			}
		}

		public static Bitmap GetImageContent(string uri)
		{
			string cachePath = UriToCachePath(uri);
			if (File.Exists(cachePath))
				return new Bitmap(cachePath);

			WebRequest request = WebRequest.Create(uri);
			Bitmap result = new Bitmap(request.GetResponse().GetResponseStream());
			result.Save(cachePath);
			return result;
		}

		public static JObject GetJsonContent(string uri)
		{
			string content = GetHttpContent(uri);
			return JObject.Parse(content);
		}
	}
}
