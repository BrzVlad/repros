# Standalone crossgen2 ReadyToRun "Failed to load assembly" repro

Minimal, self-contained reproduction of the crossgen2 / ReadyToRun regression that broke
.NET MAUI CI (dotnet/maui PR #35364, build 1474132) after the SDK bump
`preview.5 -> preview.6` + macios `Microsoft.iOS.Sdk` 26.5 preview.6. Fixed by **dotnet/macios#25583**.

Original MAUI failure (`Core.DeviceTests.csproj`, `net11.0-ios`, `iossimulator-arm64`):

```
Microsoft.NET.CrossGen.targets(501,5): error : Failed to load assembly 'Microsoft.Maui'
  Internal.TypeSystem.TypeSystemException+FileNotFoundException: Failed to load assembly 'Microsoft.Maui'
    at ILCompiler.ReadyToRunVisibilityRootProvider.GetAllMethodImplRecordsForType(EcmaType)
    at ILCompiler.ReadyToRunVisibilityRootProvider.RootMethods(...)
    at ILCompiler.ReadyToRunVisibilityRootProvider.AddCompilationRoots(...)
```

This repo reproduces the **identical** failure with `Repro.Base` standing in for `Microsoft.Maui`,
in a plain non-MAUI iOS app.

## Run

```bash
./build-ios.sh        # plain `dotnet build` of the iOS app head -> reproduces the exact error
```

No Xcode 26.5 required: this build does **not** trim and does not use any iOS 26.5 API, so it
reaches the crossgen2 step directly. `reproduce.sh` is an Xcode-independent variant that drives
crossgen2 by hand and also demonstrates the fix:

## What actually happens

The macios iOS workload, in **Debug + CoreCLR + simulator**, uses these defaults
(`Xamarin.Shared.Sdk.props` / `Xamarin.Shared.Sdk.Trimming.props`):

- `UseMonoRuntime = false` — CoreCLR is now the default runtime.
- link mode = **None** — *trimming is off* (the EnC/debug-friendly default for the simulator).
- `PublishReadyToRun = true` with a crossgen2 **composite** image.

So assemblies reach crossgen2 **un-trimmed**, keeping their `[AssemblyMetadata("IsTrimmable","true")]`
attribute and all their `MethodImpl` records.

1. `_SelectR2RAssemblies` (`Microsoft.Sdk.R2R.targets`) splits the published assemblies: anything
   **without** a `NuGetPackageId` is a "user assembly" (`PublishReadyToRunExclude`) and is left out
   of the R2R composite to run interpreted. `Microsoft.Maui` (a project reference / `Repro.Base`,
   no PackageId) is excluded — **but it is still published** (it sits in `linked/` and the `.app`).
2. `Microsoft.Maui.Controls` (`Repro.Impl`) **is** in the composite and is **rooted**. crossgen2
   uses `ReadyToRunVisibilityRootProvider` for `[IsTrimmable]` modules, which roots every type and
   resolves the **declaring interface** of each `MethodImpl` to check its visibility.
3. `Controls`/`ThingImpl` implement an interface declared in `Microsoft.Maui`/`Repro.Base`.
   Resolving that MethodImpl forces crossgen2 to load `Repro.Base` — but the crossgen2 `--composite`
   command **neither lists it as an input nor passes it as `--reference`**, even though the dll is
   right there in `linked/`. → `FileNotFoundException`.

It is effectively a malformed command: crossgen2 is asked to *root* `Repro.Impl` without being
given `Repro.Impl`'s full dependency closure. Verified:

```
crossgen2 --composite Repro.Impl.dll CoreLib.dll                         -> Failed to load 'Repro.Base'
crossgen2 --composite --reference:Repro.Base.dll Repro.Impl.dll CoreLib  -> OK
```

### Two possible fixes
- **(A) pass the excluded assemblies as `--reference`** — fixes the command, keeps everything R2R'd.
- **(B) don't root the non-CoreLib inputs** — dotnet/macios#25583 sets
  `PublishReadyToRunCompositeRoots = System.Private.CoreLib.dll`, so every other assembly becomes an
  **unrooted** composite input and crossgen2 never enumerates its MethodImpls. This is what shipped,
  because it *also* shrinks the Debug `.app` (only CoreLib is R2R'd: 395 MB -> 132 MB).
  `reproduce.sh`'s `-u` invocation demonstrates exactly this.

> **Trimming masks the bug.** If you force `MtouchLink=SdkOnly` or `Full`, ILLink trims `Repro.Impl`,
> which strips its `[IsTrimmable]` attribute *and* the unused MethodImpl — so crossgen2 no longer
> uses the visibility provider and the build passes. The bug needs the default *no-trim* Debug path.

## How the projects map to MAUI

| Project | Consumed as | NuGetPackageId | `[IsTrimmable]` | Stands in for |
|---------|-------------|----------------|-----------------|---------------|
| `Base` (`Repro.Base.dll`, `interface IThing`)        | project reference | none | no  | `Microsoft.Maui` (excluded user assembly) |
| `Impl` (`Repro.Impl.dll`, `class ThingImpl : IThing`) | NuGet package     | `Repro.Impl` | yes | `Microsoft.Maui.Controls` (rooted composite) |
| `App`  (`net11.0-ios` exe)                            | references both   | -    | -   | `Core.DeviceTests` |

`ThingImpl` implements `IThing` **explicitly** (emits the `MethodImpl` record); `[IsTrimmable]` is set
via `Impl/AssemblyInfo.cs`. `App` leaves `MtouchLink` at the macios default (None) — the key to the repro.

## Custom dotnet

```bash
DOTNET=/path/to/custom/.dotnet/dotnet ./build-ios.sh     # or ./reproduce.sh
```

Edit `global.json` to the custom SDK's version. A build that contains macios#25583 (e.g. macios
>= the fixed 26.5) builds clean.

## Why a "normal" (non-macios) SDK can't hit it

The "exclude no-PackageId user assemblies from the R2R composite" heuristic lives only in the macios
workload (`Microsoft.Sdk.R2R.targets`). A normal .NET R2R publish (console/Windows/Linux) includes &
roots everything and passes the full reference set, so crossgen2 can always resolve the closure.
The bug therefore needs the **macios iOS workload** (CoreCLR simulator + composite R2R) — but **not**
MAUI itself, as this repo shows.
