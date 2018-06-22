object TagManDlg: TTagManDlg
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  ClientHeight = 224
  ClientWidth = 312
  Color = clBtnFace
  Constraints.MinWidth = 320
  ParentFont = True
  KeyPreview = True
  OldCreateOrder = False
  Position = poDesigned
  ShowHint = True
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListPanel: TPanel
    Left = 0
    Top = 23
    Width = 312
    Height = 121
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    DesignSize = (
      312
      121)
    object TagCheckListBox: TCheckListBox
      Left = 0
      Top = 0
      Width = 312
      Height = 121
      OnClickCheck = TagCheckListBoxClickCheck
      Align = alClient
      BorderStyle = bsNone
      DoubleBuffered = True
      ItemHeight = 13
      ParentDoubleBuffered = False
      PopupMenu = PopupMenu1
      Style = lbOwnerDrawFixed
      TabOrder = 0
      OnClick = TagCheckListBoxClick
      OnDrawItem = TagCheckListBoxDrawItem
      OnKeyDown = TagCheckListBoxKeyDown
      OnKeyPress = TagCheckListBoxKeyPress
    end
    object BlankPanel: TPanel
      Left = 298
      Top = 107
      Width = 14
      Height = 14
      Anchors = [akRight, akBottom]
      AutoSize = True
      BevelOuter = bvNone
      Color = clAppWorkSpace
      ParentBackground = False
      TabOrder = 1
      Visible = False
      StyleElements = [seBorder]
      object ShowOptBtn: TSpeedButton
        Left = 0
        Top = 0
        Width = 14
        Height = 14
        Flat = True
        OnClick = ChgOptBtnClick
      end
    end
  end
  object OptPanel: TPanel
    Left = 0
    Top = 144
    Width = 312
    Height = 80
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object InpOptPanel: TPanel
      Left = 0
      Top = 40
      Width = 312
      Height = 40
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        312
        40)
      object HideOptBtn: TSpeedButton
        Left = 294
        Top = 24
        Width = 18
        Height = 16
        Anchors = [akTop, akRight]
        Flat = True
        OnClick = ChgOptBtnClick
      end
      object CanButton: TButton
        Left = 215
        Top = 8
        Width = 76
        Height = 26
        Anchors = [akTop, akRight]
        Cancel = True
        Caption = #12461#12515#12531#12475#12523
        ModalResult = 2
        TabOrder = 2
      end
      object OkButton: TButton
        Left = 131
        Top = 8
        Width = 76
        Height = 26
        Anchors = [akTop, akRight]
        Caption = 'OK'
        Default = True
        ModalResult = 1
        TabOrder = 1
      end
      object HideCheckBox: TCheckBox
        Left = 10
        Top = 8
        Width = 117
        Height = 17
        Caption = #20837#21147#27396#12434#38560#12377'(&H)'
        TabOrder = 0
        OnClick = HideCheckBoxClick
      end
    end
    object FindOptPanel: TPanel
      Left = 0
      Top = 8
      Width = 312
      Height = 32
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 0
      object ResLinkCheckBox: TCheckBox
        Left = 103
        Top = 8
        Width = 200
        Height = 17
        Caption = #32080#26524#12522#12473#12488#12363#12425#21453#23550#20596#12408#21453#26144'(&R)'
        TabOrder = 1
        OnClick = OptCheckBoxClick
      end
      object AndCheckBox: TCheckBox
        Left = 10
        Top = 8
        Width = 91
        Height = 17
        Caption = '&AND'#26908#32034
        TabOrder = 0
        OnClick = OptCheckBoxClick
      end
    end
  end
  object InpPanel: TPanel
    Left = 0
    Top = 0
    Width = 312
    Height = 23
    Align = alTop
    AutoSize = True
    BevelOuter = bvNone
    BorderWidth = 1
    TabOrder = 2
    object TagEdit: TEdit
      Left = 1
      Top = 1
      Width = 310
      Height = 21
      Align = alTop
      PopupMenu = UserModule.EditPopupMenuE
      TabOrder = 0
      OnChange = TagEditChange
      OnKeyDown = TagEditKeyDown
      OnKeyPress = TagEditKeyPress
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 24
    Top = 56
    object RenTagItem: TMenuItem
      Action = RenTagAction
    end
    object DelTagItem: TMenuItem
      Action = DelTagAction
    end
    object Sep_1: TMenuItem
      Caption = '-'
    end
    object CountTagItem: TMenuItem
      Action = CountTagAction
    end
    object U1: TMenuItem
      Action = ShowTagCountAction
    end
    object Sep_2: TMenuItem
      Caption = '-'
    end
    object TrimDataItem: TMenuItem
      Action = TrimDataAction
    end
  end
  object ActionList1: TActionList
    Left = 104
    Top = 56
    object RenTagAction: TAction
      Caption = #12479#12464#21517#12398#22793#26356'(&R)'
      OnExecute = RenTagActionExecute
      OnUpdate = SelActionUpdate
    end
    object DelTagAction: TAction
      Caption = #12479#12464#21517#12398#21066#38500'(&D)'
      OnExecute = DelTagActionExecute
      OnUpdate = SelActionUpdate
    end
    object TrimDataAction: TAction
      Caption = #12479#12464#12487#12540#12479#12398#25972#29702'(&U)'
      OnExecute = TrimDataActionExecute
    end
    object CountTagAction: TAction
      Caption = #12479#12464#12398#20351#29992#25968#12434#12459#12454#12531#12488'(&C)'
      OnExecute = CountTagActionExecute
      OnUpdate = CountTagActionUpdate
    end
    object ShowTagCountAction: TAction
      Caption = #12479#12464#12398#20351#29992#25968#12434#24120#12395#34920#31034'(&S)'
      OnExecute = ShowTagCountActionExecute
      OnUpdate = ShowTagCountActionUpdate
    end
  end
end
