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

Task("write-version")
	.Does(() =>
{
	var version = System.Environment.GetEnvironmentVariable("APPVEYOR_BUILD_VERSION") ?? "1.0.0";
	var file = File("./src/build_version.h");
	System.IO.File.WriteAllText(file, "#pragma once\n");
	System.IO.File.AppendAllText(file, $"#define BUILD_VERSION {version.Replace('.',',')}\n");
	System.IO.File.AppendAllText(file, $"#define BUILD_VERSION_STR \"{version}\"\n");
	version = version.Substring(0, version.LastIndexOf('.'));
	System.IO.File.AppendAllText(file, $"#define PRODUCT_VERSION {version.Replace('.',',')}\n");
	System.IO.File.AppendAllText(file, $"#define PRODUCT_VERSION_STR \"{version}\"\n");
});

Task("build")
	.IsDependentOn("clean")
	.IsDependentOn("write-version")
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
