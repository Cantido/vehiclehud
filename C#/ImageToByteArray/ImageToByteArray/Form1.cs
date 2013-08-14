using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace ImageToByteArray
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void fileBtn_Click(object sender, EventArgs e)
        {
            // Create an instance of the open file dialog box.
            OpenFileDialog openFile = new OpenFileDialog();

            // Set filter options and filter index.
            openFile.Filter = "Bitmap Files (.bmp)|*.bmp";
            openFile.FilterIndex = 1;

            openFile.Multiselect = false;

            // Call the ShowDialog method to show the dialog box.
            DialogResult userClickedOK = openFile.ShowDialog();

            // Process input if the user clicked OK.
            if (userClickedOK == DialogResult.OK)
            {
                rtbOutput.Clear();
                using (FileStream stream = new FileStream(openFile.FileName, FileMode.Open, FileAccess.Read))
                {
                    
                    Bitmap bm = (Bitmap)Image.FromFile(openFile.FileName);
                    pbInput.Image = bm;
                    Color pixel;

                    Byte[] byteArray = new Byte[bm.Width*(bm.Height)/8];

                    Byte tempByte = 0;

                    for (int t = 0; t < bm.Height / 8; ++t)
                    {
                        for (int x = 0; x < bm.Width; ++x)
                        {
                            tempByte = 0;
                            for (int y = 0; y < 8; ++y)
                            {
                                pixel = bm.GetPixel(x, (8 * t) + y);

                                if (pixel.R < 128 && pixel.G < 128 && pixel.B < 128)
                                    tempByte |= (byte)(1 << y);                                    
                            }

                            byteArray[x + bm.Width * t] = tempByte;
                        }
                    }

                    int z = 0;
                    foreach (var item in byteArray)
                    {
                        rtbOutput.Text += ("0x" + item.ToString("X2"));
                        ++z;
                        rtbOutput.Text += ((z % bm.Width == 0) ? "\n" : ", ");
                    }
                }
            }
            
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            try
            {
                if (rtbOutput.Text != "")
                    System.Windows.Forms.Clipboard.SetText(rtbOutput.Text);
            }
            catch (System.Runtime.InteropServices.ExternalException) {}
        }
    }
}
