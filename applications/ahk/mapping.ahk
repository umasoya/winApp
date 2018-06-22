; ---------------------------------------------------------------------------
; Disabled
; ---------------------------------------------------------------------------

; 無変換
Hotkey, SC07B, Disable
;変換
Hotkey, SC079, Disable
;ひらがな/カタカナ
HotKey, SC070, Disable

; ---------------------------------------------------------------------------
; Label
; ---------------------------------------------------------------------------
Disable:
Return

; ---------------------------------------------------------------------------
; Mapping
; ---------------------------------------------------------------------------
; CapsLock -> Ctrl
CapsLock::Ctrl

; Alt+Space -> DoubleByte Space(Unicode)
; !Space::Send, {U+3000}

; 無変換/変換+Space -> Toggle Input mode
SC07B & Space::
SC079 & Space::SC029

; Script Reload
!r::Reload

; ---------------------------------------------------------------------------
; For specific software
; ---------------------------------------------------------------------------
; PDF-XChanger Viewer
#IfWinExist ahk_class DSUI:PDFXCViewer
    !p::WinActivate, ahk_class DSUI:PDFXCViewer
#IfWinExist

#IfWinActive ahk_class DSUI:PDFXCViewer
    ^h::Left
    ^j::Down
    ^k::Up
    ^l::Right
#IfWinActive


; ---------------------------------------------------------------------------
; Window controll
; ---------------------------------------------------------------------------
^!Space::toggleWinMaximize()
!q::WinClose, A

; Toggle active window maximization
toggleWinMaximize()
{
    WinGet, maxp, MinMax, A
    If maxp = 1
        WinRestore, A
    Else
        WinMaximize, A
    Return
}
