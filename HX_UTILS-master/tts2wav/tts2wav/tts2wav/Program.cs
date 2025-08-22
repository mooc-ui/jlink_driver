using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using DotNetSpeech;
using System.Threading;

using HxHtils;

namespace tts2wav
{
    public class Config : INIConfig
    {
        public string rem_配置文件版本
            = "说明: \n"
            + "1. 删除该配置文件后应用程序会自动重新生成一个新的,并设置每个配置项为默认参数\n"
            + "2. 配置文件按行读取,每一行一个配置项\n"
            + "3. 符号\"#\"之后直到一行末尾为注释,对应用程序无任何影响.\n";
        public string 配置文件版本 = "0.1";

        public class Section_应用参数
        {
            public string rem_语速 = "范围 [-10,10]";
            public int 语速 = 0;
            public string rem_音量 = "范围 [0,100]";
            public int 音量 = 100;
            public string rem_TTS库 = "参考 控制面板/语音识别/TTS列表设置";
            public string TTS库 = "Microsoft Simplified Chinese";
            public string rem_文本 = "需要转换的文本";
            public string 文本 = "老人卡,学生卡";
            public string rem_分隔字符串 = "文本将被改字符串分隔输出多个独立文件";
            public string 分隔字符串 = ",";

        }
        
        public string rem_应用参数
            = "======================================================\n"
            + "应用程序设定参数.";
        public Section_应用参数 应用参数 = new Section_应用参数();

    }
    class Program
    {
        static void Main(string[] args)
        {
            string conf_file = AppDomain.CurrentDomain.BaseDirectory + "config.ini";
            Config cfg = new Config();
            cfg.LoadIniFile(conf_file);
            cfg.SaveIniFile();
            

            //SpeechVoiceSpeakFlags spFlags = SpeechVoiceSpeakFlags.SVSFlagsAsync;
            SpVoice voice = new SpVoice();
            voice.Rate = cfg.应用参数.语速; //语速,[-10,10]
            voice.Volume = cfg.应用参数.音量; //音量,[0,100]

            string tts = cfg.应用参数.TTS库;// "Microsoft Lili";// 

            // string tts = "MS-Lili-2052-20-DSK";// 

             voice.Voice = voice.GetVoices("name="+tts/*Microsoft Simplified Chinese"*/).Item(0);

            //dynamic vs = voice.GetVoices();
            //System.Console.WriteLine("TTS list:");
            //for (int i = 0; i < vs.Count; i++)
            //{
            //    string s = vs[i].id;
            //    System.Console.WriteLine("" + (i + 1) + ":" + s);
            //}
            //next:
            //System.Console.WriteLine("Please Select:");
            //string ins = System.Console.ReadLine();
            //int sel = int.Parse(ins);
            //if (sel < 0 || sel > vs.Count)
            //    goto next;

            //voice.Voice = voice.GetVoices("name=XiaoKun").Item(0);
            /*
             dynamic ttss = spVoice.GetVoices();//‘获得语音引擎集合
             ttss.count
             ttss.id
             */

            string st = cfg.应用参数.文本;
            string sp = cfg.应用参数.分隔字符串;
            string[] ss = st.Split(new string[] { sp }, StringSplitOptions.None);
            foreach (string s in ss)
            {
                string outname;
                string text;
                if (s.Contains('%'))
                {
                    string[] sst = s.Split('%');
                    outname = sst[0];
                    text = sst[1];
                }
                else
                {
                    outname = s;
                    text = s;
                }
                //SpFileStream spfs = new SpFileStream();
                string path = AppDomain.CurrentDomain.BaseDirectory  + outname +".wav";
                System.Console.WriteLine("Output: " + path);
                //spfs.Open(path, SpeechStreamFileMode.SSFMCreateForWrite, false);
                // spfs.Write(voice.)

                Type type2 = Type.GetTypeFromProgID("SAPI.SpFileStream");
                dynamic objFileStream = Activator.CreateInstance(type2);

                objFileStream.Format.Type = SpeechAudioFormatType.SAFT22kHz16BitMono;
                objFileStream.Open(path, SpeechStreamFileMode.SSFMCreateForWrite, false);
                voice.AudioOutputStream = objFileStream;
                voice.Speak(text, SpeechVoiceSpeakFlags.SVSFlagsAsync);
                voice.WaitUntilDone(-1);
                objFileStream.Close();
                //voice.Speak("老人卡,学生卡,当");// How do you do");
            }
            System.Console.WriteLine("All Convert Ok, Press Any key to exit.");
            System.Console.ReadKey();

        }
    }
}
