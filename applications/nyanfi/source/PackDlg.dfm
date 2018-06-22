object PackArcDlg: TPackArcDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #12450#12540#12459#12452#12502#12398#20316#25104
  ClientHeight = 319
  ClientWidth = 414
  Color = clBtnFace
  ParentFont = True
  KeyPreview = True
  OldCreateOrder = False
  Position = poDesigned
  ShowHint = True
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  DesignSize = (
    414
    319)
  PixelsPerInch = 96
  TextHeight = 13
  object FextLabel: TLabel
    Left = 354
    Top = 11
    Width = 17
    Height = 13
    Caption = '.zip'
  end
  object OkButton: TButton
    Left = 119
    Top = 287
    Width = 80
    Height = 26
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 5
  end
  object ArcNameEdit: TLabeledEdit
    Left = 106
    Top = 8
    Width = 289
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    EditLabel.Width = 73
    EditLabel.Height = 13
    EditLabel.Caption = #12450#12540#12459#12452#12502#21517'(&A)'
    LabelPosition = lpLeft
    PopupMenu = UserModule.EditPopupMenuE
    TabOrder = 0
  end
  object CanButton: TButton
    Left = 215
    Top = 287
    Width = 80
    Height = 26
    Cancel = True
    Caption = #12461#12515#12531#12475#12523
    ModalResult = 2
    TabOrder = 6
  end
  object FormatRadioGroup: TRadioGroup
    Left = 8
    Top = 74
    Width = 398
    Height = 42
    Anchors = [akLeft, akTop, akRight]
    Caption = #12501#12457#12540#12510#12483#12488'(&F)'
    Columns = 5
    Items.Strings = (
      'ZIP'
      '7z'
      'LHA'
      'CAB'
      'TAR')
    TabOrder = 3
    OnClick = FormatRadioGroupClick
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 128
    Width = 398
    Height = 143
    Anchors = [akLeft, akTop, akRight]
    Caption = #12458#12503#12471#12519#12531'(&O)'
    TabOrder = 4
    DesignSize = (
      398
      143)
    object Label1: TLabel
      Left = 44
      Top = 21
      Width = 48
      Height = 13
      Alignment = taRightJustify
      Anchors = [akTop, akRight]
      Caption = #22311#32302#24418#24335
    end
    object PasswordEdit: TLabeledEdit
      Left = 98
      Top = 109
      Width = 289
      Height = 21
      EditLabel.Width = 47
      EditLabel.Height = 13
      EditLabel.Caption = #12497#12473#12527#12540#12489
      LabelPosition = lpLeft
      PopupMenu = UserModule.EditPopupMenuE
      TabOrder = 3
      StyleElements = [seBorder]
    end
    object ParamComboBox: TComboBox
      Left = 98
      Top = 18
      Width = 289
      Height = 21
      Style = csDropDownList
      TabOrder = 0
    end
    object SfxCheckBox: TCheckBox
      Left = 98
      Top = 80
      Width = 113
      Height = 17
      Caption = #33258#24049#35299#20941
      TabOrder = 2
      OnClick = SfxCheckBoxClick
    end
    object ExSwEdit: TLabeledEdit
      Left = 98
      Top = 47
      Width = 289
      Height = 21
      EditLabel.Width = 58
      EditLabel.Height = 13
      EditLabel.Caption = #36861#21152#12473#12452#12483#12481
      LabelPosition = lpLeft
      PopupMenu = UserModule.EditPopupMenuE
      TabOrder = 1
      StyleElements = [seBorder]
    end
  end
  object PerDirCheckBox: TCheckBox
    Left = 21
    Top = 41
    Width = 185
    Height = 17
    Caption = #12487#12451#12524#12463#12488#12522#27598#12395#20491#21029#20316#25104'(&I)'
    TabOrder = 1
    OnClick = PerDirCheckBoxClick
  end
  object IncDirCheckBox: TCheckBox
    Left = 212
    Top = 41
    Width = 181
    Height = 17
    Caption = #23550#35937#12487#12451#12524#12463#12488#12522#12418#21547#12417#12427'(&D)'
    TabOrder = 2
    OnClick = PerDirCheckBoxClick
  end
end
