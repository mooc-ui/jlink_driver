    public class Config : INIConfig
    {
        public string rem_�����ļ��汾
            = "˵��: \n"
            + "1. ɾ���������ļ���Ӧ�ó�����Զ���������һ���µ�,������ÿ��������ΪĬ�ϲ���\n"
            + "2. �����ļ����ж�ȡ,ÿһ��һ��������\n"
            + "3. ����\"#\"֮��ֱ��һ��ĩβΪע��,��Ӧ�ó������κ�Ӱ��.\n";
        public string �����ļ��汾 = "0.1";
        public class Section_Ӧ�ò���
        {
            public string rem_��־�洢·�� = "������־�Ĵ洢·��,ʹ�þ���·��";
            public string ��־�洢·�� = "";
        }
        public class Section_���Բ���
        {
            public string rem_���ò��Ե�Ԫ = "�ö��ŷָ������,�������A1��B2: A1,B2";
            public string ���ò��Ե�Ԫ = "";
        }
        public string rem_Ӧ�ò���
            = "======================================================\n"
            + "Ӧ�ó����趨����.";
        public Section_Ӧ�ò��� Ӧ�ò��� = new Section_Ӧ�ò���();
        public string rem_���Բ���
            = "======================================================\n"
            + "���Ե�Ԫ�趨����.";
        public Section_���Բ��� ���Բ��� = new Section_���Բ���();

    }