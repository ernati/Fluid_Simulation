
using UnrealBuildTool;
using System.IO;

public class Constant_Acceleration_Simulation_2D : ModuleRules
{
    public Constant_Acceleration_Simulation_2D(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput","Eigen" });

    }
}