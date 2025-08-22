using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace hxTestTool
{
    class ComPort : Object
    {
        public bool ShowData = false;
        private int port = -1;
        private uint _RecvTimeoutInMs = 50;
        private uint _Baudrate = 115200;
        Log log = new Log(AppDomain.CurrentDomain.BaseDirectory 
            + @"Log_Serial_" + DateTime.Now.ToShortDateString() + ".txt");

        public uint Baudrate
        {
            get { return _Baudrate; }
            set { _Baudrate = value; }
        }
        public uint RecvTimeoutInMs
        {
            get { return _RecvTimeoutInMs; }
            set { _RecvTimeoutInMs = value; }
        }
        public bool Open(uint port)
        {
            bool res = ComPort.OpenComPort(port, _Baudrate);
            if (res)
            {
                this.port = (int)port;
                return true;
            }
            else
            {
                this.port = -1;
                return false;
            }
        }
        public void Close()
        {
            if (port >= 0)
                ComPort.CloseComPort((uint)port);
        }
        public bool Send(byte[] data)
        {
            if (port >= 0)
                return ComPort.Send((uint)port, data, (uint)data.Length);
            return false;
        }
        public bool Send(string s)
        {
            byte[] data = System.Text.Encoding.ASCII.GetBytes(s);
            return Send(data);
        }
        public byte[] Recive(uint size)
        {
            if (port >= 0)
            {
                byte[] res = new byte[size];
                int n = ComPort.Recive((uint)port, res, (uint)res.Length, _RecvTimeoutInMs);
                if(n>0) 
                    System.Diagnostics.Trace.WriteLine("Serial GetData : " + Program.bin_to_asc(res, n));
                if (n == size)
                    return res;
                else if (n > 0)
                {
                    byte[] ss = new byte[n];
                    for (int i = 0; i < n; i++)
                        ss[i] = res[i];
                    return ss;
                }
            }
            return null;
        }
        public byte[] Recive(uint size,uint timeout)
        {
            if (port >= 0)
            {
                byte[] res = new byte[size];
                int n = ComPort.Recive((uint)port, res, (uint)res.Length, timeout);
                if (n > 0)
                    System.Diagnostics.Trace.WriteLine("Serial GetData : " + Program.bin_to_asc(res, n));
                if (n == size)
                    return res;
                else if (n > 0)
                {
                    byte[] ss = new byte[n];
                    for (int i = 0; i < n; i++)
                        ss[i] = res[i];
                    return ss;
                }
            }
            return null;
        }
        public string ReciveString(uint size)
        {
            byte[] res = Recive(size,500);
            if (res == null)
                return "";
            return System.Text.Encoding.Default.GetString(res);
        }
        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static bool OpenComPort(uint port, uint baudrate);
        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static void CloseComPort(uint port);
        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static bool Send(uint port,byte[] data, uint len);
        static long last_gets = 0;
        internal string gets()
        {
            string s = "";
            for (;;)
            {
                string ts = ReciveString(1);
                s += ts;
                if (s.Length>0 &&(ts=="" || s.EndsWith("\r\n")))
                {
                    if (s.StartsWith("DEBUG_INFO"))
                    {
                        ConsoleColor tmp = Console.ForegroundColor;
                        Console.ForegroundColor = ConsoleColor.Red;
                    }
                    else
                    {
                        ConsoleColor tmp = Console.ForegroundColor;
                        Console.ForegroundColor = ConsoleColor.Yellow;
                    }

                    if (ShowData)
                        Program.Write("[TX->]  " + s);
                    
                    //Console.ForegroundColor = tmp;
                    log.log("[TX->] " + s);

                    return s;
                }
            }
        }

        internal void puts(string v)
        {
            ConsoleColor tmp = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Green;
            //Console.ForegroundColor = tmp;
            Send(v + "\r\n");

            if (ShowData)
                Program.Write("[RX<-] "+v + "\r\n");
            
            log.log("[RX<-] " + v);
        }

        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static int Recive(uint port, byte[] data, uint len, uint timeout);
    }
    class hxSerialProtocol : Object
    {
        public enum DialogProtocolType { SIMPLE,UNIVERSAL};
        private int port = -1;
        private uint _ReciveLength = 0;
        private DialogProtocolType _DialogProcoto = 0;
        private uint _RecvTimeoutInMs = 50;
        private uint _RetryTimes = 3;
        private uint _Baudrate = 115200;
        public uint ReciveLength
        {
            get { return _ReciveLength; }
            set { _ReciveLength = value; }
        }
        public uint Baudrate
        {
            get { return _Baudrate; }
            set { _Baudrate = value; }
        }
        public DialogProtocolType DialogProcoto
        {
            get { return _DialogProcoto; }
            set { _DialogProcoto = value; }
        }
        public uint RetryTimes
        {
            get { return _RetryTimes; }
            set { _RetryTimes = value; }
        }
        public uint RecvTimeoutInMs
        {
            get { return _RecvTimeoutInMs; }
            set { _RecvTimeoutInMs = value; }
        }
        public string  ErrorInfo
        {
            get
            {
                if (port >= 0)
                {
                    IntPtr p = getErrorInfo((uint)port);
                    return Marshal.PtrToStringAnsi(p);
                }
                return "port not opened!";
            }
        }
        public bool Open(uint port)
        {
            bool res = hxSerialProtocol.OpenSerial(port, 
                Baudrate, 
                (_DialogProcoto == DialogProtocolType.UNIVERSAL ? true : false));
            if (res)
            {
                this.port = (int)port;
                return true;
            }
            else
            {
                this.port = -1;
                return false;
            }
        }
        public void Close()
        {
            if(port>=0)
                hxSerialProtocol.CloseSerial((uint)port);
        }
        public byte[] Dialog(byte[] message)
        {
            if (port >= 0)
            {
                if (_ReciveLength == 0)
                    _ReciveLength = (uint)message.Length;
                byte[] recv = new byte[_ReciveLength];
                bool res = hxSerialProtocol.DoDialog(
                    (uint)port, 
                    (uint)_RecvTimeoutInMs,
                    (_DialogProcoto == DialogProtocolType.UNIVERSAL?true:false), 
                    (uint)_RetryTimes, 
                    message, 
                    (uint)message.Length,
                    recv,
                    (uint)recv.Length);
                if (res)
                    return recv;
                else
                    return null;
            }
            else
            {
                return null;
            }
        }
        public string Dialog(string s, Encoding encoding)
        {
            byte[] bs = encoding.GetBytes(s);
            bs = Dialog(bs);
            if (bs == null)
                return null;
            return encoding.GetString(bs);
        }
        public string Dialog(string s)
        {
            return Dialog(s, Encoding.UTF8);
        }
        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static bool OpenSerial(uint port, uint baudrate, bool use_uniproto);

        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static void CloseSerial(uint port);

        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static bool DoDialog(
            uint port,              //端口号
            uint timeout_in_ms,     //接收超时
            bool use_uniproto,      //使用协议
            uint retry_times,		//重试次数
            byte[] data_send,       //发送数据
            uint send_len,          //发送长度
            byte[] data_recv,       //接收缓冲
            uint recv_len);         //接收长度
        [DllImport("hxSerialProtocol.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static IntPtr getErrorInfo(uint port);
    }
}
