using hxTestTool;

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

        public string rem_使用固定IP = "如果当前项目被设置,将不会检查at指令中所指定的ip";
        public string 使用固定IP = "192.168.200.254";
        public string 使用固定端口 = "9020";

        public string rem_AT指令扩展匹配字符串 = "当前项目用于扩展AT指令,格式为 cmd|res|...";
        public string AT指令扩展匹配字符串 = "AT|OK|ATE|OK";

        public string rem_AT指令扩展起始匹配字符串 = "当前项目用于扩展AT指令,格式为 cmd|res|...";
        public string AT指令扩展起始指令匹配字符串 = "AT+IPMODE|OK";

        public string rem_透传建立AT指令匹配起始 = "当收到改命令,将建立透传并使用固定ip,格式为 cmd|res";
        public string 透传建立AT指令匹配起始 = "";

    }

    public string rem_应用参数
        = "======================================================\n"
        + "应用程序设定参数.";
    public Section_应用参数 应用参数 = new Section_应用参数();

}