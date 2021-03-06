object RenameDlg: TRenameDlg
  Left = 0
  Top = 0
  HelpContext = 51
  BorderIcons = [biSystemMenu]
  Caption = #21517#21069#12398#22793#26356
  ClientHeight = 427
  ClientWidth = 542
  Color = clBtnFace
  Constraints.MinHeight = 460
  Constraints.MinWidth = 550
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
  object StatusBar1: TStatusBar
    Left = 0
    Top = 407
    Width = 542
    Height = 20
    Panels = <
      item
        Style = psOwnerDraw
        Width = 160
      end
      item
        Width = 200
      end
      item
        Width = 50
      end>
    StyleElements = [seClient, seBorder]
    OnDrawPanel = StatusBar1DrawPanel
  end
  object MainPanel: TPanel
    Left = 0
    Top = 0
    Width = 542
    Height = 407
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object CommonPanel: TPanel
      Left = 0
      Top = 256
      Width = 542
      Height = 151
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        542
        151)
      object AtrGroupBox: TGroupBox
        Left = 8
        Top = 7
        Width = 524
        Height = 42
        Anchors = [akLeft, akBottom]
        Caption = #23646#24615
        TabOrder = 0
        object ReadOnlyPanel: TPanel
          Left = 12
          Top = 18
          Width = 126
          Height = 17
          AutoSize = True
          BevelOuter = bvNone
          ParentBackground = False
          ShowCaption = False
          TabOrder = 0
          StyleElements = []
          object ReadOnlyCheckBox: TCheckBox
            Left = 0
            Top = 0
            Width = 126
            Height = 17
            Caption = #35501#12415#21462#12426#23554#29992'(&R)'
            TabOrder = 0
            OnClick = AttrCheckBoxClick
          end
        end
        object HiddenPanel: TPanel
          Left = 144
          Top = 18
          Width = 121
          Height = 17
          AutoSize = True
          BevelOuter = bvNone
          ParentBackground = False
          ShowCaption = False
          TabOrder = 1
          StyleElements = []
          object HiddenCheckBox: TCheckBox
            Left = 0
            Top = 0
            Width = 121
            Height = 17
            Caption = #38560#12375#12501#12449#12452#12523'(&H)'
            TabOrder = 0
            OnClick = AttrCheckBoxClick
          end
        end
        object SysPanel: TPanel
          Left = 272
          Top = 18
          Width = 102
          Height = 17
          AutoSize = True
          BevelOuter = bvNone
          ParentBackground = False
          ShowCaption = False
          TabOrder = 2
          StyleElements = [seBorder]
          object SysCheckBox: TCheckBox
            Left = 0
            Top = 0
            Width = 102
            Height = 17
            Caption = #12471#12473#12486#12512'(&S)'
            TabOrder = 0
            OnClick = AttrCheckBoxClick
          end
        end
        object ArcPanel: TPanel
          Left = 380
          Top = 18
          Width = 111
          Height = 17
          AutoSize = True
          BevelOuter = bvNone
          ParentBackground = False
          ShowCaption = False
          TabOrder = 3
          StyleElements = []
          object ArcCheckBox: TCheckBox
            Left = 0
            Top = 0
            Width = 111
            Height = 17
            Caption = #12450#12540#12459#12452#12502'(&A)'
            TabOrder = 0
            OnClick = AttrCheckBoxClick
          end
        end
      end
      object TimeGroupBox: TGroupBox
        Left = 8
        Top = 55
        Width = 524
        Height = 51
        Anchors = [akLeft, akBottom]
        Caption = #12479#12452#12512#12473#12479#12531#12503'(&T)'
        TabOrder = 1
        object TimeMaskEdit: TMaskEdit
          Left = 12
          Top = 20
          Width = 162
          Height = 21
          EditMask = '!9999/99/99 !99:99:99;1;_'
          MaxLength = 19
          PopupMenu = UserModule.EditPopupMenuE
          TabOrder = 0
          Text = '    /  /     :  :  '
          StyleElements = [seBorder]
          OnChange = TimeMaskEditChange
        end
        object TimeNowBtn: TButton
          Left = 180
          Top = 19
          Width = 60
          Height = 24
          Caption = #29694#22312'(&C)'
          TabOrder = 1
          OnClick = TimeNowBtnClick
        end
        object Time00Btn: TButton
          Left = 324
          Top = 19
          Width = 60
          Height = 24
          Caption = '&00:00'
          TabOrder = 3
          OnClick = Time00BtnClick
        end
        object Time12Btn: TButton
          Left = 388
          Top = 19
          Width = 60
          Height = 24
          Caption = '&12:00'
          TabOrder = 4
          OnClick = Time12BtnClick
        end
        object TimeX0Btn: TButton
          Left = 452
          Top = 19
          Width = 60
          Height = 24
          Caption = '&XX:00'
          TabOrder = 5
          OnClick = TimeX0BtnClick
        end
        object TimeLatestBtn: TButton
          Left = 244
          Top = 19
          Width = 60
          Height = 24
          Action = LatestTimeAction
          TabOrder = 2
        end
      end
      object OkButton: TButton
        Left = 352
        Top = 118
        Width = 80
        Height = 26
        Action = RenOkAction
        Anchors = [akRight, akBottom]
        Default = True
        TabOrder = 4
      end
      object CanButton: TButton
        Left = 440
        Top = 118
        Width = 80
        Height = 26
        Anchors = [akRight, akBottom]
        Cancel = True
        Caption = #12461#12515#12531#12475#12523
        ModalResult = 2
        TabOrder = 5
      end
      object AutoPrvCheckBox: TCheckBox
        Left = 136
        Top = 122
        Width = 126
        Height = 17
        Anchors = [akTop, akRight]
        Caption = #33258#21205#12503#12524#12499#12517#12540
        TabOrder = 2
        OnClick = AutoPrvCheckBoxClick
      end
      object PreviewBtn: TButton
        Left = 264
        Top = 118
        Width = 80
        Height = 26
        Anchors = [akTop, akRight]
        Caption = #12503#12524#12499#12517#12540
        TabOrder = 3
        OnClick = PreviewBtnClick
      end
    end
    object NamePageControl: TPageControl
      Left = 0
      Top = 0
      Width = 542
      Height = 256
      ActivePage = NameSheet
      Align = alClient
      TabOrder = 0
      TabWidth = 90
      OnChange = NamePageControlChange
      object NameSheet: TTabSheet
        Caption = #21517#21069'(&N)'
        object NameComPanel: TPanel
          Left = 0
          Top = 40
          Width = 534
          Height = 188
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 1
          DesignSize = (
            534
            188)
          object NameInfMLabel: TLabel
            Left = 212
            Top = 168
            Width = 160
            Height = 18
            Anchors = [akLeft, akBottom]
            AutoSize = False
            Caption = #21517#21069#12398#26368#22823#25991#23383#25968' = '
          end
          object PathInfMLabel: TLabel
            Left = 13
            Top = 168
            Width = 195
            Height = 18
            Anchors = [akLeft, akBottom]
            AutoSize = False
            Caption = #12501#12523#12497#12473#21517#12398#26368#22823#25991#23383#25968' = '
          end
          object SttInfLabel: TLabel
            Left = 378
            Top = 168
            Width = 146
            Height = 18
            Alignment = taRightJustify
            Anchors = [akLeft, akRight, akBottom]
            AutoSize = False
          end
          object PreviewGrid: TStringGrid
            Left = 10
            Top = 74
            Width = 514
            Height = 88
            Anchors = [akLeft, akTop, akRight, akBottom]
            ColCount = 3
            DefaultColWidth = 100
            DefaultRowHeight = 20
            DefaultDrawing = False
            FixedCols = 0
            RowCount = 1
            FixedRows = 0
            Options = [goFixedVertLine, goFixedHorzLine, goColSizing, goRowSelect, goThumbTracking]
            ScrollBars = ssVertical
            TabOrder = 4
            StyleElements = [seBorder]
            OnDrawCell = PreviewGridDrawCell
            OnKeyDown = PreviewGridKeyDown
            ColWidths = (
              100
              100
              100)
            RowHeights = (
              20)
          end
          object FbaseRadioGroup: TRadioGroup
            Left = 10
            Top = 2
            Width = 364
            Height = 42
            Caption = #20027#37096'(&M)'
            Columns = 6
            Items.Strings = (
              #20803
              #22823
              #23567
              #38957#22823
              #20840#35282
              #21322#35282)
            TabOrder = 0
            OnClick = FModRadioGroupClick
          end
          object FextRadioGroup: TRadioGroup
            Left = 382
            Top = 2
            Width = 142
            Height = 42
            Caption = #25313#24373#23376'(&E)'
            Columns = 3
            Items.Strings = (
              #20803
              #22823
              #23567)
            TabOrder = 1
            OnClick = FModRadioGroupClick
          end
          object CnvCharCheckBox: TCheckBox
            Left = 16
            Top = 51
            Width = 308
            Height = 17
            Caption = #20027#37096#12398#31105#27490#25991#23383#65295#12518#12540#12470#23450#32681#25991#23383#12434#32622#25563'(&C)'
            TabOrder = 2
            OnClick = FModRadioGroupClick
          end
          object CnvKanaCheckBox: TCheckBox
            Left = 331
            Top = 51
            Width = 185
            Height = 17
            Caption = #12459#12479#12459#12490#12418#21322#35282#12395#22793#25563'(&K)'
            TabOrder = 3
            OnClick = FModRadioGroupClick
          end
        end
        object NameTopPanel: TPanel
          Left = 0
          Top = 0
          Width = 534
          Height = 40
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          DesignSize = (
            534
            40)
          object RenameEdit: TEdit
            Left = 10
            Top = 8
            Width = 514
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            MaxLength = 255
            PopupMenu = UserModule.EditPopupMenuE
            TabOrder = 0
            OnChange = RenameEditChange
            OnKeyDown = RenameEditKeyDown
            OnKeyPress = RenameEditKeyPress
          end
        end
      end
      object SerialSheet: TTabSheet
        Caption = #36899#30058'(&B)'
        ImageIndex = 1
        object SerialTopPanel: TPanel
          Left = 0
          Top = 0
          Width = 534
          Height = 66
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          DesignSize = (
            534
            66)
          object Fmt_A_Btn: TSpeedButton
            Tag = 3
            Left = 170
            Top = 38
            Width = 50
            Height = 22
            Hint = #12501#12449#12452#12523#21517#20027#37096#12398#20840#20307
            Caption = #20027#37096
            OnClick = Fmt_A_BtnClick
          end
          object Fmt_E_Btn: TSpeedButton
            Tag = 4
            Left = 222
            Top = 38
            Width = 50
            Height = 22
            Hint = #25313#24373#23376'( . '#12399#38500#12367')'
            Caption = #25313#24373#23376
            OnClick = Fmt_E_BtnClick
          end
          object Fmt_L_Btn: TSpeedButton
            Left = 8
            Top = 38
            Width = 50
            Height = 22
            Hint = #12501#12449#12452#12523#21517#20027#37096#12398#24038#12363#12425'n'#25991#23383
            Caption = #24038'n'#23383
            OnClick = Fmt_L_BtnClick
          end
          object Fmt_R_Btn: TSpeedButton
            Tag = 2
            Left = 112
            Top = 38
            Width = 50
            Height = 22
            Hint = #12501#12449#12452#12523#21517#20027#37096#12398#21491#12363#12425'n'#25991#23383
            Caption = #21491'n'#23383
            OnClick = Fmt_R_BtnClick
          end
          object Fmt_S_Btn: TSpeedButton
            Tag = 1
            Left = 60
            Top = 38
            Width = 50
            Height = 22
            Hint = #12501#12449#12452#12523#21517#20027#37096#12398'm'#30058#30446#12363#12425'n'#25991#23383
            Caption = 'm..n'#23383
            OnClick = Fmt_S_BtnClick
          end
          object Fmt_TS_Btn: TSpeedButton
            Tag = 4
            Left = 280
            Top = 38
            Width = 45
            Height = 22
            Hint = #12479#12452#12512#12473#12479#12531#12503
            Caption = #26085#26178
            OnClick = Fmt_TS_BtnClick
          end
          object Fmt_XT_Btn: TSpeedButton
            Tag = 2
            Left = 327
            Top = 38
            Width = 45
            Height = 22
            Hint = 'Exif '#24773#22577#12398#25774#24433#26085#26178
            Caption = 'Exif'
            OnClick = Fmt_XT_BtnClick
          end
          object SaveFmtBtn: TSpeedButton
            Tag = 2
            Left = 384
            Top = 38
            Width = 40
            Height = 22
            Hint = #29694#22312#12398#26360#24335#12434#20445#23384
            Action = SaveSerFmtAction
          end
          object SerEditPanel: TPanel
            Left = 0
            Top = 0
            Width = 534
            Height = 34
            Align = alTop
            BevelOuter = bvNone
            TabOrder = 0
            object SerSplitter: TSplitter
              Left = 280
              Top = 0
              Width = 5
              Height = 34
              AutoSnap = False
              Beveled = True
              MinSize = 200
            end
            object SerLeftPanel: TPanel
              Left = 0
              Top = 0
              Width = 280
              Height = 34
              Align = alLeft
              BevelOuter = bvNone
              TabOrder = 0
              DesignSize = (
                280
                34)
              object PreNameEdit: TEdit
                Tag = 10
                Left = 9
                Top = 8
                Width = 174
                Height = 21
                Hint = #21069#37096#20998
                Anchors = [akLeft, akTop, akRight]
                PopupMenu = UserModule.EditPopupMenuE
                TabOrder = 0
                OnChange = RenameEditChange
                OnExit = RenameEditExit
              end
              object SerNoEdit: TEdit
                Tag = 11
                Left = 190
                Top = 8
                Width = 50
                Height = 21
                Hint = #36899#30058
                Alignment = taRightJustify
                Anchors = [akTop, akRight]
                NumbersOnly = True
                PopupMenu = UserModule.EditPopupMenuE
                TabOrder = 1
                OnChange = RenameEditChange
                OnExit = RenameEditExit
              end
              object IncNoEdit: TEdit
                Tag = 12
                Left = 242
                Top = 8
                Width = 34
                Height = 21
                Hint = #22679#20998
                Alignment = taRightJustify
                Anchors = [akTop, akRight]
                NumbersOnly = True
                PopupMenu = UserModule.EditPopupMenuE
                TabOrder = 2
                OnChange = RenameEditChange
                OnExit = RenameEditExit
              end
            end
            object SerRightPanel: TPanel
              Left = 285
              Top = 0
              Width = 249
              Height = 34
              Align = alClient
              BevelOuter = bvNone
              TabOrder = 1
              DesignSize = (
                249
                34)
              object DotLabel: TLabel
                Left = 161
                Top = 11
                Width = 4
                Height = 13
                Anchors = [akTop, akRight]
                Caption = '.'
              end
              object PostNameEdit: TEdit
                Tag = 13
                Left = 4
                Top = 8
                Width = 151
                Height = 21
                Hint = #24460#37096#20998
                Anchors = [akLeft, akTop, akRight]
                PopupMenu = UserModule.EditPopupMenuE
                TabOrder = 0
                OnChange = RenameEditChange
                OnExit = RenameEditExit
              end
              object ExtEdit: TEdit
                Tag = 14
                Left = 172
                Top = 8
                Width = 68
                Height = 21
                Hint = #25313#24373#23376
                Anchors = [akTop, akRight]
                PopupMenu = UserModule.EditPopupMenuE
                TabOrder = 1
                OnChange = RenameEditChange
                OnExit = RenameEditExit
              end
            end
          end
          object SerFmtComboBox: TComboBox
            Left = 426
            Top = 38
            Width = 99
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            PopupMenu = FmtPopupMenu
            TabOrder = 1
            OnClick = SerFmtComboBoxClick
          end
        end
      end
      object ReplaceSheet: TTabSheet
        Caption = #32622#25563'(&P)'
        ImageIndex = 2
        object ReplaceTopPanel: TPanel
          Left = 0
          Top = 0
          Width = 534
          Height = 85
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          DesignSize = (
            534
            85)
          object TLabel
            Left = 15
            Top = 39
            Width = 24
            Height = 13
            Alignment = taRightJustify
            Caption = #32622#25563
          end
          object TLabel
            Left = 15
            Top = 11
            Width = 24
            Height = 13
            Alignment = taRightJustify
            Caption = #26908#32034
          end
          object SrcStrComboBox: TComboBox
            Left = 42
            Top = 8
            Width = 482
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            DropDownCount = 20
            PopupMenu = UserModule.EditPopupMenu
            TabOrder = 0
            OnChange = ReplaceComboChange
            OnEnter = SrcStrComboBoxEnter
            OnExit = SrcStrComboBoxExit
          end
          object RepStrComboBox: TComboBox
            Left = 42
            Top = 34
            Width = 482
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            DropDownCount = 20
            PopupMenu = UserModule.EditPopupMenu
            TabOrder = 1
            OnChange = ReplaceComboChange
          end
          object OnlyBaseCheckBox: TCheckBox
            Left = 240
            Top = 61
            Width = 160
            Height = 17
            Caption = #25313#24373#23376#12399#32622#25563#12375#12394#12356
            TabOrder = 4
            OnClick = ReplaceComboChange
          end
          object RegExCheckBox: TCheckBox
            Left = 139
            Top = 61
            Width = 88
            Height = 17
            Caption = #27491#35215#34920#29694
            TabOrder = 3
            OnClick = RegExCheckBoxClick
          end
          object CaseCheckBox: TCheckBox
            Left = 42
            Top = 61
            Width = 88
            Height = 17
            Caption = #22823#23567#21306#21029
            TabOrder = 2
            OnClick = ReplaceComboChange
          end
        end
      end
      object Mp3Sheet: TTabSheet
        Caption = 'MP3(&M)'
        ImageIndex = 4
        object Mp3TopPanel: TPanel
          Left = 0
          Top = 0
          Width = 534
          Height = 65
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          DesignSize = (
            534
            65)
          object Id3Btn5: TSpeedButton
            Tag = 4
            Left = 361
            Top = 34
            Width = 75
            Height = 22
            Caption = #12522#12522#12540#12473#24180
            OnClick = RefFmtBtnClick
          end
          object Id3Btn4: TSpeedButton
            Tag = 3
            Left = 283
            Top = 34
            Width = 75
            Height = 22
            Caption = #12488#12521#12483#12463'No.'
            OnClick = RefFmtBtnClick
          end
          object mp3fextLabel: TLabel
            Left = 488
            Top = 11
            Width = 28
            Height = 13
            Anchors = [akTop, akRight]
            Caption = '.mp3'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #65325#65331' '#12468#12471#12483#12463
            Font.Style = []
            ParentFont = False
          end
          object Id3Btn3: TSpeedButton
            Tag = 2
            Left = 205
            Top = 34
            Width = 75
            Height = 22
            Caption = #12488#12521#12483#12463#21517
            OnClick = RefFmtBtnClick
          end
          object TLabel
            Left = 11
            Top = 12
            Width = 24
            Height = 13
            Caption = #26360#24335
          end
          object Id3Btn2: TSpeedButton
            Tag = 1
            Left = 127
            Top = 34
            Width = 75
            Height = 22
            Caption = #12450#12523#12496#12512#21517
            OnClick = RefFmtBtnClick
          end
          object Id3Btn1: TSpeedButton
            Left = 49
            Top = 34
            Width = 75
            Height = 22
            Caption = #12450#12540#12486#12451#12473#12488
            OnClick = RefFmtBtnClick
          end
          object Mp3FmtComboBox: TComboBox
            Left = 42
            Top = 8
            Width = 443
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            DropDownCount = 20
            PopupMenu = FmtPopupMenu
            TabOrder = 0
            OnChange = Mp3FmtComboBoxChange
            OnEnter = SrcStrComboBoxEnter
            OnExit = SrcStrComboBoxExit
          end
        end
      end
      object OptionSheet: TTabSheet
        Caption = #35373#23450
        ImageIndex = 3
        object PageControl1: TPageControl
          Left = 0
          Top = 0
          Width = 534
          Height = 228
          ActivePage = AssRenSheet
          Align = alClient
          TabOrder = 0
          TabWidth = 180
          object AssRenSheet: TTabSheet
            Caption = #38306#36899#12501#12449#12452#12523#12398#21516#26178#25913#21517
            object Opt1BtmPanel: TPanel
              Left = 0
              Top = 138
              Width = 526
              Height = 62
              Align = alBottom
              BevelOuter = bvNone
              TabOrder = 1
              DesignSize = (
                526
                62)
              object DelAssRenBtn: TButton
                Left = 375
                Top = 34
                Width = 60
                Height = 24
                Action = DelAssRenAction
                Anchors = [akRight, akBottom]
                TabOrder = 4
              end
              object ChgAssRenBtn: TButton
                Left = 311
                Top = 34
                Width = 60
                Height = 24
                Action = ChgAssRenAction
                Anchors = [akRight, akBottom]
                TabOrder = 3
              end
              object AddAssRenBtn: TButton
                Left = 247
                Top = 34
                Width = 60
                Height = 24
                Action = AddAssRenAction
                Anchors = [akRight, akBottom]
                TabOrder = 2
              end
              object AssExtEdit: TLabeledEdit
                Left = 70
                Top = 8
                Width = 445
                Height = 21
                Hint = '. '#12391#21306#20999#12387#12390#35079#25968#25351#23450#12375#12390#12367#12384#12373#12356
                Anchors = [akLeft, akRight, akBottom]
                EditLabel.Width = 36
                EditLabel.Height = 13
                EditLabel.Caption = #25313#24373#23376
                LabelPosition = lpLeft
                PopupMenu = UserModule.EditPopupMenuE
                TabOrder = 0
              end
              object DowColBtn: TButton
                Left = 485
                Top = 34
                Width = 30
                Height = 24
                Action = UserModule.DownListItemAction
                Anchors = [akRight, akBottom]
                TabOrder = 6
              end
              object UpColBtn: TButton
                Left = 451
                Top = 34
                Width = 30
                Height = 24
                Action = UserModule.UpListItemAction
                Anchors = [akRight, akBottom]
                TabOrder = 5
              end
              object PrtChgCheckBox: TCheckBox
                Left = 32
                Top = 41
                Width = 193
                Height = 17
                Anchors = [akLeft, akBottom]
                Caption = #37096#20998#19968#33268#12501#12449#12452#12523#12418#25913#21517
                TabOrder = 1
              end
            end
            object Opt1MainPanel: TPanel
              Left = 0
              Top = 0
              Width = 526
              Height = 138
              Align = alClient
              BevelOuter = bvNone
              TabOrder = 0
              DesignSize = (
                526
                138)
              object AssRenListBox: TCheckListBox
                Left = 8
                Top = 4
                Width = 507
                Height = 128
                OnClickCheck = AssRenListBoxClickCheck
                Anchors = [akLeft, akTop, akRight, akBottom]
                DragMode = dmAutomatic
                ItemHeight = 13
                Style = lbOwnerDrawFixed
                TabOrder = 0
                OnClick = AssRenListBoxClick
                OnDrawItem = AssRenListBoxDrawItem
                OnKeyDown = AssRenListBoxKeyDown
              end
            end
          end
          object OtherSheet: TTabSheet
            Caption = #12381#12398#20182
            ImageIndex = 1
            object Opt2MainPanel: TPanel
              Left = 0
              Top = 0
              Width = 526
              Height = 200
              Align = alClient
              BevelOuter = bvNone
              TabOrder = 0
              DesignSize = (
                526
                200)
              object CnvCharGroupBox: TGroupBox
                Left = 8
                Top = 8
                Width = 241
                Height = 199
                Anchors = [akLeft, akTop, akBottom]
                Caption = #12501#12449#12452#12523#21517#20027#37096#12398#25991#23383#32622#25563
                TabOrder = 0
                DesignSize = (
                  241
                  199)
                object TLabel
                  Left = 184
                  Top = 23
                  Width = 11
                  Height = 13
                  Caption = #8594
                end
                object CnvCharListBox: TListBox
                  Left = 10
                  Top = 20
                  Width = 129
                  Height = 169
                  Style = lbOwnerDrawFixed
                  Anchors = [akLeft, akTop, akBottom]
                  Font.Charset = ANSI_CHARSET
                  Font.Color = clWindowText
                  Font.Height = -13
                  Font.Name = #65325#65331' '#12468#12471#12483#12463
                  Font.Style = []
                  ItemHeight = 13
                  ParentFont = False
                  TabOrder = 0
                  OnClick = CnvCharListBoxClick
                  OnDrawItem = CnvCharListBoxDrawItem
                end
                object AddKeyBtn: TButton
                  Left = 155
                  Top = 57
                  Width = 70
                  Height = 22
                  Action = AddCnvChAction
                  TabOrder = 3
                end
                object ChgKeyBtn: TButton
                  Left = 155
                  Top = 83
                  Width = 70
                  Height = 22
                  Action = ChgCnvChAction
                  TabOrder = 4
                end
                object DelKeyBtn: TButton
                  Left = 155
                  Top = 109
                  Width = 70
                  Height = 22
                  Action = DelCnvChAction
                  TabOrder = 5
                end
                object CnvChSEdit: TEdit
                  Left = 150
                  Top = 20
                  Width = 30
                  Height = 21
                  TabOrder = 1
                end
                object CnvChREdit: TEdit
                  Left = 201
                  Top = 20
                  Width = 30
                  Height = 21
                  TabOrder = 2
                end
              end
              object NoRenLogCheckBox: TCheckBox
                Left = 269
                Top = 156
                Width = 184
                Height = 17
                Caption = #25913#21517#12525#12464#12434#20316#25104#12375#12394#12356
                TabOrder = 2
              end
              object IniSttGroupBox: TGroupBox
                Left = 261
                Top = 8
                Width = 260
                Height = 136
                Caption = #21517#21069#20837#21147#27396#12398#21021#26399#29366#24907
                TabOrder = 1
                object TLabel
                  Left = 15
                  Top = 107
                  Width = 36
                  Height = 13
                  Alignment = taRightJustify
                  Caption = #27425#20505#35036
                end
                object IniSttComboBox: TComboBox
                  Left = 8
                  Top = 22
                  Width = 245
                  Height = 21
                  Style = csDropDownList
                  TabOrder = 0
                  OnClick = IniSttComboBoxClick
                end
                object IniSttWdEdit: TLabeledEdit
                  Left = 55
                  Top = 49
                  Width = 198
                  Height = 21
                  EditLabel.Width = 36
                  EditLabel.Height = 13
                  EditLabel.Caption = #25351#23450#35486
                  LabelPosition = lpLeft
                  TabOrder = 1
                end
                object IniStt2ComboBox: TComboBox
                  Left = 55
                  Top = 104
                  Width = 198
                  Height = 21
                  Style = csDropDownList
                  TabOrder = 3
                  OnClick = IniSttComboBoxClick
                end
                object EndMatchCheckBox: TCheckBox
                  Left = 55
                  Top = 76
                  Width = 169
                  Height = 17
                  Caption = #26368#24460#12398#35486#12395#12510#12483#12481
                  TabOrder = 2
                end
              end
            end
          end
        end
      end
    end
  end
  object ActionList1: TActionList
    Left = 352
    Top = 150
    object AddCnvChAction: TAction
      Caption = #36861#21152
      OnExecute = AddCnvChActionExecute
      OnUpdate = ChgCnvChActionUpdate
    end
    object ChgCnvChAction: TAction
      Caption = #22793#26356
      OnExecute = ChgCnvChActionExecute
      OnUpdate = ChgCnvChActionUpdate
    end
    object DelCnvChAction: TAction
      Caption = #21066#38500
      OnExecute = DelCnvChActionExecute
      OnUpdate = DelCnvChActionUpdate
    end
    object RenOkAction: TAction
      Caption = 'OK'
      OnExecute = RenOkActionExecute
      OnUpdate = RenOkActionUpdate
    end
    object SaveSerFmtAction: TAction
      Caption = #20445#23384
      OnExecute = SaveSerFmtActionExecute
      OnUpdate = SaveSerFmtActionUpdate
    end
    object DelItemAction: TAction
      Caption = #12371#12398#38917#30446#12434#21066#38500'(&D)'
      OnExecute = DelItemActionExecute
      OnUpdate = DelItemActionUpdate
    end
    object Inp1stNameAction: TAction
      OnExecute = Inp1stNameActionExecute
      OnUpdate = Inp1stNameActionUpdate
    end
    object AddAssRenAction: TAction
      Caption = #30331#37682
      OnExecute = AddAssRenActionExecute
      OnUpdate = AddAssRenActionUpdate
    end
    object ChgAssRenAction: TAction
      Caption = #22793#26356
      OnExecute = ChgAssRenActionExecute
      OnUpdate = ChgAssRenActionUpdate
    end
    object DelAssRenAction: TAction
      Caption = #21066#38500
      OnExecute = DelAssRenActionExecute
      OnUpdate = DelAssRenActionUpdate
    end
    object LatestTimeAction: TAction
      Caption = #26368#26032'(&L)'
      OnExecute = LatestTimeActionExecute
      OnUpdate = LatestTimeActionUpdate
    end
  end
  object FmtPopupMenu: TPopupMenu
    AutoHotkeys = maManual
    Left = 438
    Top = 150
    object DelItemItem: TMenuItem
      Action = DelItemAction
    end
  end
end
