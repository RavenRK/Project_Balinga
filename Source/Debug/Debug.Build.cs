using UnrealBuildTool;

public class Debug : ModuleRules
{
    public Debug(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
    }
}
