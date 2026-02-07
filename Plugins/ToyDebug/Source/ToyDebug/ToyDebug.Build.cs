using System.IO;
using UnrealBuildTool;

public class ToyDebug : ModuleRules
{
    public ToyDebug(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
    }
}
