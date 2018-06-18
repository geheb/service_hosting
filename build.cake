var target = Argument("target", "default");
var configuration = Argument("configuration", "Release");
var solutionFile = File("./build.sln");
var buildDir = Directory("./bin");

Task("clean")
	.Does(() =>
{
	CleanDirectory(buildDir);
});

Task("build-x86")
	.Does(() =>
{
	MSBuild(solutionFile, settings => settings
		.SetConfiguration(configuration)
		.SetPlatformTarget(PlatformTarget.x86)
		.WithTarget("Rebuild")
		.SetVerbosity(Verbosity.Minimal));
});

Task("build-x64")
	.Does(() =>
{
	if (!Context.Environment.Platform.Is64Bit) return;
	MSBuild(solutionFile, settings => settings
		.SetConfiguration(configuration)
		.SetPlatformTarget(PlatformTarget.x64)
		.WithTarget("Rebuild")
		.SetVerbosity(Verbosity.Minimal));
});

Task("build")
	.IsDependentOn("clean")
	.IsDependentOn("build-x86")
	.IsDependentOn("build-x64")
	.Does(() =>
{
});

Task("Default")
    .IsDependentOn("build")
	.Does(()=> 
{ 
});
	
RunTarget(target);
