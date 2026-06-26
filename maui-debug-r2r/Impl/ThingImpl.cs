using Repro.Base;

namespace Repro.Impl;

// NuGet (PackageId) + [IsTrimmable] -> crossgen2 uses ReadyToRunVisibilityRootProvider, which
// reads every type's MethodImpl table and resolves each record's declaring interface. ThingImpl
// explicitly implements Repro.Base.IThing. A linker descriptor (App/roots.xml) preserves this
// assembly so the MethodImpl survives, but nothing roots IThing, so full link drops Repro.Base
// -> crossgen2 cannot resolve it -> "Failed to load assembly 'Repro.Base'".
public class ThingImpl : IThing
{
    void IThing.Do()
    {
    }

    int IThing.Compute(int value) => value * 2;

    public static ThingImpl Create() => new ThingImpl();
}
