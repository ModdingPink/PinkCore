param (
    [Parameter(Mandatory=$false)]
    [Switch]$debug_so,
    [Parameter(Mandatory=$false)]
    [Switch]$log
)

& ./build.ps1
if ($debug.IsPresent) {
    & adb push build/debug_libpinkcore.so /sdcard/Android/data/com.beatgames.beatsaber/files/libs/libpinkcore.so
} else {
    & adb push build/libpinkcore.so /sdcard/Android/data/com.beatgames.beatsaber/files/libs/libpinkcore.so
}

& adb shell am force-stop com.beatgames.beatsaber
& adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
if ($log.IsPresent) {
    & ./log.ps1
}