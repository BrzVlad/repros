namespace Repro.Base;

// Interface declared in the project-referenced (no PackageId) assembly.
// A type in the NuGet-packaged "Impl" assembly implements it explicitly, which
// produces a MethodImpl record pointing back at this assembly. crossgen2's
// ReadyToRunVisibilityRootProvider.GetAllMethodImplRecordsForType must resolve
// THIS assembly while rooting the composite — and fails when it has been excluded.
public interface IThing
{
    void Do();
    int Compute(int value);
}
