object KeyListDlg: TKeyListDlg
  Left = 0
  Top = 0
  HelpContext = 55
  BorderIcons = [biSystemMenu]
  Caption = #12461#12540#21106#12426#24403#12390#19968#35239
  ClientHeight = 441
  ClientWidth = 613
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
  object KeyTabControl: TTabControl
    Left = 0
    Top = 381
    Width = 613
    Height = 60
    Align = alBottom
    OwnerDraw = True
    TabOrder = 2
    TabPosition = tpBottom
    Tabs.Strings = (
      #12501#12449#12452#12521#12540
      'INC.'#12469#12540#12481
      #12486#12461#12473#12488#12499#12517#12450#12540
      #12452#12513#12540#12472#12499#12517#12450#12540
      #12525#12464#12454#12451#12531#12489#12454)
    TabIndex = 0
    TabWidth = 120
    OnChange = KeyTabControlChange
    OnDrawTab = KeyTabControlDrawTab
    DesignSize = (
      613
      60)
    object FilterEdit: TLabeledEdit
      Left = 72
      Top = 6
      Width = 197
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 33
      EditLabel.Height = 13
      EditLabel.Caption = #12501#12451#12523#12479
      LabelPosition = lpLeft
      PopupMenu = UserModule.EditPopupMenuE
      TabOrder = 0
      OnChange = FilterEditChange
      OnEnter = FilterEditEnter
      OnExit = FilterEditExit
      OnKeyDown = FilterEditKeyDown
      OnKeyPress = FilterEditKeyPress
    end
    object ShowAllCmdCheckBox: TCheckBox
      Left = 353
      Top = 8
      Width = 158
      Height = 17
      Anchors = [akTop, akRight]
      Caption = #26410#30331#37682#12467#12510#12531#12489#12418#34920#31034
      TabOrder = 2
      OnClick = ShowAllCmdCheckBoxClick
    end
    object MigemoCheckBox: TCheckBox
      Left = 277
      Top = 8
      Width = 74
      Height = 17
      Anchors = [akTop, akRight]
      Caption = 'Migemo'
      TabOrder = 1
      OnClick = MigemoCheckBoxClick
    end
    object CnfExeCheckBox: TCheckBox
      Left = 515
      Top = 8
      Width = 88
      Height = 17
      Anchors = [akTop, akRight]
      Caption = #30906#23450#21363#23455#34892
      TabOrder = 3
    end
  end
  object HiddenCanBtn: TButton
    Left = 0
    Top = 381
    Width = 613
    Height = 0
    Align = alBottom
    Cancel = True
    ModalResult = 2
    TabOrder = 3
  end
  object KeyListHeader: THeaderControl
    Left = 0
    Top = 0
    Width = 613
    Height = 20
    Sections = <
      item
        ImageIndex = -1
        Style = hsOwnerDraw
        Width = 61
      end
      item
        ImageIndex = -1
        Style = hsOwnerDraw
        Width = 61
      end
      item
        ImageIndex = -1
        Style = hsOwnerDraw
        Width = 61
      end
      item
        ImageIndex = -1
        Style = hsOwnerDraw
        Width = 50
      end>
    OnDrawSection = KeyListHeaderDrawSection
    OnSectionClick = KeyListHeaderSectionClick
    OnSectionResize = KeyListHeaderSectionResize
    OnResize = KeyListHeaderResize
  end
  object GridPanel: TPanel
    Left = 0
    Top = 20
    Width = 613
    Height = 361
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    DesignSize = (
      613
      361)
    object KeyListGrid: TStringGrid
      Left = 0
      Top = 0
      Width = 613
      Height = 363
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      Color = clAppWorkSpace
      ColCount = 3
      DefaultRowHeight = 22
      DefaultDrawing = False
      FixedCols = 0
      FixedRows = 0
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #65325#65331' '#12468#12471#12483#12463
      Font.Style = []
      Options = [goFixedVertLine, goColSizing, goRowSelect, goThumbTracking]
      ParentFont = False
      PopupMenu = PopupMenu1
      TabOrder = 0
      StyleElements = [seBorder]
      OnDblClick = KeyListGridDblClick
      OnDrawCell = KeyListGridDrawCell
      OnKeyDown = KeyListGridKeyDown
      ColWidths = (
        64
        64
        64)
      RowHeights = (
        22
        22
        22
        22
        22)
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 40
    Top = 128
    object CopyListItem: TMenuItem
      Caption = #19968#35239#12434#12467#12500#12540'(&C)'
      OnClick = CopyListItemClick
    end
    object SaveAsListItem: TMenuItem
      Caption = #19968#35239#12434#12501#12449#12452#12523#12395#20445#23384'(&S)'
      OnClick = SaveAsListItemClick
    end
    object CopyCmdItem: TMenuItem
      Caption = #12467#12510#12531#12489#12434#12467#12500#12540'(&B)'
      OnClick = CopyCmdItemClick
    end
    object Sep_1: TMenuItem
      Caption = '-'
    end
    object SortKeyItem: TMenuItem
      Action = SortKeyAction
    end
    object SortCmdItem: TMenuItem
      Tag = 1
      Action = SortCmdAction
    end
    object SortDscItem: TMenuItem
      Tag = 2
      Caption = #12477#12540#12488': '#35500#26126'(&D)'
      OnClick = SortKeyActionExecute
    end
    object Sep_2: TMenuItem
      Caption = '-'
    end
    object OptionItem: TMenuItem
      Caption = #12458#12503#12471#12519#12531#35373#23450'(&O)'
      OnClick = OptionItemClick
    end
    object FitSizePosItem: TMenuItem
      Action = UserModule.SizePosToFlieListAction
    end
  end
  object ActionList1: TActionList
    Left = 40
    Top = 64
    object SortKeyAction: TAction
      Caption = #12477#12540#12488': '#12461#12540'(&K)'
      OnExecute = SortKeyActionExecute
      OnUpdate = SortKeyActionUpdate
    end
    object SortCmdAction: TAction
      Tag = 1
      Caption = #12477#12540#12488': '#12467#12510#12531#12489'(&M)'
      OnExecute = SortKeyActionExecute
      OnUpdate = SortKeyActionUpdate
    end
  end
end
