
namespace RideMonSpriteConfigurer
{
	partial class SpriteMoverControl
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.leftButton = new System.Windows.Forms.Button();
			this.downButton = new System.Windows.Forms.Button();
			this.upButton = new System.Windows.Forms.Button();
			this.rightButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// leftButton
			// 
			this.leftButton.Location = new System.Drawing.Point(3, 59);
			this.leftButton.Name = "leftButton";
			this.leftButton.Size = new System.Drawing.Size(50, 50);
			this.leftButton.TabIndex = 0;
			this.leftButton.Text = "◀";
			this.leftButton.UseVisualStyleBackColor = true;
			this.leftButton.Click += new System.EventHandler(this.leftButton_Click);
			// 
			// downButton
			// 
			this.downButton.Location = new System.Drawing.Point(59, 115);
			this.downButton.Name = "downButton";
			this.downButton.Size = new System.Drawing.Size(50, 50);
			this.downButton.TabIndex = 1;
			this.downButton.Text = "▼";
			this.downButton.UseVisualStyleBackColor = true;
			this.downButton.Click += new System.EventHandler(this.downButton_Click);
			// 
			// upButton
			// 
			this.upButton.Location = new System.Drawing.Point(59, 3);
			this.upButton.Name = "upButton";
			this.upButton.Size = new System.Drawing.Size(50, 50);
			this.upButton.TabIndex = 2;
			this.upButton.Text = "▲";
			this.upButton.UseVisualStyleBackColor = true;
			this.upButton.Click += new System.EventHandler(this.upButton_Click);
			// 
			// rightButton
			// 
			this.rightButton.Location = new System.Drawing.Point(115, 59);
			this.rightButton.Name = "rightButton";
			this.rightButton.Size = new System.Drawing.Size(50, 50);
			this.rightButton.TabIndex = 4;
			this.rightButton.Text = "▶";
			this.rightButton.UseVisualStyleBackColor = true;
			this.rightButton.Click += new System.EventHandler(this.rightButton_Click);
			// 
			// SpriteMoverControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.rightButton);
			this.Controls.Add(this.upButton);
			this.Controls.Add(this.downButton);
			this.Controls.Add(this.leftButton);
			this.Name = "SpriteMoverControl";
			this.Size = new System.Drawing.Size(168, 168);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button leftButton;
		private System.Windows.Forms.Button downButton;
		private System.Windows.Forms.Button upButton;
		private System.Windows.Forms.Button rightButton;
	}
}
