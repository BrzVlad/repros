// Mark Repro.Impl as trimmable via a source attribute (NOT the <IsTrimmable> MSBuild property)
// so the attribute is emitted but the assembly is not forced into ILLink's "link" action under
// SdkOnly. crossgen2 reads this attribute to decide whether to use the visibility root provider.
[assembly: System.Reflection.AssemblyMetadata("IsTrimmable", "True")]
