    using UnrealBuildTool;

public class ToyPhysics: ModuleRules
{
    public ToyPhysics(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});
    }
}
