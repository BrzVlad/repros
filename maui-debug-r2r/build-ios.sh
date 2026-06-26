#!/usr/bin/env bash
#
# Reproduce the crossgen2 / ReadyToRun "Failed to load assembly" regression (dotnet/macios#25583)
# with a plain `dotnet build` of a minimal, non-MAUI net11.0-ios app.
#
# The app leaves MtouchLink at the macios DEFAULT for CoreCLR + simulator (= None / no trimming),
# so Repro.Impl reaches crossgen2 [IsTrimmable] with its MethodImpl intact, and the no-PackageId
# Repro.Base is excluded from the R2R composite (and not passed as a --reference) -> crash.
# (Forcing MtouchLink=SdkOnly/Full would TRIM Repro.Impl and mask the bug.)
#
# Custom dotnet:  DOTNET=/path/to/custom/.dotnet/dotnet ./build-ios.sh
# Needs the macios iOS workload; does NOT need Xcode 26.5 (no trimming -> no MT0180; no 26.5 APIs).

set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"

DOTNET="${DOTNET:-/Users/vbrezae/Xamarin/repos/maui/.dotnet/dotnet}"
CONFIG="${CONFIG:-Debug}"

echo "==> dotnet: $DOTNET"
"$DOTNET" --version

echo "==> Resetting local feed"
rm -rf "$HERE/localfeed" "$HOME/.nuget/packages/repro.impl"; mkdir -p "$HERE/localfeed"
rm -rf "$HERE"/*/obj "$HERE"/*/bin

echo "==> [1/3] Building Base (project reference, NO PackageId -> the excluded 'user' assembly)"
"$DOTNET" build "$HERE/Base/Base.csproj" -c "$CONFIG"

echo "==> [2/3] Packing Impl as a NuGet package (HAS PackageId, [IsTrimmable] -> rooted composite)"
"$DOTNET" pack "$HERE/Impl/Impl.csproj" -c "$CONFIG" -o "$HERE/localfeed"

echo "==> [3/3] Building the iOS app head for net11.0-ios (default link mode None -> crossgen2 R2R)"
echo "    Expected: error : Failed to load assembly 'Repro.Base'  (NETSDK1096)"
"$DOTNET" build "$HERE/App/ReproApp.csproj" -c "$CONFIG" -f net11.0-ios \
  -p:TreatWarningsAsErrors=false \
  -bl:"$HERE/repro.binlog"


