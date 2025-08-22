using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace hxTestTool
{
    public class INIConfig
    {
        List<string> lines = new List<string>();
        FileStream fs = null;
        /// <summary>
        /// 加载初始化文件
        /// </summary>
        /// <param name="conf_file"></param>
        public bool LoadIniFile(string conf_file)
        {
            if (fs != null)
                return false;
            fs = new FileStream(conf_file, FileMode.OpenOrCreate);
            long len = fs.Length;
            if (len < 0 || len > 4096000)
                return false;
            if (len == 0)
            {
                lines = new List<string>();
                lines.Clear();
            }
            else
            {
                byte[] buffer = new byte[len];
                fs.Read(buffer, 0, (int)len);
                string sbuff = System.Text.Encoding.Default.GetString(buffer);
                lines = sbuff.Split(new char[] { '\n' }, StringSplitOptions.None).ToList();
            }

            LoadValues(this,"");
            return true;
        }

        private void LoadValues(object cls,string name)
        {
            Type t = cls.GetType();
            FieldInfo[] members = t.GetFields();
            foreach (FieldInfo fi in members)
            {
                string key = fi.Name;
                if (key.StartsWith("rem_"))
                    continue;
                object value = fi.GetValue(cls);
                Type tt = value.GetType();
                if (tt == typeof(string))
                {
                    fi.SetValue(cls, LaodItem(name, key, (string)value));
                }
                else if (tt == typeof(int))
                {
                    fi.SetValue(cls, LaodItem(name, key, (int)value));
                }
                else if (tt == typeof(double))
                {
                    fi.SetValue(cls, LaodItem(name, key, (double)value));
                }
                else if (tt == typeof(bool))
                {
                    fi.SetValue(cls, LaodItem(name, key, (bool)value));
                }
                else
                {
                    if (name==null || name=="")
                    {
                        LoadValues(value,key);
                    }
                }
            }
        }

        private string LaodItem(string section, string key, string def)
        {
            //key = key.TrimStart().Split(new char[] { '\r', '\n', '#', })[0];
            def = def.TrimStart().Split(new char[] {'\r','\n','#', })[0];
            int pos = 0;
            if (section != null && section != "")
            {
                for (pos = 0; pos < lines.Count; pos++)
                {
                    string s = lines[pos];
                    string ss = s.TrimStart();
                    if (ss.StartsWith("#"))
                        continue;
                    if (ss.StartsWith("["))
                    {
                        string sec = ss.Split(new char[] { '[', ']' })[1].Trim();
                        if (sec == section)
                        {
                            ++pos;
                            goto find_value;
                        }
                    }
                }

                //添加 section
                lines.Insert(pos++,"");
                pos = InsertRem(pos,section,"");
                lines.Insert(pos++,"[" + section + "]");
            }
        find_value:
            for (; pos < lines.Count; pos++)
            {
                string s = lines[pos];
                string ss = s.TrimStart();
                if (ss.StartsWith("#"))
                    continue;
                if (ss.StartsWith("["))
                    break;
                string[] sss = ss.Split('=');
                string k = sss[0].TrimEnd();
                if (k == key)
                {
                    string v = sss[1].Split('#')[0].Trim();
                    return v;
                }
            }
            pos = InsertRem(pos,key,section);
            lines.Insert(pos++,key + " = " + def);
            return def;
        }

        private int InsertRem(int pos, string key, string section)
        {
            if (pos < 0)
                pos = 0;
            string rem = "rem_" + key;
            FieldInfo fi;
            object obj;
            if (section == null || section == "")
            {
                obj = this;
                fi = obj.GetType().GetField(rem);
            }
            else
            {
                fi = this.GetType().GetField(section);
                obj = fi.GetValue(this);
                fi = obj.GetType().GetField(rem);
            }
            
            
            if (fi != null)
            {
                string rem_v = (string)fi.GetValue(obj);
                string[] ss = rem_v.Split('\n');
                foreach (string s in ss)
                {
                    lines.Insert(pos++, "# " + s.TrimEnd() + "\r\n");
                }
            }
            return pos;          
        }

        private int LaodItem(string section, string key, int def)
        {
            return int.Parse(LaodItem(section, key, def.ToString()));
        }
        private double LaodItem(string section, string key, double def)
        {
            return double.Parse(LaodItem(section, key, def.ToString()));
        }
        private bool LaodItem(string section, string key, bool def)
        {
            return bool.Parse(LaodItem(section, key, def.ToString()));
        }

        /// <summary>
        /// 析构时自动存储
        /// </summary>
        public void SaveIniFile()
        {
            string ss = "";
            foreach (string s in lines)
            {
                ss += s.TrimEnd() + "\r\n";
            }
            byte[] buffer = System.Text.Encoding.Default.GetBytes(ss);
            fs.Seek(0, SeekOrigin.Begin);
            fs.Write(buffer, 0, buffer.Length);
            fs.Flush();
        }
    }
}
