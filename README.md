## Can't load the project?
To load the proj you should to rename **A_Game/A_Game.vcxproj.example** to **A_Game/A_Game.vcxproj** and change your toolset if needed.
For windows7 users, you may have to configure your own DirectX11 SDK paths in 
*project->property->VC++Directories->IncludeDirectories*   
and   
*project->property->VC++Directories->LibraryDirectories*  

## 加载项目失败?

我把vcxproj放到忽略规则里了, 这样他就不会被上传, 然后第一次直接同步的项目就不能加载.  

所以需要把**A_Game/A_Game.vcxproj.example**重命名成**A_Game/A_Game.vcxproj** 然后就能加载了, 之后还要根据你自己的系统配置修改工具集, 修改SDK路径... 这些会对vcxproj造成修改的操作现在只会存在于你的本地而不会上传.