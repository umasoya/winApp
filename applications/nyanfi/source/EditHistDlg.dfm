object EditHistoryDlg: TEditHistoryDlg
  Left = 0
  Top = 0
  HelpContext = 58
  BorderIcons = [biSystemMenu]
  ClientHeight = 341
  ClientWidth = 643
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
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object HiddenCanBtn: TButton
    Left = 0
    Top = 307
    Width = 643
    Height = 0
    Align = alBottom
    Cancel = True
    ModalResult = 2
    TabOrder = 0
  end
  object OptPanel: TPanel
    Left = 0
    Top = 307
    Width = 643
    Height = 34
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    object BtnPanel: TPanel
      Left = 625
      Top = 0
      Width = 18
      Height = 34
      Align = alRight
      AutoSize = True
      BevelOuter = bvNone
      TabOrder = 0
      DesignSize = (
        18
        34)
      object HideOptBtn: TSpeedButton
        Left = 0
        Top = 16
        Width = 18
        Height = 16
        Anchors = [akLeft, akBottom]
        Flat = True
        OnClick = ChgOptBtnClick
      end
    end
    object OptInPanel: TPanel
      Left = 0
      Top = 0
      Width = 625
      Height = 34
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
      object RadioPanel: TPanel
        Left = 0
        Top = 0
        Width = 286
        Height = 34
        Align = alLeft
        BevelOuter = bvNone
        TabOrder = 0
        object OptRadioGroup: TRadioGroup
          Left = -1
          Top = -5
          Width = 290
          Height = 40
          Columns = 3
          ItemIndex = 0
          Items.Strings = (
            #12377#12409#12390#34920#31034
            #12459#12524#12531#12488#20197#19979
            #12459#12524#12531#12488#12398#12415)
          TabOrder = 0
          OnClick = OptRadioGroupClick
        end
      end
      object OptSubPanel: TPanel
        Left = 286
        Top = 0
        Width = 204
        Height = 34
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 1
        DesignSize = (
          204
          34)
        object Bevel1: TBevel
          Left = 0
          Top = 0
          Width = 4
          Height = 34
          Align = alLeft
          Shape = bsLeftLine
        end
        object FilterEdit: TLabeledEdit
          Left = 55
          Top = 6
          Width = 70
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
        object MigemoCheckBox: TCheckBox
          Left = 131
          Top = 9
          Width = 72
          Height = 17
          Anchors = [akTop, akRight]
          Caption = 'Migemo'
          TabOrder = 1
        end
      end
      object MarkPanel: TPanel
        Left = 490
        Top = 0
        Width = 135
        Height = 34
        Align = alRight
        BevelOuter = bvNone
        Constraints.MinWidth = 4
        ParentBackground = False
        TabOrder = 2
        object MemoBtn: TSpeedButton
          Left = 85
          Top = 6
          Width = 50
          Height = 22
          Caption = #12513#12514
          OnClick = MemoItemClick
        end
        object OpenCheckBox: TCheckBox
          Left = 0
          Top = 9
          Width = 81
          Height = 17
          Caption = #30906#23450#12391#38283#12367
          TabOrder = 0
          OnClick = OpenCheckBoxClick
        end
      end
    end
  end
  object EditHistHeader: THeaderControl
    Left = 0
    Top = 0
    Width = 643
    Height = 17
    Sections = <>
    Style = hsFlat
    OnDrawSection = EditHistHeaderDrawSection
    OnSectionClick = EditHistHeaderSectionClick
    OnSectionResize = EditHistHeaderSectionResize
    OnResize = EditHistHeaderResize
  end
  object GridPanel: TPanel
    Left = 0
    Top = 17
    Width = 643
    Height = 290
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 3
    DesignSize = (
      643
      290)
    object EditHistGrid: TStringGrid
      Left = 0
      Top = 0
      Width = 643
      Height = 307
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      Color = clAppWorkSpace
      ColCount = 4
      DefaultDrawing = False
      FixedCols = 0
      FixedRows = 0
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Options = [goFixedVertLine, goColSizing, goRowSelect, goThumbTracking]
      ParentFont = False
      PopupMenu = MarkPopupMenu
      TabOrder = 0
      StyleElements = [seBorder]
      OnDblClick = EditHistGridDblClick
      OnDrawCell = EditHistGridDrawCell
      OnKeyDown = EditHistGridKeyDown
      ColWidths = (
        64
        64
        64
        64)
      RowHeights = (
        24
        24
        24
        24
        24)
    end
    object BlankPanel: TPanel
      Left = 629
      Top = 276
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
  object MarkPopupMenu: TPopupMenu
    Left = 144
    Top = 64
    object MemoItem: TMenuItem
      Caption = #12513#12514#12434#20837#21147'(&M)'
      OnClick = MemoItemClick
    end
    object ClrMemoItem: TMenuItem
      Caption = #12513#12514#12434#21066#38500'(&C)'
      OnClick = ClrMemoItemClick
    end
    object Sep_1: TMenuItem
      Caption = '-'
    end
    object DelMarkItem: TMenuItem
      Caption = #12510#12540#12463#35299#38500'(&D)'
      OnClick = DelMarkItemClick
    end
    object ClrAllMarkItem: TMenuItem
      Caption = #12377#12409#12390#12398#12510#12540#12463#12434#35299#38500'(&A)'
      OnClick = ClrAllMarkItemClick
    end
    object Sep_2: TMenuItem
      Caption = '-'
    end
    object FitSizePos2Item: TMenuItem
      Action = UserModule.SizePosToFlieListAction
    end
  end
  object HistPopupMenu: TPopupMenu
    Left = 48
    Top = 64
    object ClearAllItem: TMenuItem
      Caption = #23653#27508#12434#12377#12409#12390#21066#38500'(&A)'
      OnClick = ClearAllItemClick
    end
    object MenuItem1: TMenuItem
      Caption = '-'
    end
    object NoHistItem: TMenuItem
      Caption = #23653#27508#12395#20837#12428#12394#12356#12497#12473#12398#35373#23450'(&N)...'
      OnClick = NoHistItemClick
    end
    object FitSizePosItem: TMenuItem
      Action = UserModule.SizePosToFlieListAction
    end
  end
end
