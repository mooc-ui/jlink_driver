using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace hxTestTool
{
    public class Log
    {
        private string logFile;
        private StreamWriter writer;
        private FileStream fileStream = null;
        private bool add_time_and_end = true;

        public Log(string fileName)
        {
            logFile = fileName;
            CreateDirectory(logFile);
        }

        public Log(string fileName, bool v) 
        {
            logFile = fileName;
            CreateDirectory(logFile);
            this.add_time_and_end = v;
        }

        public void log(string info)
        {

            try
            {
                System.IO.FileInfo fileInfo = new System.IO.FileInfo(logFile);
                if (!fileInfo.Exists)
                {
                    fileStream = fileInfo.Create();
                    writer = new StreamWriter(fileStream);
                }
                else
                {
                    fileStream = fileInfo.Open(FileMode.Append, FileAccess.Write);
                    writer = new StreamWriter(fileStream);
                }
                if(add_time_and_end)
                   writer.WriteLine(DateTime.Now + ": " + info);
                else
                    writer.Write(info);
            }
            finally
            {
                if (writer != null)
                {
                    writer.Close();
                    writer.Dispose();
                    fileStream.Close();
                    fileStream.Dispose();
                }
            }
        }

        public void CreateDirectory(string infoPath)
        {
            DirectoryInfo directoryInfo = Directory.GetParent(infoPath);
            if (!directoryInfo.Exists)
            {
                directoryInfo.Create();
            }
        }

        internal void WriteLine()
        {
            log("\r\n");
        }

        internal void WriteLine(string v)
        {
            log(v + "\r\n");
        }

        internal void Write(string v)
        {
            log(v);
        }
        internal void Write(char v)
        {
            log(v.ToString());
        }
    }
}
