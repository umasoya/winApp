object CvTxtEncDlg: TCvTxtEncDlg
  Left = 0
  Top = 0
  Anchors = [akRight, akBottom]
  BorderStyle = bsDialog
  Caption = #25991#23383#12467#12540#12489#12398#22793#25563
  ClientHeight = 83
  ClientWidth = 394
  Color = clBtnFace
  ParentFont = True
  KeyPreview = True
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 253
    Top = 11
    Width = 53
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = #25913#34892'(&R)'
    FocusControl = OutLnBrkComboBox
    WordWrap = True
  end
  object Label1: TLabel
    Left = 6
    Top = 11
    Width = 86
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = #20986#21147#12467#12540#12489'(&C)'
    FocusControl = OutCodeComboBox
    WordWrap = True
  end
  object CanButton: TButton
    Left = 306
    Top = 49
    Width = 80
    Height = 26
    Cancel = True
    Caption = #12461#12515#12531#12475#12523
    ModalResult = 2
    TabOrder = 3
  end
  object OutLnBrkComboBox: TComboBox
    Left = 313
    Top = 8
    Width = 72
    Height = 21
    Style = csDropDownList
    TabOrder = 1
  end
  object OutCodeComboBox: TComboBox
    Left = 95
    Top = 8
    Width = 145
    Height = 21
    Style = csDropDownList
    TabOrder = 0
  end
  object OkButton: TButton
    Left = 210
    Top = 49
    Width = 80
    Height = 26
    Caption = #38283#22987
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
end
