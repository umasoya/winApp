object CsvRecForm: TCsvRecForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  ClientHeight = 283
  ClientWidth = 312
  Color = clBtnFace
  Constraints.MinWidth = 200
  DoubleBuffered = True
  ParentFont = True
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poDesigned
  ShowHint = True
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnHide = FormHide
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OptPanel: TPanel
    Left = 0
    Top = 252
    Width = 312
    Height = 31
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object TopIsHeaderCheckBox: TCheckBox
      Left = 12
      Top = 8
      Width = 157
      Height = 17
      Caption = #20808#38957#34892#12364#38917#30446#21517
      TabOrder = 0
      OnClick = TopIsHeaderCheckBoxClick
    end
  end
  object RecordHeader: THeaderControl
    Left = 0
    Top = 0
    Width = 312
    Height = 20
    Sections = <
      item
        AllowClick = False
        ImageIndex = -1
        Style = hsOwnerDraw
        Width = 201
      end
      item
        AllowClick = False
        ImageIndex = -1
        Style = hsOwnerDraw
        Width = 201
      end
      item
        ImageIndex = -1
        Style = hsOwnerDraw
        Width = 50
      end>
    Style = hsFlat
    OnDrawSection = RecordHeaderDrawSection
    OnSectionResize = RecordHeaderSectionResize
    OnResize = RecordHeaderResize
  end
  object GridPanel: TPanel
    Left = 0
    Top = 20
    Width = 312
    Height = 232
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    DesignSize = (
      312
      232)
    object RecordGrid: TStringGrid
      Left = 0
      Top = 0
      Width = 312
      Height = 234
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      Color = clAppWorkSpace
      ColCount = 2
      DefaultColWidth = 100
      FixedCols = 0
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goRangeSelect, goColSizing, goThumbTracking]
      PopupMenu = PopupMenu1
      TabOrder = 0
      StyleElements = [seBorder]
      OnDrawCell = RecordGridDrawCell
      ColWidths = (
        100
        100)
      RowHeights = (
        24
        24
        24
        24
        24)
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 56
    Top = 136
    object CopyItem: TMenuItem
      Caption = #12467#12500#12540'(&C)'
      OnClick = CopyItemClick
    end
  end
end
