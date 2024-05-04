using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace PaletteReduceAssist
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		private Bitmap m_CurrentImage;
		private Stack<Bitmap> m_ImageHistory = new Stack<Bitmap>();
		private int m_CurrentColourIndex;
		private int m_CurrentColourToMergeWith;
		private List<System.Drawing.Color> m_CurrentPalette;
		private Dictionary<System.Drawing.Color, int> m_CurrentPaletteSortScores;

		public MainWindow()
		{
			InitializeComponent();
		}

		private void Button_LoadImage(object sender, RoutedEventArgs e)
		{
			OpenFileDialog fileDialog = new OpenFileDialog();
			fileDialog.RestoreDirectory = true;
			fileDialog.Title = "Select Image to Reduce";
			fileDialog.DefaultExt = "png";

			fileDialog.ShowDialog();

			if (!string.IsNullOrWhiteSpace(fileDialog.FileName))
				LoadNewImage(fileDialog.FileName);
		}

		private void Button_SaveImage(object sender, RoutedEventArgs e)
		{
			SaveFileDialog fileDialog = new SaveFileDialog();
			fileDialog.RestoreDirectory = true;
			fileDialog.Title = "Select Image to Save";
			fileDialog.DefaultExt = "png";

			fileDialog.ShowDialog();

			if (!string.IsNullOrWhiteSpace(fileDialog.FileName))
				m_CurrentImage.Save(fileDialog.FileName);
		}

		private void Button_PrevSourceColour(object sender, RoutedEventArgs e)
		{
			--m_CurrentColourIndex;

			if (m_CurrentColourIndex < 0)
				m_CurrentColourIndex = m_CurrentPalette.Count - 1;

			SelectNearestTargetColour();

			UpdateCurrentDisplayedColor();
			UpdateAfterImage();
		}

		private void Button_NextSourceColour(object sender, RoutedEventArgs e)
		{
			m_CurrentColourIndex = (m_CurrentColourIndex + 1) % m_CurrentPalette.Count;

			SelectNearestTargetColour();

			UpdateCurrentDisplayedColor();
			UpdateAfterImage();
		}

		private void Button_PrevTargetColour(object sender, RoutedEventArgs e)
		{
			--m_CurrentColourToMergeWith;

			if (m_CurrentColourToMergeWith < 0)
				m_CurrentColourToMergeWith = m_CurrentPalette.Count - 1;

			UpdateCurrentDisplayedColor();
			UpdateAfterImage();
		}

		private void Button_NextTargetColour(object sender, RoutedEventArgs e)
		{
			m_CurrentColourToMergeWith = (m_CurrentColourToMergeWith + 1) % m_CurrentPalette.Count;

			UpdateCurrentDisplayedColor();
			UpdateAfterImage();
		}

		private void Button_CommitColour(object sender, RoutedEventArgs e)
		{
			m_ImageHistory.Push(new Bitmap(m_CurrentImage));

			for (int y = 0; y < m_CurrentImage.Height; ++y)
			{
				for (int x = 0; x < m_CurrentImage.Width; ++x)
				{
					var pixel = SourceColourToKey(m_CurrentImage.GetPixel(x, y));

					if (pixel == m_CurrentPalette[m_CurrentColourIndex])
					{
						m_CurrentImage.SetPixel(x, y, m_CurrentPalette[m_CurrentColourToMergeWith]);
					}
				}
			}

			this.BeforeImage.Source = ConvertBitmapToSource(PrepareBitmapForViewing(m_CurrentImage));
			UpdatePaletteCount();
		}

		private void Button_UndoColour(object sender, RoutedEventArgs e)
		{
			if(m_ImageHistory.Count != 0)
			{
				m_CurrentImage = m_ImageHistory.Pop();

				this.BeforeImage.Source = ConvertBitmapToSource(PrepareBitmapForViewing(m_CurrentImage));
				UpdatePaletteCount();
			}
		}

		private void LoadNewImage(string filePath)
		{
			m_CurrentImage = new Bitmap(filePath);
			m_ImageHistory = new Stack<Bitmap>();

			this.BeforeImage.Source = ConvertBitmapToSource(PrepareBitmapForViewing(m_CurrentImage));
			UpdatePaletteCount();
		}

		private System.Drawing.Color SourceColourToKey(System.Drawing.Color src)
		{
			if (src.A < 255)
			{
				src = System.Drawing.Color.Transparent;
			}

			return src;
		}

		private void UpdatePaletteCount()
		{
			m_CurrentPaletteSortScores = new Dictionary<System.Drawing.Color, int>();

			// Default scores are the pixel count
			for (int y = 0; y < m_CurrentImage.Height; ++y)
			{
				for (int x = 0; x < m_CurrentImage.Width; ++x)
				{
					var pixel = SourceColourToKey(m_CurrentImage.GetPixel(x, y));

					if (!m_CurrentPaletteSortScores.ContainsKey(pixel))
						m_CurrentPaletteSortScores[pixel] = 0;

					++m_CurrentPaletteSortScores[pixel];
				}
			}

			m_CurrentPalette = new List<System.Drawing.Color>(m_CurrentPaletteSortScores.OrderBy((kvp) => kvp.Value).Select((kvp) => kvp.Key));

			// Actually change out mind and change scores to the closest pixel
			if(!SortByPixelCount.IsChecked.GetValueOrDefault())
			{
				foreach (var col in m_CurrentPaletteSortScores.Keys.ToArray())
				{
					int index = SelectNearestColour(col, out int score);
					m_CurrentPaletteSortScores[col] = score;

					if (col.A < 255)
						m_CurrentPaletteSortScores[col] = int.MaxValue;
				}

				// Recreate + sort
				m_CurrentPalette = new List<System.Drawing.Color>(m_CurrentPaletteSortScores.OrderBy((kvp) => kvp.Value).Select((kvp) => kvp.Key));
			}

			this.PaletteSizeText.Text = "" + m_CurrentPalette.Count + " total";

			m_CurrentColourIndex = 0;
			SelectNearestTargetColour();

			UpdateCurrentDisplayedColor();
			UpdateAfterImage();
		}

		private void SelectNearestTargetColour()
		{
			m_CurrentColourToMergeWith = SelectNearestColour(m_CurrentPalette[m_CurrentColourIndex], out _);
		}

		private int SelectNearestColour(System.Drawing.Color key, out int outScore)
		{
			int bestIndex = 0;
			int lowestScore = int.MaxValue;

			for (int i = 0; i < m_CurrentPalette.Count; ++i)
			{
				if (m_CurrentPalette[i] == key || m_CurrentPalette[i].A < 255)
					continue;

				int dr = m_CurrentPalette[i].R - key.R;
				int dg = m_CurrentPalette[i].G - key.G;
				int db = m_CurrentPalette[i].B - key.B;

				int score = (dr * dr) + (dg * dg) + (db * db);

				if (score < lowestScore)
				{
					lowestScore = score;
					bestIndex = i;
				}
			}

			outScore = lowestScore;
			return bestIndex;
		}

		private void UpdateCurrentDisplayedColor()
		{
			System.Windows.Media.Color col = new System.Windows.Media.Color();
			col.R = m_CurrentPalette[m_CurrentColourIndex].R;
			col.G = m_CurrentPalette[m_CurrentColourIndex].G;
			col.B = m_CurrentPalette[m_CurrentColourIndex].B;
			col.A = m_CurrentPalette[m_CurrentColourIndex].A;
			this.PaletteColour.Fill = new SolidColorBrush(col);
			this.PaletteName.Text = m_CurrentPalette[m_CurrentColourIndex].Name + " = " + m_CurrentPaletteSortScores[m_CurrentPalette[m_CurrentColourIndex]] + " score";

			col = new System.Windows.Media.Color();
			col.R = m_CurrentPalette[m_CurrentColourToMergeWith].R;
			col.G = m_CurrentPalette[m_CurrentColourToMergeWith].G;
			col.B = m_CurrentPalette[m_CurrentColourToMergeWith].B;
			col.A = m_CurrentPalette[m_CurrentColourToMergeWith].A;
			this.TargetPaletteColour.Fill = new SolidColorBrush(col);
			this.TargetPaletteName.Text = m_CurrentPalette[m_CurrentColourToMergeWith].Name + " = " + m_CurrentPaletteSortScores[m_CurrentPalette[m_CurrentColourToMergeWith]] + " score";


			Bitmap highlightImg = new Bitmap(m_CurrentImage);

			for (int y = 0; y < highlightImg.Height; ++y)
			{
				for (int x = 0; x < highlightImg.Width; ++x)
				{
					var pixel = SourceColourToKey(highlightImg.GetPixel(x, y));

					if (pixel == m_CurrentPalette[m_CurrentColourIndex])
					{
						highlightImg.SetPixel(x, y, System.Drawing.Color.Red);
					}
					else if (pixel == m_CurrentPalette[m_CurrentColourToMergeWith])
					{
						highlightImg.SetPixel(x, y, System.Drawing.Color.Blue);
					}
					else
					{
						// TODO - outline


						//System.Drawing.Color highLightColour = System.Drawing.Color.FromArgb(
						//	255,
						//	255 - m_CurrentPalette[m_CurrentColourIndex].R,
						//	255 - m_CurrentPalette[m_CurrentColourIndex].G,
						//	255 - m_CurrentPalette[m_CurrentColourIndex].B
						//);
					}
				}
			}

			this.HighlightImage.Source = ConvertBitmapToSource(PrepareBitmapForViewing(highlightImg));
		}

		private void UpdateAfterImage()
		{
			Bitmap afterImg = new Bitmap(m_CurrentImage);

			for (int y = 0; y < afterImg.Height; ++y)
			{
				for (int x = 0; x < afterImg.Width; ++x)
				{
					var pixel = SourceColourToKey(afterImg.GetPixel(x, y));

					if (pixel == m_CurrentPalette[m_CurrentColourIndex])
					{
						afterImg.SetPixel(x, y, m_CurrentPalette[m_CurrentColourToMergeWith]);
					}
				}
			}

			this.AfterImage.Source = ConvertBitmapToSource(PrepareBitmapForViewing(afterImg));
		}

		public static Bitmap PrepareBitmapForViewing(Bitmap inputImg)
		{
			int upscaleAmount = 8;
			Bitmap outputImg = new Bitmap(inputImg.Width * upscaleAmount, inputImg.Height * upscaleAmount);

			for (int y = 0; y < inputImg.Height; ++y)
			{
				for (int x = 0; x < inputImg.Width; ++x)
				{
					var pixel = inputImg.GetPixel(x, y);

					for (int dy = 0; dy < upscaleAmount; ++dy)
						for (int dx = 0; dx < upscaleAmount; ++dx)
						{
							outputImg.SetPixel(
								x * upscaleAmount + dx,
								y * upscaleAmount + dy,
								pixel
							);
						}
				}
			}

			return outputImg;
		}

		public static BitmapSource ConvertBitmapToSource(Bitmap bitmap)
		{
			if (bitmap == null)
				throw new ArgumentNullException("bitmap");

			return System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
				bitmap.GetHbitmap(),
				IntPtr.Zero,
				Int32Rect.Empty,
				BitmapSizeOptions.FromEmptyOptions());
		}

		private void SortByPixelCount_Checked(object sender, RoutedEventArgs e)
		{
			UpdatePaletteCount();
		}
	}
}
