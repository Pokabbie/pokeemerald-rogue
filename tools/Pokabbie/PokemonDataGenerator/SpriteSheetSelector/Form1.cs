using Newtonsoft.Json;
using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SpriteSheetSelector
{
	public partial class Form1 : Form
	{
		private class ExportData
		{
			public SortedDictionary<string, string> monPaletteAssignment = new SortedDictionary<string, string>();
		}

		private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings 
		{
			Formatting = Formatting.Indented
		};

#if DEBUG
		private static readonly string c_SpriteDirectory = Path.GetFullPath(@"..\..\..\PokemonDataGenerator\bin\Debug\sprite_out");
#else
		private static readonly string c_SpriteDirectory = Path.GetFullPath(@"..\..\..\PokemonDataGenerator\bin\Release\sprite_out");
#endif
		private static readonly string c_ExportFilePath = Path.Combine(c_SpriteDirectory, @"..\..\..\mon_palette_mapping.json");

		private List<PictureBox> m_OrderedPictureBoxes = new List<PictureBox>();

		private Queue<string> m_MonsToCheck = new Queue<string>();
		private string m_CurrentMon = null;
		private ExportData m_OutputData = new ExportData();

		public Form1()
		{
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			// Setup boxes
			m_OrderedPictureBoxes.Add(pictureBox1);
			m_OrderedPictureBoxes.Add(pictureBox2);
			m_OrderedPictureBoxes.Add(pictureBox3);
			m_OrderedPictureBoxes.Add(pictureBox4);
			m_OrderedPictureBoxes.Add(pictureBox5);
			m_OrderedPictureBoxes.Add(pictureBox6);
			m_OrderedPictureBoxes.Add(pictureBox7);
			m_OrderedPictureBoxes.Add(pictureBox8);
			m_OrderedPictureBoxes.Add(pictureBox9);
			m_OrderedPictureBoxes.Add(pictureBox10);
			m_OrderedPictureBoxes.Add(pictureBox11);
			m_OrderedPictureBoxes.Add(pictureBox12);
			m_OrderedPictureBoxes.Add(pictureBox13);
			m_OrderedPictureBoxes.Add(pictureBox14);
			m_OrderedPictureBoxes.Add(pictureBox15);
			m_OrderedPictureBoxes.Add(pictureBox16);
			m_OrderedPictureBoxes.Add(pictureBox17);
			m_OrderedPictureBoxes.Add(pictureBox18);

			for(int i = 0; i < m_OrderedPictureBoxes.Count; ++i)
			{
				m_OrderedPictureBoxes[i].Click += Form1_PictureSelectClick;
			}

			// Load source data
			if (File.Exists(c_ExportFilePath))
			{
				string jsonData = File.ReadAllText(c_ExportFilePath);

				m_OutputData = JsonConvert.DeserializeObject<ExportData>(jsonData, c_JsonSettings);
			}

			foreach (var file in Directory.EnumerateFiles(Path.Combine(c_SpriteDirectory, "raw")))
			{
				string monName = Path.GetFileNameWithoutExtension(file);

				if(!m_OutputData.monPaletteAssignment.ContainsKey(monName))// || monName.EndsWith("_shiny"))
					m_MonsToCheck.Enqueue(monName);
			}

			SetupNextMon();
		}

		private void Form1_PictureSelectClick(object sender, EventArgs e)
		{
			int index = Array.IndexOf(m_OrderedPictureBoxes.ToArray(), sender);

			if(index != -1)
			{
				// Set data and save
				m_OutputData.monPaletteAssignment[m_CurrentMon] = $"pal_{index}";

				string jsonData = JsonConvert.SerializeObject(m_OutputData, c_JsonSettings);
				File.WriteAllText(c_ExportFilePath, jsonData);

				SetupNextMon();
			}
		}

		private void SetScaledImage(PictureBox pictureBox, string path)
		{
			string fullPath = Path.Combine(c_SpriteDirectory, path);
			pictureBox.Enabled = false;

			if (File.Exists(fullPath))
			{
				Image sourceImg = Image.FromFile(fullPath);
				Bitmap scaledImg = new Bitmap(pictureBox.Width, pictureBox.Height);

				using (Graphics gfx = Graphics.FromImage(scaledImg))
				{
					gfx.InterpolationMode = InterpolationMode.NearestNeighbor;
					gfx.PixelOffsetMode = PixelOffsetMode.Half;

					gfx.DrawImage(sourceImg, new Rectangle(Point.Empty, scaledImg.Size));
				}

				pictureBox.Image = scaledImg;
				pictureBox.Enabled = true;
			}
			else
			{
				//pictureBox.Image = new Bitmap(1, 1);
				//pictureBox.Enabled = false;
			}
		}

		private void SetupNextMon()
		{
			if(m_MonsToCheck.Count != 0)
			{
				m_CurrentMon = m_MonsToCheck.Dequeue();

				SetupMon(m_CurrentMon);
			}
			else
			{
				m_CurrentMon = null;
				primaryLabel.Text = "[= FINISHED =]";

				sourcePictureBox.Enabled = false;

				for (int i = 0; i < 16; ++i)
					m_OrderedPictureBoxes[i].Enabled = false;
			}
		}

		private void SetupMon(string mon)
		{
			primaryLabel.Text = mon;
			secondaryLabel.Text = $"{m_MonsToCheck.Count} / {m_MonsToCheck.Count + m_OutputData.monPaletteAssignment.Count} Remaining";

			m_OutputData.monPaletteAssignment.TryGetValue(mon, out string assignedPal);

			SetScaledImage(sourcePictureBox, $"raw/{mon}.png");

			for (int i = 0; i < m_OrderedPictureBoxes.Count; ++i)
			{
				bool isAssigned = assignedPal == $"pal_{i}";
				SetScaledImage(m_OrderedPictureBoxes[i], $"pal_{i}/{mon}.png");

				m_OrderedPictureBoxes[i].BorderStyle = isAssigned ? BorderStyle.FixedSingle : BorderStyle.None;
			}
		}

		private void textBox1_TextChanged(object sender, EventArgs e)
		{
			if (m_CurrentMon != null)
			{
				// Re-add to front of queue
				var list = m_MonsToCheck.ToList();
				list.Insert(0, m_CurrentMon);

				m_MonsToCheck = new Queue<string>(list);

				m_CurrentMon = null;
			}

			string mon = textBox1.Text;
			SetupMon("");

			if (File.Exists(Path.Combine(c_SpriteDirectory, $"raw/{mon}.png")))
			{
				m_CurrentMon = mon;
				SetupMon(mon);
			}
		}

		private void sourcePictureBox_Click(object sender, EventArgs e)
		{

			foreach (var file in Directory.EnumerateFiles(Path.Combine(c_SpriteDirectory, "raw")))
			{
				string monName = Path.GetFileNameWithoutExtension(file);

				if (!m_OutputData.monPaletteAssignment.ContainsKey(monName))// || monName.EndsWith("_shiny"))
					m_MonsToCheck.Enqueue(monName);
			}

		}

		private void button1_Click(object sender, EventArgs e)
		{
			string species = "SPECIES_" + GameDataHelpers.FormatKeyword(textBox2.Text);

			if(GameDataHelpers.SpeciesDefines.ContainsKey(species))
			{
				bool enqueue = false;

				foreach(var kvp in GameDataHelpers.SpeciesDefines)
				{
					if (!enqueue)
					{
						if (kvp.Key == species)
							enqueue = true;
						else
							continue;
					}

					if(enqueue)
					{
						string monName = kvp.Key.Substring("SPECIES_".Length).ToLower();
						m_MonsToCheck.Enqueue(monName);
					}
				}
			}

			SetupNextMon();
		}

		private void button2_Click(object sender, EventArgs e)
		{
			string species = "SPECIES_" + GameDataHelpers.FormatKeyword(textBox2.Text);

			if (GameDataHelpers.SpeciesDefines.ContainsKey(species))
			{
				bool enqueue = false;

				foreach (var kvp in GameDataHelpers.SpeciesDefines)
				{
					if (!enqueue)
					{
						if (kvp.Key == species)
							enqueue = true;
						else
							continue;
					}

					if (enqueue)
					{
						string monName = kvp.Key.Substring("SPECIES_".Length).ToLower();
						m_MonsToCheck.Enqueue(monName + "_shiny");
					}
				}
			}

			SetupNextMon();
		}
	}
}
