using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace hxTestTool
{

    class Program
    {

        static ComPort ser = null;
        static Socket sock = null;
        static Config config = new Config();
        static bool FixRemoteIP = config.应用参数.使用固定IP.Trim() == "" ? false : true;
        static string rm_ip_fix = config.应用参数.使用固定IP;
        static bool _serial_info = true;
        static bool _pack_head_info = true;
        static Log log = new Log(AppDomain.CurrentDomain.BaseDirectory + @"Log_Packet_" + DateTime.Now.ToShortDateString() + ".txt");
        static Log alllog = new Log(AppDomain.CurrentDomain.BaseDirectory + @"Log_All_Info_" + DateTime.Now.ToShortDateString() + ".txt", false);

        enum NicType
        {
            Direct,     //透传模式  GPRS 4G WIFI
            AT,         //AT+SEND模式  WCDMA
            CMD,        //CMD模式  LAN WIFI
        }
        static NicType TransMode = NicType.Direct;     //默认为AT

        static void Main(string[] args)
        {
            foreach(string sss in args) {
                if(sss == "all") {
                    _serial_info = false;
                    _pack_head_info = false;
                }
                else if(sss == "ser") {
                    _serial_info = false;
                }
                else if(sss == "packet") {
                    _pack_head_info = false;
                }
            }
            EnsureOnlyOneInstance();
            string conf_file = AppDomain.CurrentDomain.BaseDirectory + "config.ini";
            config.LoadIniFile(conf_file);
            config.SaveIniFile();
           

            string fileName = (args.Length > 0) ? args[0] : Assembly.GetExecutingAssembly().Location;
            string buildtime = System.IO.File.GetLastWriteTime(fileName).ToString();
            string version = "V0.4.0";
            string app_title = "HX Net Module (Universal) Emulate " + version + " build [" + buildtime + "]";
            ConsoleLogWriteLine(app_title);
            ConsoleLogWriteLine("==========================================================");

            FixRemoteIP = config.应用参数.使用固定IP.Trim() == "" ? false : true;
            if(FixRemoteIP) {
                Console.WriteLine("使用固定IP:" + config.应用参数.使用固定IP);
                rm_ip_fix = config.应用参数.使用固定IP;
            }
            else {
                Console.WriteLine("使用命令指定的IP, 如果没有指定便使用默认的:" + config.应用参数.使用固定IP);
            }

            //---------------------------------------------------------------------
            ser = new ComPort();
            ser.RecvTimeoutInMs = 1000;
            ConsoleLogWriteLine("Communication Configs:");
            //ConsoleLogWriteLine("Protocol : " + ser.DialogProcoto);
            // (ser.DialogProcoto == hxSerialProtocol.DialogProtocolType.SIMPLE?"SIMPLE":"UNIVERSAL") );
            ConsoleLogWriteLine("Baudrate : " + ser.Baudrate);
            ConsoleLogWriteLine("Recive Timeout : " + ser.RecvTimeoutInMs + "ms");
            //ConsoleLogWriteLine("Retry Times : " + ser.RetryTimes);

            ConsoleLogWriteLine("Alive Serial Ports:");
            List<uint> ap = new List<uint>();
            for(uint i = 0; i < 40; i++) {
                if(ser.Open(i)) {
                    ser.Close();
                    ConsoleLogWriteLine("COM" + i);
                    ap.Add(i);
                }
            }
        cfg_port_nr:
            ConsoleLogWrite("Please Select A Port :");
            string s = Console.ReadLine();
            uint n;
            try { n = uint.Parse(s); }
            catch {
                ConsoleLogWriteLine("input error.");
                goto cfg_port_nr;
            }
            if(!ap.Contains(n)) {
                ConsoleLogWriteLine("unalivable port.");
                goto cfg_port_nr;
            }
            if(!ser.Open(n)) {
                ConsoleLogWriteLine("cannot open port.");
                goto cfg_port_nr;
            }
            ser.ShowData = _serial_info;
            ser.puts(app_title);
            try {
            reset:
                ConsoleLogWriteLine("=====================================================================");
                ConsoleLogWriteLine("->Net Module Reset");
                Thread.Sleep(1000);
                ser.puts("+ZEND");

                for(;;) {
                    s = ser.gets();
                    s = s.Trim();
                    if(s.StartsWith("AT")) {
                        bool dellf = false;
                        {
                            try {
                                string ss1 = config.应用参数.AT指令扩展匹配字符串;
                                if(ss1.Trim() == "")
                                    throw new System.Exception();
                                string[] sss1 = ss1.Split(new char[] { '|' });
                                if(sss1.Length < 2)
                                    throw new System.Exception();
                                for(int i = 0; i < sss1.Length; i += 2) {
                                    if(s == sss1[i]) {
                                        ser.puts(sss1[i + 1]);
                                        dellf = true;
                                    }
                                }
                            }
                            catch {
                            }

                            try {
                                string ss1 = config.应用参数.AT指令扩展起始指令匹配字符串;
                                if(ss1.Trim() == "")
                                    throw new System.Exception();
                                string[] sss1 = ss1.Split(new char[] { '|' });
                                if(sss1.Length < 2)
                                    throw new System.Exception();
                                for(int i = 0; i < sss1.Length; i += 2) {
                                    if(s.StartsWith(sss1[i])) {
                                        dellf = true;
                                        ser.puts(sss1[i + 1]);
                                    }
                                }
                            }
                            catch { }

                            try {
                                string ss1 = config.应用参数.透传建立AT指令匹配起始;
                                if(ss1.Trim() == "")
                                    throw new System.Exception();
                                string[] sss1 = ss1.Split(new char[] { '|' });
                                if(sss1.Length < 1)
                                    throw new System.Exception();
                                if(s.StartsWith(sss1[0])) {
                                    IPAddress ip = IPAddress.Parse(rm_ip_fix);
                                    string port = config.应用参数.使用固定端口;
                                    try {
                                        sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                                        sock.Connect(new IPEndPoint(ip, int.Parse(port)));
                                        ser.puts("CONNECT");
                                        sock.BeginReceive(new byte[2048], 0, 2048, SocketFlags.None, new AsyncCallback(sock_recv_directly), ser);
                                        List<byte> buf = new List<byte>();
                                        List<byte> buf9 = new List<byte>();
                                        buf.Clear();
                                        dellf = true;
                                        int tickcount = 0;
                                        for(;;) {
                                            try {
                                                //byte[] bs = ser.Recive(1024,100);
                                                //sock.Send(bs);
                                                //WriteLine("Dev->Server : (" + bs.Length + ") " + bin_to_asc(bs, bs.Length));
                                                //log.log("TX PACKET : " + bs.Length + " " + bin_to_asc(bs, bs.Length));

                                                byte[] bs = ser.Recive(1);
                                                if(bs != null && bs.Length == 1) {
                                                    byte b0 = bs[0];
                                                    if(b0 == '@') {
                                                        tickcount = System.Environment.TickCount;
                                                        buf9.Clear();
                                                        buf.Clear();
                                                        buf.Add(b0);
                                                    }
                                                    else if(bs[0] == '#') {
                                                        buf.Add(b0);
                                                        if(sock != null) {
                                                            int nsend = sock.Send(buf.ToArray());
                                                            WriteLine("Dev->Server : (" + buf.Count + ") " + bin_to_asc(buf.ToArray(), buf.Count));
                                                            log.log("TX PACKET : " + buf.Count + " " + bin_to_asc(buf.ToArray(), buf.Count));
                                                        }
                                                        else {
                                                            ser.puts("ERROR");
                                                        }
                                                        buf.Clear();
                                                    }
                                                    else {
                                                        if(System.Environment.TickCount - tickcount > 500) {
                                                            buf9.Add(b0);
                                                        }
                                                        if(buf9.Count > 2) {
                                                            string sss = System.Text.Encoding.ASCII.GetString(buf9.ToArray());
                                                            string[] sas = sss.Split(new string[] { "AT" },StringSplitOptions.None);
                                                            string[] sas2 = sss.Split(new string[] { "\n" }, StringSplitOptions.None);
                                                            if(sas.Length > 3 && sas2.Length > 3) {
                                                                try {
                                                                    sock.Shutdown(SocketShutdown.Both);
                                                                    sock.Close();
                                                                }
                                                                catch { }
                                                                goto reset;
                                                            }
                                                        }
                                                        if(buf.Count > 4096)
                                                            buf.Clear();
                                                        buf.Add(b0);
                                                    }
                                                }
                                            }
                                            catch {
                                                ; ; ;
                                            }
                                        }

                                    }
                                    catch {
                                    }
                                }
                                ser.puts(sss1[1]);
                            }


                            catch { }
                        }

                        if(dellf == false) {

                            if(s == "AT") {
                                ser.puts("AT");
                                ser.puts("OK");
                            }
                            else if(s == "ATE0") {
                                ser.puts("ATE0");
                                ser.puts("OK");
                            }
                            else if(s == "ATE1") {
                                ser.puts("OK");
                            }
                            else if(s == "AT+CSQ?") {
                                ser.puts("OK");
                                ser.puts("+CSQ: 29,99");
                            }
                            else if(s == "AT+CREG?") {
                                ser.puts("OK");
                                ser.puts("+REG: 1,1");
                            }
                            else if(s == "AT+CIPMODE=1") {
                                ser.puts("OK");
                            }
                            else if(s == "AT+CIPSHUT") {
                                ser.puts("SHUT OK");
                                ser.puts("OK");
                            }
                            else if(s.StartsWith("AT+CIPCSGP")) {
                                ser.puts("OK");
                            }
                            else if(s.StartsWith("AT+CGSN")) {
                                ser.puts("012345678901234");
                            }
                            else if(s.StartsWith("AT+CIMI")) {
                                ser.puts("112233445566778");
                            }
                            else if(s.StartsWith("AT+ZIPCFG=")) {
                                ser.puts("OK");
                            }
                            else if(s.StartsWith("AT+ZIPCALL=1")) {
                                ser.puts("OK");
                                ser.puts("+ZIPCALL: 1,99.88.99.88");
                            }
                            else if(s.StartsWith("AT+ZIPSTAT=")) {
                                int state = sock == null ? 0 : 1;
                                ser.puts("+ZIPSTAT: 1," + state);
                            }
                            else if(s.StartsWith("AT+CIPSTART")) {
                                // "AT+CIPSTART="TCP","119.75.218.70","80"
                                try {
                                    string v = s.Split('=')[1];
                                    string[] ss = v.Split(',');
                                    string ip_addr = ss[1].Trim(new char[] { '\"' });
                                    string port = ss[2].Trim(new char[] { '\"' });
                                    if(ip_addr != null && port != null) {
                                        ser.puts("OK");

                                        IPAddress ip;
                                        if(FixRemoteIP)
                                            ip = IPAddress.Parse(rm_ip_fix);
                                        else
                                            ip = IPAddress.Parse(ip_addr);
                                        try {
                                            sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                                            sock.Connect(new IPEndPoint(ip, int.Parse(port)));
                                            //byte[] bbb = new byte[1024];
                                            //int nnn = sock.Receive(bbb);
                                            ser.puts("CONNECT");
                                            sock.BeginReceive(new byte[2048], 0, 2048, SocketFlags.None, new AsyncCallback(sock_recv_directly), ser);
                                            List<byte> buf = new List<byte>();
                                            List<byte> buf9 = new List<byte>();
                                            buf.Clear();
                                            int tickcount = 0;
                                            for(;;) {
                                                try {
                                                    //byte[] bs = ser.Recive(1024,100);
                                                    //sock.Send(bs);
                                                    //WriteLine("Dev->Server : (" + bs.Length + ") " + bin_to_asc(bs, bs.Length));
                                                    //log.log("TX PACKET : " + bs.Length + " " + bin_to_asc(bs, bs.Length));

                                                    byte[] bs = ser.Recive(1);
                                                    if(bs != null && bs.Length == 1) {
                                                        byte b0 = bs[0];
                                                        if(b0 == '@') {
                                                            tickcount = System.Environment.TickCount;
                                                            buf9.Clear();
                                                            buf.Clear();
                                                            buf.Add(b0);
                                                        }
                                                        else if(bs[0] == '#') {
                                                            buf.Add(b0);
                                                            if(sock != null) {
                                                                int nsend = sock.Send(buf.ToArray());
                                                                WriteLine("Dev->Server : (" + buf.Count + ") " + bin_to_asc(buf.ToArray(), buf.Count));
                                                                log.log("TX PACKET : " + buf.Count + " " + bin_to_asc(buf.ToArray(), buf.Count));
                                                            }
                                                            else {
                                                                ser.puts("ERROR");
                                                            }
                                                            buf.Clear();
                                                        }
                                                        else {
                                                            if(System.Environment.TickCount - tickcount > 500) {
                                                                buf9.Add(b0);
                                                            }
                                                            if(buf9.Count >= 3) {
                                                                if(buf9[0] == '+'
                                                                        && buf9[1] == '+'
                                                                        && buf9[2] == '+') {
                                                                        sock.Shutdown(SocketShutdown.Both);
                                                                        sock.Close();
                                                                    ser.puts("OK");
                                                                    WriteLine("+++ DISCONNECT");
                                                                    log.log("+++ DISCONNECT");
                                                                    goto reset;
                                                                }
                                                                
                                                            }
                                                            if(buf9.Count > 2) {
                                                                string sss = System.Text.Encoding.ASCII.GetString(buf9.ToArray());
                                                                string[] sas = sss.Split(new string[] { "AT" }, StringSplitOptions.None);
                                                                string[] sas2 = sss.Split(new string[] { "\n" }, StringSplitOptions.None);
                                                                if(sas.Length > 3 && sas2.Length>3) {
                                                                    try {
                                                                        sock.Shutdown(SocketShutdown.Both);
                                                                        sock.Close();
                                                                    }
                                                                    catch { }
                                                                    goto reset;
                                                                }
                                                            }
                                                            if(buf.Count > 4096)
                                                                buf.Clear();
                                                            buf.Add(b0);
                                                        }
                                                    }
                                                }
                                                catch {
                                                    ; ; ;
                                                }
                                            }

                                        }
                                        catch {
                                        }
                                    }
                                }

                                catch {

                                }
                                ser.puts("ERROR");
                            }
                            else if(s.StartsWith("AT+ZIPSEND=")) {
                                string v = s.Split('=')[1];
                                string[] ss = v.Split(',');
                                string sock_id = ss[0];
                                string data = ss[1];
                                byte[] bin_data = asc_to_bin(data);
                                AnalysPacket(1, data);
                                ser.puts("OK");
                                if(sock != null) {
                                    int nsend = sock.Send(bin_data);
                                    ser.puts("+ZIPSEND:1," + nsend);
                                    log.log("TX PACKET : " + data);
                                }
                                else {
                                    ser.puts("ERROR");
                                }
                            }
                            else if(s.StartsWith("AT+ZIPOPEN=")) {
                                string v = s.Split('=')[1];
                                string[] ss = v.Split(',');
                                string sock_id = ss[0];
                                string tcp0_udp1 = ss[1];
                                string ip_addr = ss[2];
                                string port = ss[3];
                                string local_port = ss[4];
                                ser.puts("OK");

                                IPAddress ip;
                                if(FixRemoteIP) {
                                    port = config.应用参数.使用固定端口;
                                    ip = IPAddress.Parse(rm_ip_fix);
                                }
                                else
                                    ip = IPAddress.Parse(ip_addr);
                                try {
                                    sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                                    sock.Connect(new IPEndPoint(ip, int.Parse(port)));
                                    ser.puts("+ZIPSTAT: " + sock_id + "," + 1);

                                    sock.BeginReceive(new byte[2048], 0, 2048, SocketFlags.None, new AsyncCallback(sock_recv), ser);
                                }
                                catch {
                                    ser.puts("+ZIPSTAT: " + sock_id + "," + 0);
                                }


                            }
                            else {
                                ser.puts("***ERROR");
                            }
                        }
                    }
                    else if(s.StartsWith("TK")) {
                        if(s == "TK+RESET") {
                            ser.puts("OK");
                            goto reset;
                        }
                    }
                    else {



                    }
                }
            }
            catch {

            }

            System.Console.ReadKey();
        }
        public static void ConsoleLogWriteLine()
        {
            Console.WriteLine();
            alllog.WriteLine();
        }
        public static void ConsoleLogWriteLine(string v)
        {
            Console.WriteLine(v);
            alllog.WriteLine(v);
        }

        public static void ConsoleLogWrite(string v)
        {
            Console.Write(v);
            alllog.Write(v);
        }
        public static void ConsoleLogWrite(char v)
        {
            Console.Write(v);
            alllog.Write(v);
        }
        private static void AnalysPacket(int IsSend, string data)
        {
            try {
                ConsoleLogWriteLine(DateTime.Now.ToString());
                string s = TrimTranslation(data);
                if(s.StartsWith("40") && s.EndsWith("23")) {
                    //提取帧头
                    s = s.Substring(2, s.Length - 4);
                    string cmd = s.Substring(0, 2);
                    int pack_len = Convert.ToInt16(s.Substring(4, 2) + s.Substring(2, 2), 16);
                    if(_pack_head_info)
                        ConsoleLogWriteLine("数据帧信息: 命令字:" + s.Substring(0, 2) +
                            "  数据块长度(LE):" + s.Substring(2, 4) + "(" + pack_len + ")" +
                            "  校验字节" + s.Substring(s.Length - 2, 2));
                    //提取包头
                    s = s.Substring(6, pack_len * 2);

                    switch(cmd) {
                    case "EA":
                        if(IsSend == 1) {
                            string tmp = s.Substring(26, 8);
                            int len = Convert.ToInt32(tmp, 16);

                            string ss = "EA透传包: 设备信息:" + s.Substring(0, 10) +
                                "  时间:" + s.Substring(10, 12) +
                                "  外设类型:" + s.Substring(22, 2) +
                                "  外设ID:" + s.Substring(24, 2) +
                                "  数据长度:" + s.Substring(26, 8) + "(" + len + ")";
                            int reserve_len = s.Length - 34 - len * 2;
                            if(reserve_len <= 8 && reserve_len > 0)
                                ss += "  保留字:" + s.Substring(s.Length - reserve_len, reserve_len);
                            else
                                ss += "  保留字:无";
                            if(_pack_head_info)
                                ConsoleLogWriteLine(ss);
                            s = s.Substring(34, len * 2);
                            break;
                        }
                        else {
                            string tmp = s.Substring(16, 8);
                            int len = Convert.ToInt32(tmp, 16);

                            string ss = "EA透传包: 时间:" + s.Substring(0, 12) +
                                "  外设类型:" + s.Substring(12, 2) +
                                "  外设ID:" + s.Substring(14, 2) +
                                "  数据长度:" + s.Substring(16, 8) + "(" + len + ")";
                            int reserve_len = s.Length - 24 - len * 2;
                            if(reserve_len <= 8 && reserve_len > 0)
                                ss += "  保留字:" + s.Substring(s.Length - reserve_len, reserve_len);
                            else
                                ss += "  保留字:无";
                            if(_pack_head_info)
                                ConsoleLogWriteLine(ss);
                            s = s.Substring(24, len * 2);
                            break;
                        }
                    case "C0":
                        break;
                    case "A0":
                        break;
                    default:
                        break;
                    }

                    //打印数据
                    ConsoleLogWriteLine("包数据: [" + s + "]");

                    //ConsoleLogWrite("--------[");
                    //for (int i = 0; i < s.Length; i += 2)
                    //{
                    //    ConsoleLogWrite(((i / 2)+1).ToString("D2"));
                    //}
                    //ConsoleLogWriteLine( "]");
                }
                else {
                    ConsoleLogWriteLine("包错误,格式不为 [@(40)...#(23)]");
                }
            }
            catch {
                ConsoleLogWriteLine("解析包过程发生异常!!!");
            }
            ConsoleLogWriteLine();
        }

        private static string TrimTranslation(string data)
        {
            string res = "";
            for(int i = 0; i < data.Length; i += 2) {
                string s = data.Substring(i, 2);
                if(s == "5E") {
                    i += 2;
                    s = data.Substring(i, 2);
                    if(s == "01")
                        res += "24";// ((uint)'$').ToString("X2");
                    else if(s == "02")
                        res += "23";// ((uint)'#').ToString("X2");
                    else if(s == "03")
                        res += "40";// ((uint)'@').ToString("X2");

                    else if(s == "04")
                        res += "5E";
                    else
                        res += s;

                }
                else {
                    res += s;
                }
            }
            return res;
        }
        private static void sock_recv_directly(IAsyncResult ar)
        {
            try {
                //Socket rSocket = (Socket)ar.AsyncState;
                sock.EndReceive(ar);
                //ar.AsyncWaitHandle.Close();
                byte[] buffer = new byte[40960];
                while(true) {
                    int n = sock.Receive(buffer);
                    if(n == 0)
                        break;
                    string data = bin_to_asc(buffer, n);
                    //string remote = sock.RemoteEndPoint.ToString();
                    //string[] ss = remote.Split(':');
                    //string ip = ss[0];
                    //string port = ss[1];
                    byte[] d = buffer.Take(n).ToArray();
                    if(ser != null) {
                        ser.Send(d);
                        //ser.puts("+ZIPRECV: 1," + ip + "," + port + "," + n + "," + data);
                        AnalysPacket(0, data);
                    }


                    System.Diagnostics.Trace.WriteLine(DateTime.Now.ToString());
                    System.Diagnostics.Trace.WriteLine("RX->" + data);

                    log.log("RX PACKET : " + data);
                }
                sock.BeginReceive(new byte[40960], 0, 40960, SocketFlags.None, new AsyncCallback(sock_recv), ser);
            }
            catch (Exception e){
                Console.WriteLine(e.Message);
                try {
                    sock.BeginReceive(new byte[40960], 0, 40960, SocketFlags.None, new AsyncCallback(sock_recv), ser);
                }
                catch { }
            }
        }
        private static void sock_recv(IAsyncResult ar)
        {
            try {
                //Socket rSocket = (Socket)ar.AsyncState;
                sock.EndReceive(ar);
                byte[] buffer = new byte[40960];
                while(true) {
                    int n = sock.Receive(buffer);
                    if(n == 0)
                        break;
                    string data = bin_to_asc(buffer, n);
                    string remote = sock.RemoteEndPoint.ToString();
                    string[] ss = remote.Split(':');
                    string ip = ss[0];
                    string port = ss[1];
                    if(ser != null) {
                        ser.puts("+ZIPRECV: 1," + ip + "," + port + "," + n + "," + data);
                        AnalysPacket(0, data);
                    }


                    System.Diagnostics.Trace.WriteLine(DateTime.Now.ToString());
                    System.Diagnostics.Trace.WriteLine("RX->" + data);

                    log.log("RX PACKET : " + data);
                }
                sock.BeginReceive(new byte[40960], 0, 40960, SocketFlags.None, new AsyncCallback(sock_recv), ser);
            }
            catch {
                if(sock.Connected)
                    sock.BeginReceive(new byte[40960], 0, 40960, SocketFlags.None, new AsyncCallback(sock_recv), ser);
            }
        }

        public static string bin_to_asc(byte[] buffer, int n)
        {
            string s = "";
            for(int i = 0; i < n; i++) {
                string tmp = buffer[i].ToString("X").PadLeft(2, '0');
                s += tmp;
            }
            return s;
        }

        public static byte[] asc_to_bin(string data)
        {
            List<byte> res = new List<byte>();
            int n = data.Length / 2;
            for(int i = 0; i < n; i++) {
                string s = data.Substring(i * 2, 2);
                res.Add(byte.Parse(s, System.Globalization.NumberStyles.HexNumber));
            }
            return res.ToArray();
        }

        public static void WriteLine(string v)
        {
            ConsoleLogWriteLine(v);
            //WriteLog(v + "\r\n");
        }
        public static void Write(string v)
        {
            ConsoleLogWrite(v);
            //WriteLog(v);

        }


        public static void Write(char v)
        {
            ConsoleLogWrite(v);
            //WriteLog(v);

        }

        private static void EnsureOnlyOneInstance()
        {
            System.Diagnostics.Process current = System.Diagnostics.Process.GetCurrentProcess();
            System.Diagnostics.Process[] all_processes = System.Diagnostics.Process.GetProcesses();
            List<System.Diagnostics.Process> others = new List<System.Diagnostics.Process>();
            foreach(System.Diagnostics.Process process in all_processes) //查找相同名称的进程
            {
                if(process.Id == current.Id)  //忽略当前进程
                    continue;

                string s = "";
                try {
                    s = process.MainModule.FileName;
                }
                catch {
                    continue;
                }
                if(s == current.MainModule.FileName) {
                    others.Add(process);
                }
            }

            if(others.Count > 0) {
                Program.WriteLine(
                    "当前有至少一个" + current.ProcessName + "应用正在运行,是否关闭它们并开启行新的实例.\r\n"
                    + "y : 关闭其他" + current.ProcessName + ",启动一个新的.\r\n"
                    + "n : 退出.");
                Program.Write(">:");

                if(Console.Read() == 'y') {
                    foreach(System.Diagnostics.Process p in others) {
                        p.Kill();
                    }
                }
                else {
                    Environment.Exit(0);
                }
            }


        }

    }
}

