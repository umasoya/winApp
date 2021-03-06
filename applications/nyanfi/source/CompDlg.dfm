object FileCompDlg: TFileCompDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #21516#21517#12501#12449#12452#12523#12398#27604#36611
  ClientHeight = 315
  ClientWidth = 414
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
  object CanButton: TButton
    Left = 219
    Top = 283
    Width = 80
    Height = 26
    Cancel = True
    Caption = #12461#12515#12531#12475#12523
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ModalResult = 2
    ParentFont = False
    TabOrder = 8
  end
  object OkButton: TButton
    Left = 114
    Top = 283
    Width = 80
    Height = 26
    Action = OkAction
    Default = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ModalResult = 1
    ParentFont = False
    TabOrder = 7
  end
  object CmpDirCheckBox: TCheckBox
    Left = 12
    Top = 225
    Width = 153
    Height = 17
    Caption = #12487#12451#12524#12463#12488#12522#12418#27604#36611'(&D)'
    TabOrder = 3
  end
  object TimePanel: TPanel
    Left = 0
    Top = 0
    Width = 414
    Height = 55
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    StyleElements = []
    DesignSize = (
      414
      55)
    object TimeRadioGroup: TRadioGroup
      Left = 8
      Top = 6
      Width = 398
      Height = 41
      Anchors = [akLeft, akTop, akRight]
      Caption = #12479#12452#12512#12473#12479#12531#12503'(&T)'
      Columns = 5
      ItemIndex = 0
      Items.Strings = (
        #28961#35222
        #19981#19968#33268
        #19968#33268
        #26032#12375#12356
        #21476#12356)
      TabOrder = 0
    end
  end
  object SizePanel: TPanel
    Left = 0
    Top = 55
    Width = 414
    Height = 55
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    StyleElements = []
    DesignSize = (
      414
      55)
    object SizeRadioGroup: TRadioGroup
      Left = 9
      Top = 6
      Width = 397
      Height = 41
      Anchors = [akLeft, akTop, akRight]
      Caption = #12469#12452#12474'(&S)'
      Columns = 5
      ItemIndex = 0
      Items.Strings = (
        #28961#35222
        #19981#19968#33268
        #19968#33268
        #22823#12365#12356
        #23567#12373#12356)
      TabOrder = 0
    end
  end
  object HashPanel: TPanel
    Left = 0
    Top = 110
    Width = 414
    Height = 100
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 2
    StyleElements = []
    DesignSize = (
      414
      100)
    object HashRadioGroup: TRadioGroup
      Left = 8
      Top = 6
      Width = 398
      Height = 87
      Anchors = [akLeft, akTop, akRight]
      Caption = #12495#12483#12471#12517'(&H)'
      Columns = 5
      ItemIndex = 0
      Items.Strings = (
        #28961#35222
        #19981#19968#33268
        #19968#33268)
      TabOrder = 0
    end
    object AlgRadioGroup: TRadioGroup
      Left = 220
      Top = 18
      Width = 181
      Height = 70
      Anchors = [akLeft, akTop, akRight]
      Caption = #12450#12523#12468#12522#12474#12512'(&A)'
      Columns = 2
      ItemIndex = 0
      Items.Strings = (
        'MD5'
        'SHA-1'
        'SHA-256'
        'SHA-384'
        'SHA-512'
        'CRC32')
      TabOrder = 1
    end
  end
  object SelOppCheckBox: TCheckBox
    Left = 167
    Top = 225
    Width = 136
    Height = 17
    Caption = #21453#23550#20596#12418#36984#25246'(&O)'
    TabOrder = 4
  end
  object ReverseCheckBox: TCheckBox
    Left = 305
    Top = 225
    Width = 99
    Height = 17
    Caption = #32080#26524#12434#21453#36578'(&R)'
    TabOrder = 5
  end
  object SelMaskCheckBox: TCheckBox
    Left = 12
    Top = 250
    Width = 213
    Height = 17
    Caption = #36984#25246#38917#30446#12384#12369#12434#27531#12377'(&M)'
    TabOrder = 6
  end
  object ActionList1: TActionList
    Left = 361
    Top = 264
    object OkAction: TAction
      Caption = 'OK'
      OnUpdate = OkActionUpdate
    end
  end
end
