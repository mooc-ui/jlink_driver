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
        
        public string 用户自编号 = "00000001";
        public string 接入点 = "cmnet";
        public string 用户名 = "";
        public string 密码 = "";
        public string 服务器IP = "180.89.58.27";
        public int 服务器端口 = 9020;

    }

    public string rem_应用参数
        = "======================================================\n"
        + "应用程序设定参数.";
    public Section_应用参数 应用参数 = new Section_应用参数();

}