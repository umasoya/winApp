object AboutBox: TAboutBox
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize, biMaximize, biHelp]
  BorderStyle = bsToolWindow
  Caption = #12496#12540#12472#12519#12531#24773#22577
  ClientHeight = 222
  ClientWidth = 354
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 338
    Height = 161
    BevelInner = bvRaised
    BevelOuter = bvLowered
    ParentColor = True
    TabOrder = 0
    object ProgramIcon: TImage
      Left = 16
      Top = 16
      Width = 64
      Height = 64
      Stretch = True
      IsControl = True
    end
    object ProductName: TLabel
      Left = 79
      Top = 20
      Width = 180
      Height = 37
      Alignment = taCenter
      AutoSize = False
      Caption = 'NyanFi'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -27
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      IsControl = True
    end
    object Version: TLabel
      Left = 79
      Top = 60
      Width = 180
      Height = 26
      Alignment = taCenter
      AutoSize = False
      Caption = 'Version'
      IsControl = True
    end
    object Copyright: TLabel
      Left = 37
      Top = 96
      Width = 263
      Height = 26
      Alignment = taCenter
      AutoSize = False
      Caption = 'Copyright'
      IsControl = True
    end
    object SupportURL: TLabel
      Left = 37
      Top = 126
      Width = 263
      Height = 26
      Alignment = taCenter
      AutoSize = False
      Caption = 'http://'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsUnderline]
      ParentFont = False
      OnClick = SupportURLClick
      IsControl = True
    end
  end
  object OKButton: TButton
    Left = 139
    Top = 187
    Width = 75
    Height = 26
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
  object CancelBtn: TButton
    Left = 0
    Top = 222
    Width = 354
    Height = 0
    Align = alBottom
    Cancel = True
    ModalResult = 2
    TabOrder = 2
  end
end
