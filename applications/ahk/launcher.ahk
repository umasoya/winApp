; ---------------------------------------------------------------------------
; Launcher
; ---------------------------------------------------------------------------
!f::launchOrActivate("firefox.exe")
!g::launchOrActivate("chrome.exe")
!m::launchOrActivate("thunderbird.exe")
!s::launchOrActivate("slack.exe")
!u::launchOrActivate("ubuntu.exe")

launchOrActivate(target)
{
    Process, Exist, %target%
    If ErrorLevel <> 0
        WinActivate, ahk_pid %ErrorLevel%
    Else
        Run, %target%
    Return
}
