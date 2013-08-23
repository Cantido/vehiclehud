namespace OBD_Test
{
    partial class Form1
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.btnInit = new System.Windows.Forms.Button();
            this.txtSpeed = new System.Windows.Forms.TextBox();
            this.txtRPM = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.RPM = new System.Windows.Forms.Label();
            this.serialOBD = new System.IO.Ports.SerialPort(this.components);
            this.cmbPorts = new System.Windows.Forms.ComboBox();
            this.btnComm = new System.Windows.Forms.Button();
            this.rtbPrgStat = new System.Windows.Forms.RichTextBox();
            this.rtbOBDMsg = new System.Windows.Forms.RichTextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.barRPM = new System.Windows.Forms.ProgressBar();
            this.SuspendLayout();
            // 
            // btnInit
            // 
            this.btnInit.Location = new System.Drawing.Point(12, 87);
            this.btnInit.Name = "btnInit";
            this.btnInit.Size = new System.Drawing.Size(75, 23);
            this.btnInit.TabIndex = 0;
            this.btnInit.Text = "Initialize";
            this.btnInit.UseVisualStyleBackColor = true;
            this.btnInit.Click += new System.EventHandler(this.btnInit_Click);
            // 
            // txtSpeed
            // 
            this.txtSpeed.Location = new System.Drawing.Point(243, 108);
            this.txtSpeed.Name = "txtSpeed";
            this.txtSpeed.Size = new System.Drawing.Size(100, 20);
            this.txtSpeed.TabIndex = 1;
            // 
            // txtRPM
            // 
            this.txtRPM.Location = new System.Drawing.Point(243, 134);
            this.txtRPM.Name = "txtRPM";
            this.txtRPM.Size = new System.Drawing.Size(100, 20);
            this.txtRPM.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(196, 108);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Speed:";
            // 
            // RPM
            // 
            this.RPM.AutoSize = true;
            this.RPM.Location = new System.Drawing.Point(203, 134);
            this.RPM.Name = "RPM";
            this.RPM.Size = new System.Drawing.Size(34, 13);
            this.RPM.TabIndex = 4;
            this.RPM.Text = "RPM:";
            // 
            // cmbPorts
            // 
            this.cmbPorts.FormattingEnabled = true;
            this.cmbPorts.Location = new System.Drawing.Point(12, 60);
            this.cmbPorts.Name = "cmbPorts";
            this.cmbPorts.Size = new System.Drawing.Size(121, 21);
            this.cmbPorts.TabIndex = 5;
            // 
            // btnComm
            // 
            this.btnComm.Location = new System.Drawing.Point(243, 160);
            this.btnComm.Name = "btnComm";
            this.btnComm.Size = new System.Drawing.Size(75, 23);
            this.btnComm.TabIndex = 6;
            this.btnComm.Text = "Get Data";
            this.btnComm.UseVisualStyleBackColor = true;
            this.btnComm.Click += new System.EventHandler(this.btnComm_Click);
            // 
            // rtbPrgStat
            // 
            this.rtbPrgStat.Location = new System.Drawing.Point(28, 279);
            this.rtbPrgStat.Name = "rtbPrgStat";
            this.rtbPrgStat.Size = new System.Drawing.Size(172, 147);
            this.rtbPrgStat.TabIndex = 7;
            this.rtbPrgStat.Text = "";
            // 
            // rtbOBDMsg
            // 
            this.rtbOBDMsg.Location = new System.Drawing.Point(206, 279);
            this.rtbOBDMsg.Name = "rtbOBDMsg";
            this.rtbOBDMsg.Size = new System.Drawing.Size(193, 147);
            this.rtbOBDMsg.TabIndex = 8;
            this.rtbOBDMsg.Text = "";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(25, 263);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(79, 13);
            this.label2.TabIndex = 9;
            this.label2.Text = "Program Status";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(203, 263);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(86, 13);
            this.label3.TabIndex = 10;
            this.label3.Text = "OBD Responses";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(13, 41);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(104, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "Available COM Ports";
            // 
            // barRPM
            // 
            this.barRPM.Location = new System.Drawing.Point(243, 79);
            this.barRPM.Name = "barRPM";
            this.barRPM.Size = new System.Drawing.Size(100, 23);
            this.barRPM.TabIndex = 12;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(439, 438);
            this.Controls.Add(this.barRPM);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.rtbOBDMsg);
            this.Controls.Add(this.rtbPrgStat);
            this.Controls.Add(this.btnComm);
            this.Controls.Add(this.cmbPorts);
            this.Controls.Add(this.RPM);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtRPM);
            this.Controls.Add(this.txtSpeed);
            this.Controls.Add(this.btnInit);
            this.Name = "Form1";
            this.Text = "OBD Test";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnInit;
        private System.Windows.Forms.TextBox txtSpeed;
        private System.Windows.Forms.TextBox txtRPM;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label RPM;
        private System.IO.Ports.SerialPort serialOBD;
        private System.Windows.Forms.ComboBox cmbPorts;
        private System.Windows.Forms.Button btnComm;
        private System.Windows.Forms.RichTextBox rtbPrgStat;
        private System.Windows.Forms.RichTextBox rtbOBDMsg;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ProgressBar barRPM;
    }
}

