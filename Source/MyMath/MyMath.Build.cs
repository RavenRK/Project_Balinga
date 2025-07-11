using UnrealBuildTool;

public class MyMath: ModuleRules
{
    public MyMath(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});
    }
}
