set RM=linux_tools\rm -rf

%RM% fix_name\*.db
%RM% fix_name\*.bin
%RM% fix_name\fix_name\*.bin
%RM% fix_name\Debug
%RM% fix_name\fix_name\Debug

%RM% fix_name_v2\*.db
%RM% fix_name_v2\*.bin
%RM% fix_name_v2\fix_name\*.bin
%RM% fix_name_v2\Debug
%RM% fix_name_v2\fix_name\Debug

%RM% jlink_dcc_view\*.db
%RM% jlink_dcc_view\Debug\*.ilk
%RM% jlink_dcc_view\Debug\*.pdb
%RM% jlink_dcc_view\jlink_dcc\Debug\*
%RM% jlink_dcc_view\Release\*.iobj
%RM% jlink_dcc_view\Release\*.ipdb
%RM% jlink_dcc_view\Release\*.pdb
%RM% jlink_dcc_view\Release\*.txt
%RM% jlink_dcc_view\Debug\*.txt

%RM% jlink_udt_view\*.db
%RM% jlink_udt_view\Debug\*.ilk
%RM% jlink_udt_view\Debug\*.pdb
%RM% jlink_udt_view\jlink_dcc\Debug\*
%RM% jlink_udt_view\Release\*.iobj
%RM% jlink_udt_view\Release\*.ipdb
%RM% jlink_udt_view\Release\*.pdb
%RM% jlink_udt_view\Release\*.txt
%RM% jlink_udt_view\Debug\*.txt


%RM% wcdma_m_emu/ipch
%RM% wcdma_m_emu/*.db
%RM% wcdma_m_emu/out/Log*
%RM% wcdma_m_emu/out/*.txt
%RM% wcdma_m_emu/out/*.lib
%RM% wcdma_m_emu/out/*.pdb
%RM% wcdma_m_emu/out/*.exp

%RM% wcdma_m_emu/hxTestTool/obj
%RM% wcdma_m_emu/hxTestTool/bin

%RM% wcdma_m_emu/hxSerialProtocol/Debug

%RM% tts2wav/tts2wav/*.db
%RM% tts2wav/tts2wav/obj/Debug



pause