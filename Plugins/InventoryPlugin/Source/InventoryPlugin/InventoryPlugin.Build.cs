// Some copyright should be here...
using System.IO;
using UnrealBuildTool;

public class InventoryPlugin : ModuleRules
{
    private string PluginsPath
    {
        get { return Path.GetFullPath(BuildConfiguration.RelativeEnginePath) + "Plugins/Runtime/"; }
    }

    public InventoryPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //bEnforceIWYU = true;

        PublicIncludePaths.AddRange(
			new string[] {
				"InventoryPlugin/Public",
			}
			);

        PrivateIncludePaths.AddRange(
			new string[] {
				"InventoryPlugin/Private",
			}
			);

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
			"Core",
			"CoreUObject",
			"Engine",
			"PhysX",
			"HeadMountedDisplay",
			"UMG",
        });


        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
                "InputCore",
                "RHI",
                "RenderCore",
                "ShaderCore",
                "NetworkReplayStreaming",
                "AIModule",
                "UMG",
                "GameplayTags"
            });



        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore"
			}
			);
    }
}
