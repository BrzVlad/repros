This solution consists of two projects, the console app `app` and the classlib `lib`. Both projects reference the `Microsoft.Diagnostics.Tracing.TraceEvent` package. This package contains dlls, for example `Dia2Lib.dll` which get included to the final build via a `CopyToOutputDirectory` msbuild property. The issue is reproduced only when doing a composite r2r publish. The failure is that an assembly is passed twice to the `crossgen2` command. The set of assemblies contains duplicated items following a computation here: https://github.com/dotnet/sdk/blob/v10.0.101/src/Tasks/Microsoft.NET.Build.Tasks/targets/Microsoft.NET.Publish.targets#L804

In order to repro:
- `cd app`
- `dotnet publish`

The produced failure is:
```
vbrezae@ubuntuvm:~/repros/composite-r2r-publish/app$ dotnet publish
Restore complete (0.3s)
  lib net10.0 succeeded (0.1s) → /home/vbrezae/repros/composite-r2r-publish/lib/bin/Release/net10.0/lib.dll
  app net10.0 linux-arm64 failed with 11 error(s) (1.3s) → bin/Release/net10.0/linux-arm64/app.dll
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error Unhandled exception. System.CommandLine.CommandLineException: Multiple input files matching same simple name /home/vbrezae/.nuget/packages/microsoft.diagnostics.tracing.traceevent/3.1.21/lib/netstandard2.0/Dia2Lib.dll /home/vbrezae/.nuget/packages/microsoft.diagnostics.tracing.traceevent/3.1.21/lib/netstandard2.0/Dia2Lib.dll
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Helpers.AppendExpandedPaths(Dictionary`2, String, Boolean) + 0x168
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Helpers.BuildPathDictionary(IReadOnlyList`1, Boolean) + 0x80
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Argument`1.<>c__DisplayClass8_0.<set_CustomParser>b__0(ArgumentResult, Object&) + 0x44
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Parsing.ArgumentResult.ValidateAndConvert(Boolean) + 0x198
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Parsing.CommandResult.ValidateArgumentsAndAddDefaultResults(Boolean) + 0x1cc
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Parsing.ParseOperation.ValidateAndAddDefaultResults() + 0x3c
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Parsing.ParseOperation.Parse() + 0x2c
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at System.CommandLine.Parsing.CommandLineParser.Parse(Command, IReadOnlyList`1, String, ParserConfiguration) + 0x90
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(467,5): error    at ILCompiler.Program.Main(String[] args) + 0xfc
    /home/vbrezae/.dotnet/sdk/10.0.101/Sdks/Microsoft.NET.Sdk/targets/Microsoft.NET.CrossGen.targets(353,5): error NETSDK1096: Optimizing assemblies for performance failed. You can either exclude the failing assemblies from being optimized, or set the PublishReadyToRun property to false.

Build failed with 11 error(s) in 1.9s
```
