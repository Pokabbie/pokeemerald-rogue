using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;

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

		public static bool ExistsInCache(string uri)
		{
			string cachePath = UriToCachePath(uri);
			return File.Exists(cachePath);
		}

		public static string GetHttpContent(string uri)
		{
			string cachePath = UriToCachePath(uri);
			if (File.Exists(cachePath))
				return File.ReadAllText(cachePath);

			using (HttpClient web = new HttpClient())
			{
				var task = web.GetStringAsync(uri);
				task.Wait(); // if we fail here, can manually populate cache to work around

				File.WriteAllText(cachePath, task.Result);
				return task.Result;
			}
        }

		public static string ParseHttpContentPlainText(string uri)
		{
			string htmlStr = GetHttpContent(uri);
            return HtmlToPlainText(htmlStr);
        }

        private static string HtmlToPlainText(string html)
        {
            const string tagWhiteSpace = @"(>|$)(\W|\n|\r)+<";//matches one or more (white space or line breaks) between '>' and '<'
            const string stripFormatting = @"<[^>]*(>|$)";//match any character between '<' and '>', even when end tag is missing
            const string lineBreak = @"<(br|BR)\s{0,1}\/{0,1}>";//matches: <br>,<br/>,<br />,<BR>,<BR/>,<BR />
            var lineBreakRegex = new Regex(lineBreak, RegexOptions.Multiline);
            var stripFormattingRegex = new Regex(stripFormatting, RegexOptions.Multiline);
            var tagWhiteSpaceRegex = new Regex(tagWhiteSpace, RegexOptions.Multiline);

            var text = html;
            //Decode html specific characters
            text = System.Net.WebUtility.HtmlDecode(text);
            //Remove tag whitespace/line breaks
            text = tagWhiteSpaceRegex.Replace(text, "><");
            //Replace <br /> with line breaks
            text = lineBreakRegex.Replace(text, Environment.NewLine);
            //Strip formatting
            text = stripFormattingRegex.Replace(text, string.Empty);

            return text;
        }

        private static Stream StringToStream(string s)
        {
            var stream = new MemoryStream();
            var writer = new StreamWriter(stream);
            writer.Write(s);
            writer.Flush();
            stream.Position = 0;
            return stream;
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
