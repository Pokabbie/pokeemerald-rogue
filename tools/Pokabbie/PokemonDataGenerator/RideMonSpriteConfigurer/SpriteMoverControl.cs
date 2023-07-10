using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace RideMonSpriteConfigurer
{
	public partial class SpriteMoverControl : UserControl
	{
		public delegate void MovementCallback(Point delta);

		public SpriteMoverControl()
		{
			InitializeComponent();
		}

		public event MovementCallback OnMovement;

		private void upButton_Click(object sender, EventArgs e)
		{
			OnMovement.Invoke(new Point(0, -1));
		}

		private void downButton_Click(object sender, EventArgs e)
		{
			OnMovement.Invoke(new Point(0, 1));
		}

		private void leftButton_Click(object sender, EventArgs e)
		{
			OnMovement.Invoke(new Point(-1, 0));
		}

		private void rightButton_Click(object sender, EventArgs e)
		{
			OnMovement.Invoke(new Point(1, 0));
		}
	}
}
