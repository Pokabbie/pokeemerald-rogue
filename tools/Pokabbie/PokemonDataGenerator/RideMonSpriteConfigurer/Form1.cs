using Newtonsoft.Json;
using PokemonDataGenerator.Utils;
using RideMonSpriteConfigurer.Helpers;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Windows.Forms;

namespace RideMonSpriteConfigurer
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

		private string m_CurrentMon = null;
		private SpeciesRideMonInfo m_CurrentRideMonInfo = null;

#if DEBUG
		private static readonly string c_SpriteDirectory = Path.GetFullPath(@"..\..\..\PokemonDataGenerator\bin\Debug\sprite_out");
#else
		private static readonly string c_SpriteDirectory = Path.GetFullPath(@"..\..\..\PokemonDataGenerator\bin\Release\sprite_out");
#endif
		private static readonly string c_ExportFilePath = Path.Combine(c_SpriteDirectory, @"..\..\..\mon_palette_mapping.json");

		private List<PictureBox> m_OrderedPictureBoxes = new List<PictureBox>();

		private Queue<string> m_MonsToCheck = new Queue<string>();



		private ExportData m_OutputData = new ExportData();

		public Form1()
		{
			InitializeComponent();

			// Down controls
			{
				var downSpriteMover0 = new SpriteMoverControl();
				downSpriteMover0.Location = new Point(45, 200);
				downSpriteMover0.OnMovement += (Point delta) =>
				{
					if (m_CurrentRideMonInfo != null)
					{
						m_CurrentRideMonInfo.DownSprite.RiderOffsetX += delta.X;
						m_CurrentRideMonInfo.DownSprite.RiderOffsetY += delta.Y;
						RefreshDisplayedSprites();
					}
				};
				this.Controls.Add(downSpriteMover0);

				var downSpriteMover1 = new SpriteMoverControl();
				downSpriteMover1.Location = new Point(45, 200 + 150);
				downSpriteMover1.OnMovement += (Point delta) =>
				{
					if (m_CurrentRideMonInfo != null)
					{
						m_CurrentRideMonInfo.DownSprite.MonOffsetX += delta.X;
						m_CurrentRideMonInfo.DownSprite.MonOffsetY += delta.Y;
						RefreshDisplayedSprites();
					}
				};
				this.Controls.Add(downSpriteMover1);
			}

			// Up controls
			{
				var downSpriteMover0 = new SpriteMoverControl();
				downSpriteMover0.Location = new Point(45 + 200, 200);
				downSpriteMover0.OnMovement += (Point delta) =>
				{
					if (m_CurrentRideMonInfo != null)
					{
						m_CurrentRideMonInfo.UpSprite.RiderOffsetX += delta.X;
						m_CurrentRideMonInfo.UpSprite.RiderOffsetY += delta.Y;
						RefreshDisplayedSprites();
					}
				};
				this.Controls.Add(downSpriteMover0);

				var downSpriteMover1 = new SpriteMoverControl();
				downSpriteMover1.Location = new Point(45 + 200, 200 + 150);
				downSpriteMover1.OnMovement += (Point delta) =>
				{
					if (m_CurrentRideMonInfo != null)
					{
						m_CurrentRideMonInfo.UpSprite.MonOffsetX += delta.X;
						m_CurrentRideMonInfo.UpSprite.MonOffsetY += delta.Y;
						RefreshDisplayedSprites();
					}
				};
				this.Controls.Add(downSpriteMover1);
			}

			// Side controls
			{
				var downSpriteMover0 = new SpriteMoverControl();
				downSpriteMover0.Location = new Point(45 + 400, 200);
				downSpriteMover0.OnMovement += (Point delta) =>
				{
					if (m_CurrentRideMonInfo != null)
					{
						m_CurrentRideMonInfo.SideSprite.RiderOffsetX += delta.X;
						m_CurrentRideMonInfo.SideSprite.RiderOffsetY += delta.Y;
						RefreshDisplayedSprites();
					}
				};
				this.Controls.Add(downSpriteMover0);

				var downSpriteMover1 = new SpriteMoverControl();
				downSpriteMover1.Location = new Point(45 + 400, 200 + 150);
				downSpriteMover1.OnMovement += (Point delta) =>
				{
					if (m_CurrentRideMonInfo != null)
					{
						m_CurrentRideMonInfo.SideSprite.MonOffsetX += delta.X;
						m_CurrentRideMonInfo.SideSprite.MonOffsetY += delta.Y;
						RefreshDisplayedSprites();
					}
				};
				this.Controls.Add(downSpriteMover1);
			}
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			OnRideMonInfoSelected();
		}

		private void SetScaledImage(PictureBox pictureBox, Image sourceImg)
		{
			pictureBox.Enabled = false;

			if (sourceImg == null)
			{
				sourceImg = new Bitmap(64, 64);
			}

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

		private void textBox1_TextChanged(object sender, EventArgs e)
		{
			string monName = textBox1.Text;

			m_CurrentRideMonInfo = RideMonInfoHelper.FindRideInfo(monName);
			OnRideMonInfoSelected();
		}

		private void OnRideMonInfoSelected()
		{
			downRiderInFrontCheckBox.Checked = false;
			upRiderInFrontCheckBox.Checked = false;
			sideRiderInFrontCheckBox.Checked = false;

			if(m_CurrentRideMonInfo != null)
			{
				downRiderInFrontCheckBox.Checked = m_CurrentRideMonInfo.DownSprite.RiderRendersInFront;
				upRiderInFrontCheckBox.Checked = m_CurrentRideMonInfo.UpSprite.RiderRendersInFront;
				sideRiderInFrontCheckBox.Checked = m_CurrentRideMonInfo.SideSprite.RiderRendersInFront;

				canClimbCheckBox.Checked = m_CurrentRideMonInfo.SupportsClimbing;
				canSwimCheckBox.Checked = m_CurrentRideMonInfo.SupportsSwimming;
				canFlyCheckBox.Checked = m_CurrentRideMonInfo.SupportsFlying;
			}
			else
			{
				downRiderInFrontCheckBox.Checked = false;
				upRiderInFrontCheckBox.Checked = false;
				sideRiderInFrontCheckBox.Checked = false;

				canClimbCheckBox.Checked = false;
				canSwimCheckBox.Checked = false;
				canFlyCheckBox.Checked = false;
			}

			RefreshHeaders();
			RefreshDisplayedSprites();
		}

		private void RefreshHeaders()
		{
			if (m_CurrentRideMonInfo == null)
				primaryLabel.Text = "none selected";
			else
				primaryLabel.Text = m_CurrentRideMonInfo.SpeciesName;
		}

		private void RefreshDisplayedSprites()
		{
			SetScaledImage(downPictureBox0, null);
			SetScaledImage(downPictureBox1, null);
			SetScaledImage(upPictureBox0, null);
			SetScaledImage(upPictureBox1, null);
			SetScaledImage(sidePictureBox0, null);
			SetScaledImage(sidePictureBox1, null);

			if (m_CurrentRideMonInfo != null)
			{
				string riderOverworldSprite = Path.Combine(GameDataHelpers.RootDirectory, "graphics\\object_events\\pics\\people\\brendan\\riding.png");
				string monOverworldSprite = Path.Combine(GameDataHelpers.RootDirectory, "graphics\\object_events\\pics\\pokemon_ow\\" + m_CurrentRideMonInfo.SpeciesName + ".png");

				if (File.Exists(riderOverworldSprite) && File.Exists(monOverworldSprite))
				{
					Image riderSourceImg = Image.FromFile(riderOverworldSprite);
					Image monSourceImg = Image.FromFile(monOverworldSprite);

					SetScaledImage(downPictureBox0, RideMonInfoHelper.CreateRidingSprite(riderSourceImg, 0, monSourceImg, 0, m_CurrentRideMonInfo.DownSprite));
					SetScaledImage(downPictureBox1, RideMonInfoHelper.CreateRidingSprite(riderSourceImg, 0, monSourceImg, 3, m_CurrentRideMonInfo.DownSprite));
					SetScaledImage(upPictureBox0, RideMonInfoHelper.CreateRidingSprite(riderSourceImg, 1, monSourceImg, 1, m_CurrentRideMonInfo.UpSprite));
					SetScaledImage(upPictureBox1, RideMonInfoHelper.CreateRidingSprite(riderSourceImg, 1, monSourceImg, 4, m_CurrentRideMonInfo.UpSprite));
					SetScaledImage(sidePictureBox0, RideMonInfoHelper.CreateRidingSprite(riderSourceImg, 2, monSourceImg, 2, m_CurrentRideMonInfo.SideSprite));
					SetScaledImage(sidePictureBox1, RideMonInfoHelper.CreateRidingSprite(riderSourceImg, 2, monSourceImg, 5, m_CurrentRideMonInfo.SideSprite));
				}
			}
		}

		private void createRideMonInfo_Click(object sender, EventArgs e)
		{
			string monName = textBox1.Text;

			m_CurrentRideMonInfo = RideMonInfoHelper.TryCreateRideInfo(monName);
			OnRideMonInfoSelected();
		}

		private void deleteRideMonInfo_Click(object sender, EventArgs e)
		{
			string monName = textBox1.Text;
			RideMonInfoHelper.DeleteRideInfo(monName);

			m_CurrentRideMonInfo = RideMonInfoHelper.FindRideInfo(monName);
			OnRideMonInfoSelected();
		}

		private void downRiderInFrontCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_CurrentRideMonInfo != null)
			{
				m_CurrentRideMonInfo.DownSprite.RiderRendersInFront = downRiderInFrontCheckBox.Checked;
				RefreshDisplayedSprites();
			}
		}

		private void upRiderInFrontCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_CurrentRideMonInfo != null)
			{
				m_CurrentRideMonInfo.UpSprite.RiderRendersInFront = upRiderInFrontCheckBox.Checked;
				RefreshDisplayedSprites();
			}
		}

		private void sideRiderInFrontCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_CurrentRideMonInfo != null)
			{
				m_CurrentRideMonInfo.SideSprite.RiderRendersInFront = sideRiderInFrontCheckBox.Checked;
				RefreshDisplayedSprites();
			}
		}

		private void canClimbCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_CurrentRideMonInfo != null)
			{
				m_CurrentRideMonInfo.SupportsClimbing = canClimbCheckBox.Checked;
			}
		}

		private void canSwimCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_CurrentRideMonInfo != null)
			{
				m_CurrentRideMonInfo.SupportsSwimming = canSwimCheckBox.Checked;
			}
		}

		private void canFlyCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_CurrentRideMonInfo != null)
			{
				m_CurrentRideMonInfo.SupportsFlying = canFlyCheckBox.Checked;
			}
		}

		private void prevEntryButton_Click(object sender, EventArgs e)
		{
			textBox1.Text = RideMonInfoHelper.GetRelativeSpeciesName(textBox1.Text, -1);
		}

		private void nextEntryButton_Click(object sender, EventArgs e)
		{
			textBox1.Text = RideMonInfoHelper.GetRelativeSpeciesName(textBox1.Text, 1);
		}

		private void saveSourceButton_Click(object sender, EventArgs e)
		{
			RideMonInfoHelper.ExportSourceData();
		}

		private void exportDataButton_Click(object sender, EventArgs e)
		{
			RideMonInfoHelper.ExportGameData();
		}
	}
}
