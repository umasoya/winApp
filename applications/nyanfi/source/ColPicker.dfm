object ColorPicker: TColorPicker
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = #12459#12521#12540#12500#12483#12459#12540
  ClientHeight = 113
  ClientWidth = 314
  Color = clBtnFace
  ParentFont = True
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  DesignSize = (
    314
    113)
  PixelsPerInch = 96
  TextHeight = 13
  object ColLabel: TLabel
    Left = 8
    Top = 4
    Width = 3
    Height = 13
    Anchors = [akLeft, akTop, akRight]
  end
  object ColEdit: TEdit
    Left = 119
    Top = 84
    Width = 136
    Height = 21
    TabStop = False
    Anchors = [akLeft, akTop, akRight]
    ReadOnly = True
    TabOrder = 6
  end
  object ColPanel: TPanel
    Left = 260
    Top = 4
    Width = 50
    Height = 50
    Anchors = [akTop, akRight]
    BevelOuter = bvLowered
    TabOrder = 0
    StyleElements = []
    object ColImage: TImage
      Left = 1
      Top = 1
      Width = 48
      Height = 48
      Align = alClient
    end
  end
  object FmtComboBox: TComboBox
    Left = 8
    Top = 84
    Width = 110
    Height = 21
    Style = csDropDownList
    DropDownCount = 12
    TabOrder = 5
  end
  object Col2Panel: TPanel
    Left = 260
    Top = 58
    Width = 50
    Height = 50
    Anchors = [akTop, akRight]
    BevelOuter = bvLowered
    TabOrder = 3
    StyleElements = []
  end
  object Ave9pxCheckBox: TCheckBox
    Left = 8
    Top = 59
    Width = 80
    Height = 17
    Caption = '9px'#24179#22343
    TabOrder = 1
  end
  object SafeColCheckBox: TCheckBox
    Left = 150
    Top = 59
    Width = 105
    Height = 17
    Caption = #12475#12540#12501#12459#12521#12540
    TabOrder = 2
  end
  object CmpColCheckBox: TCheckBox
    Left = 89
    Top = 59
    Width = 60
    Height = 17
    Caption = #35036#33394
    TabOrder = 4
  end
end
