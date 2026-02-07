using UnrealBuildTool;

public class ToyMaths: ModuleRules
{
    public ToyMaths(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});
    }
}
