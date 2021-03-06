object FuncListDlg: TFuncListDlg
  Left = 0
  Top = 0
  HelpContext = 66
  BorderIcons = [biSystemMenu]
  ClientHeight = 341
  ClientWidth = 484
  Color = clBtnFace
  Constraints.MinWidth = 400
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
  object UserDefPanel: TPanel
    Left = 0
    Top = 271
    Width = 484
    Height = 35
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    DesignSize = (
      484
      35)
    object UsrStrLabel: TLabel
      Left = 25
      Top = 13
      Width = 36
      Height = 13
      Alignment = taRightJustify
      Caption = #25991#23383#21015
    end
    object UserDefComboBox: TComboBox
      Left = 66
      Top = 10
      Width = 132
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      PopupMenu = UserModule.EditPopupMenu
      TabOrder = 0
      OnClick = UserDefComboBoxClick
    end
    object UpdateBtn: TButton
      Left = 402
      Top = 9
      Width = 75
      Height = 24
      Action = UpdUserDefAction
      Anchors = [akTop, akRight]
      TabOrder = 3
    end
    object RegExCheckBox: TCheckBox
      Left = 204
      Top = 12
      Width = 100
      Height = 17
      Anchors = [akTop, akRight]
      Caption = #27491#35215#34920#29694'(&R)'
      TabOrder = 1
      OnClick = RegExCheckBoxClick
    end
    object RegHeaderBtn: TButton
      Left = 308
      Top = 9
      Width = 90
      Height = 24
      Action = RegHeaderAction
      Anchors = [akTop, akRight]
      TabOrder = 2
    end
  end
  object BottomPanel: TPanel
    Left = 0
    Top = 306
    Width = 484
    Height = 35
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object Opt1Panel: TPanel
      Left = 332
      Top = 0
      Width = 76
      Height = 35
      Align = alRight
      BevelOuter = bvNone
      TabOrder = 1
      TabStop = True
      object NameOnlyCheckBox: TCheckBox
        Left = 2
        Top = 9
        Width = 71
        Height = 17
        Hint = #38306#25968#12398#21517#21069#37096#20998#12398#12415#34920#31034
        Caption = #21517#21069'(&N)'
        TabOrder = 0
        OnClick = NameOnlyCheckBoxClick
      end
    end
    object FilterPanel: TPanel
      Left = 0
      Top = 0
      Width = 332
      Height = 35
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      TabStop = True
      DesignSize = (
        332
        35)
      object MigemoCheckBox: TCheckBox
        Left = 251
        Top = 9
        Width = 80
        Height = 17
        Anchors = [akTop, akRight]
        Caption = '&Migemo'
        TabOrder = 1
        OnClick = MigemoCheckBoxClick
      end
      object FilterEdit: TLabeledEdit
        Left = 66
        Top = 8
        Width = 177
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 47
        EditLabel.Height = 13
        EditLabel.Caption = #12501#12451#12523#12479'(&F)'
        LabelPosition = lpLeft
        PopupMenu = UserModule.EditPopupMenuE
        TabOrder = 0
        OnChange = FilterEditChange
        OnEnter = FilterEditEnter
        OnExit = FilterEditExit
        OnKeyDown = FilterEditKeyDown
        OnKeyPress = FilterEditKeyPress
      end
    end
    object Opt2Panel: TPanel
      Left = 408
      Top = 0
      Width = 76
      Height = 35
      Align = alRight
      BevelOuter = bvNone
      TabOrder = 2
      TabStop = True
      object LinkCheckBox: TCheckBox
        Left = 2
        Top = 9
        Width = 70
        Height = 17
        Hint = #12499#12517#12450#12540#12398#12459#12540#12477#12523#34892#12434#36899#21205
        Caption = #36899#21205'(&L)'
        TabOrder = 0
        OnClick = NameOnlyCheckBoxClick
      end
    end
  end
  object ListPanel: TPanel
    Left = 0
    Top = 0
    Width = 484
    Height = 271
    Align = alClient
    BevelOuter = bvNone
    DoubleBuffered = False
    ParentDoubleBuffered = False
    TabOrder = 2
    object FuncListBox: TListBox
      Left = 0
      Top = 0
      Width = 484
      Height = 271
      Style = lbOwnerDrawFixed
      AutoComplete = False
      Align = alLeft
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      DoubleBuffered = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #65325#65331' '#12468#12471#12483#12463
      Font.Style = []
      MultiSelect = True
      ParentDoubleBuffered = False
      ParentFont = False
      PopupMenu = PopupMenu1
      TabOrder = 0
      StyleElements = [seBorder]
      OnClick = FuncListBoxClick
      OnDblClick = FuncListBoxDblClick
      OnDrawItem = FuncListBoxDrawItem
      OnKeyDown = FuncListBoxKeyDown
      OnKeyPress = FuncListBoxKeyPress
    end
  end
  object ActionList1: TActionList
    Left = 72
    Top = 16
    object UpdUserDefAction: TAction
      Caption = #26356#26032'(&U)'
      OnExecute = UpdUserDefActionExecute
      OnUpdate = UpdUserDefActionUpdate
    end
    object ReqEditAction: TAction
      ShortCut = 32837
      OnExecute = ReqEditActionExecute
    end
    object CloseListAction: TAction
      Caption = 'CloseListAction'
      OnExecute = CloseListActionExecute
    end
    object CopyListAction: TAction
      Caption = #19968#35239#12434#12467#12500#12540'(&C)'
      OnExecute = CopyListActionExecute
      OnUpdate = ListActionUpdate
    end
    object SaveListAction: TAction
      Caption = #19968#35239#12434#12501#12449#12452#12523#12395#20445#23384'(&S)'
      OnExecute = SaveListActionExecute
      OnUpdate = ListActionUpdate
    end
    object RegHeaderAction: TAction
      Caption = #35211#20986#12375#30331#37682
      OnExecute = RegHeaderActionExecute
      OnUpdate = RegHeaderActionUpdate
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 72
    Top = 76
    object CopyListItem: TMenuItem
      Action = CopyListAction
    end
    object SaveListItem: TMenuItem
      Action = SaveListAction
    end
  end
end
