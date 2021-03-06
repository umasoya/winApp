object FileInfoDlg: TFileInfoDlg
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  ClientHeight = 448
  ClientWidth = 470
  Color = clBtnFace
  ParentFont = True
  KeyPreview = True
  OldCreateOrder = False
  Position = poDesigned
  ShowHint = True
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListPanel: TPanel
    Left = 0
    Top = 0
    Width = 470
    Height = 448
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object InfListBox: TListBox
      Left = 0
      Top = 0
      Width = 470
      Height = 448
      Style = lbOwnerDrawFixed
      Align = alLeft
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      Constraints.MinHeight = 32
      Constraints.MinWidth = 32
      DoubleBuffered = False
      ItemHeight = 13
      MultiSelect = True
      ParentDoubleBuffered = False
      PopupMenu = InfPopupMenu
      TabOrder = 0
      StyleElements = [seBorder]
      OnDblClick = InfListBoxDblClick
      OnDrawItem = InfListBoxDrawItem
      OnKeyDown = InfListBoxKeyDown
      OnKeyPress = InfListBoxKeyPress
    end
  end
  object InfPopupMenu: TPopupMenu
    AutoHotkeys = maManual
    Left = 40
    Top = 38
    object PopCopyFileInfoItm: TMenuItem
      Action = CopyInfoAction
    end
    object Sep_i_1: TMenuItem
      Caption = '-'
    end
    object InfEditCopyItem: TMenuItem
      Action = CopyAction
    end
    object InfEditSelectAllItem: TMenuItem
      Action = SelAllAction
      ShortCut = 16449
    end
    object Sep_i_2: TMenuItem
      Caption = '-'
    end
    object URLW1: TMenuItem
      Action = OpenUrlAction
    end
    object Sep_i_3: TMenuItem
      Caption = '-'
    end
    object PropertyDlgItem: TMenuItem
      Action = PropertyDlgAction
    end
  end
  object ActionList1: TActionList
    Left = 128
    Top = 40
    object CopyInfoAction: TAction
      Caption = #24773#22577#12434#12467#12500#12540'(&I)'
      OnExecute = CopyInfoActionExecute
    end
    object CopyAction: TAction
      Caption = #12467#12500#12540'(&C)'
      ShortCut = 16451
      OnExecute = CopyActionExecute
      OnUpdate = CopyActionUpdate
    end
    object SelAllAction: TAction
      Caption = #12377#12409#12390#36984#25246'(&A)'
      OnExecute = SelAllActionExecute
    end
    object PropertyDlgAction: TAction
      Caption = #12503#12525#12497#12486#12451'(&R)'
      OnExecute = PropertyDlgActionExecute
      OnUpdate = PropertyDlgActionUpdate
    end
    object OpenUrlAction: TAction
      Caption = 'URL'#12434#38283#12367'(&W)'
      ShortCut = 13
      OnExecute = OpenUrlActionExecute
      OnUpdate = OpenUrlActionUpdate
    end
  end
end
