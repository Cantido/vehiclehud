using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;

namespace OBD_Test
{
    public partial class Form1 : Form
    {
        public Form1()
        {   
            //define stuff we need
            string[] ports = SerialPort.GetPortNames();

            InitializeComponent();
            btnComm.Enabled = false;

            foreach (string port in ports)
                cmbPorts.Items.Add(port);

        }

        private void btnInit_Click(object sender, EventArgs e)
        {
            new Thread(OBDInit).Start();
        }

        private void btnComm_Click(object sender, EventArgs e)
        {
            new Thread(OBDComm2).Start();
        }

        public void UpdateTextBox(TextBox box, string value)
        {
            if (InvokeRequired)
            {
                this.Invoke(new Action<TextBox, string>(UpdateTextBox), new object[] {box, value });
                return;
            }
            box.Text = value;
        }

        int GetSpeed()
        {
            int i = 0;
            char inChar = ' ';
            char[] data = new char[11];

            //get rid of any junk in the buffer
            serialOBD.DiscardInBuffer();

            //ask for speed
            serialOBD.Write("010D\r\n");

            //wait until car is done responding
            //while (serialOBD.BytesToRead < 11) Thread.Sleep(25);

            //read in each character
            do
            {
                inChar = (char)serialOBD.ReadChar();
                if ((inChar != '\r') && (inChar != ' ') && (inChar != '>'))
                {
                    data[i] = inChar;
                    ++i;
                }
            } while (inChar != '\r');

            //translate data
            return Convert.ToInt16(new string(data, 4, 2), 16);
        }

        int GetRPM()
        {
            int i = 0;
            char inChar = ' ';
            char[] data = new char[14];
            //get rid of any junk in the buffer
            serialOBD.DiscardInBuffer();

            //ask for RPM
            serialOBD.Write("010C\r\n");

            //wait until car is done responding
            while (serialOBD.BytesToRead < 14) Thread.Sleep(25);

            //read in each character
            do
            {
                inChar = (char)serialOBD.ReadChar();
                if ((inChar != '\r') && (inChar != ' ') && (inChar != '>'))
                {
                    data[i] = inChar;
                    ++i;
                }
                
            } while (inChar != '\r');

            //translate data
            return (Convert.ToInt16(new string(data, 4, 2), 16) * 256 + Convert.ToInt16(new string(data, 6, 2), 16)) / 4;
        }

        void OBDInit()
        {
            //string for response from OBD chip
            string response;

            //close the port if it is open
            if (serialOBD.IsOpen)
                serialOBD.Close();            

            /* ------------------------ Set the port parameters ---------------------- */

            //use a delegate to access the GUI combobox across threads
            this.Invoke(new MethodInvoker(delegate() { serialOBD.PortName = cmbPorts.SelectedItem.ToString(); }));

            serialOBD.BaudRate = 9600;
            serialOBD.DataBits = 8;
            serialOBD.Parity = Parity.None;
            serialOBD.StopBits = StopBits.One;

            //open the port
            serialOBD.Open();
            this.Invoke(new MethodInvoker(delegate() { rtbPrgStat.Text += "Serial port opened.\n"; }));

            /* ----------------------- Initialize OBD Communication ---------------------- */

            //reset the OBD chip
            serialOBD.Write("ATZ\r\n");

            //wait for the OBD chip to reset
            Thread.Sleep(1000);

            //set baud rate to 115200
            serialOBD.Write("ATBRD 23\r\n");
            Thread.Sleep(200);

            serialOBD.Close();
            serialOBD.BaudRate = 115200;
            serialOBD.Open();
            serialOBD.Write("\r");

            Thread.Sleep(1000);

            this.Invoke(new MethodInvoker(delegate() { rtbPrgStat.Text += "Baud rate set.\n"; }));

            //ask the OBD chip to find a protocol (can send any command)
            serialOBD.Write("0100\r\n");

            //wait for the OBD chip to find a protocol
            Thread.Sleep(2000);

            //clean out the buffers
            serialOBD.DiscardInBuffer();
            serialOBD.DiscardOutBuffer();

            //make sure the chip is working
            do
            {
                this.Invoke(new MethodInvoker(delegate() { rtbPrgStat.Text += "Waiting for IGN response.\n"; }));
                serialOBD.Write("ATIGN\r\n");
                serialOBD.ReadTo("\r");
                response = serialOBD.ReadTo("\r");
                Thread.Sleep(100);
            } while (response != "ON");

            //diable echos
            serialOBD.Write("ATE0\r\n");

            this.Invoke(new MethodInvoker(delegate() { rtbPrgStat.Text += "OBD Chip reset and ready.\n"; }));
            this.Invoke(new MethodInvoker(delegate() { btnComm.Enabled = true; }));
            this.Invoke(new MethodInvoker(delegate() { btnInit.Enabled = false; }));
            //new Thread(OBDComm2).Start();
        }

        void OBDComm2()
        {
            int rpm = 0, oldrpm = 0;
            while (serialOBD.IsOpen)
            {
                oldrpm = rpm;
                rpm = GetRPM();
                UpdateTextBox(txtRPM, rpm.ToString());
                this.Invoke(new MethodInvoker(delegate() { barRPM.Value = rpm * 100 / 8000; }));

                //UpdateTextBox(txtSpeed, GetSpeed().ToString());
            }
        }

    }
}
