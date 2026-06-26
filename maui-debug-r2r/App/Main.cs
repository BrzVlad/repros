using ReproApp;

// Use ThingImpl (and dispatch through IThing) so the Repro.Impl composite assembly and its
// MethodImpl survive full-link trimming. crossgen2's visibility root provider (Repro.Impl is
// [IsTrimmable]) then resolves that MethodImpl's interface and fails to load the dropped Repro.Base.
var thing = Repro.Impl.ThingImpl.Create ();
GC.KeepAlive (thing);

// This is the main entry point of the application.
// If you want to use a different Application Delegate class from "AppDelegate"
// you can specify it here.
UIApplication.Main (args, null, typeof (AppDelegate));
