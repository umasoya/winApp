object DistributionDlg: TDistributionDlg
  Left = 0
  Top = 0
  HelpContext = 77
  BorderIcons = [biSystemMenu]
  Caption = #25391#12426#20998#12369
  ClientHeight = 371
  ClientWidth = 624
  Color = clBtnFace
  Constraints.MinHeight = 400
  Constraints.MinWidth = 640
  ParentFont = True
  KeyPreview = True
  OldCreateOrder = False
  PopupMenu = PopupMenu1
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
  object PrvSplitter: TSplitter
    Left = 0
    Top = 250
    Width = 624
    Height = 5
    Cursor = crVSplit
    Align = alTop
    AutoSnap = False
  end
  object MainPanel: TPanel
    Left = 0
    Top = 0
    Width = 624
    Height = 250
    Align = alTop
    BevelOuter = bvNone
    Constraints.MinHeight = 220
    TabOrder = 0
    object RegListBox: TCheckListBox
      Left = 0
      Top = 0
      Width = 624
      Height = 88
      OnClickCheck = RegListBoxClickCheck
      Align = alClient
      BorderStyle = bsNone
      DoubleBuffered = False
      DragMode = dmAutomatic
      ItemHeight = 13
      ParentDoubleBuffered = False
      Style = lbOwnerDrawFixed
      TabOrder = 0
      OnClick = RegListBoxClick
      OnDrawItem = RegListBoxDrawItem
      OnKeyDown = RegListBoxKeyDown
    end
    object HideCanBtn: TButton
      Left = 624
      Top = 0
      Width = 0
      Height = 88
      Align = alRight
      Cancel = True
      ModalResult = 2
      TabOrder = 2
      TabStop = False
    end
    object RegPanel: TPanel
      Left = 0
      Top = 88
      Width = 624
      Height = 118
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        624
        118)
      object FindDownBtn: TSpeedButton
        Left = 570
        Top = 90
        Width = 23
        Height = 22
        Hint = #19979#26041#21521#12395#26908#32034
        Action = FindDownAction
        Anchors = [akRight, akBottom]
      end
      object FindUpBtn: TSpeedButton
        Left = 595
        Top = 90
        Width = 23
        Height = 22
        Hint = #19978#26041#21521#12395#26908#32034
        Action = FindUpAction
        Anchors = [akRight, akBottom]
      end
      object CheckBtn: TButton
        Left = 8
        Top = 4
        Width = 30
        Height = 24
        Anchors = [akLeft, akBottom]
        Caption = #10003
        TabOrder = 0
        OnClick = CheckBtnClick
      end
      object ChgItemBtn: TButton
        Left = 65
        Top = 88
        Width = 55
        Height = 24
        Action = ChgRegAction
        Anchors = [akLeft, akBottom]
        TabOrder = 7
      end
      object DelItemBtn: TButton
        Left = 122
        Top = 88
        Width = 55
        Height = 24
        Action = DelRegAction
        Anchors = [akLeft, akBottom]
        TabOrder = 8
      end
      object DowItemBtn: TButton
        Left = 215
        Top = 88
        Width = 30
        Height = 24
        Action = UserModule.DownListItemAction
        Anchors = [akLeft, akBottom]
        TabOrder = 10
      end
      object UpItemBtn: TButton
        Left = 183
        Top = 88
        Width = 30
        Height = 24
        Action = UserModule.UpListItemAction
        Anchors = [akLeft, akBottom]
        TabOrder = 9
      end
      object TitleEdit: TLabeledEdit
        Left = 96
        Top = 6
        Width = 522
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        BevelEdges = [beLeft, beTop, beRight]
        EditLabel.Width = 35
        EditLabel.Height = 13
        EditLabel.Caption = #12479#12452#12488#12523
        LabelPosition = lpLeft
        PopupMenu = UserModule.EditPopupMenuE
        TabOrder = 1
      end
      object DistrDirEdit: TLabeledEdit
        Left = 96
        Top = 60
        Width = 484
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        BevelEdges = [beLeft, beTop, beRight]
        EditLabel.Width = 54
        EditLabel.Height = 13
        EditLabel.Caption = #25391#12426#20998#12369#20808
        LabelPosition = lpLeft
        PopupMenu = UserModule.EditPopupMenuE
        TabOrder = 4
      end
      object DistrMaskEdit: TLabeledEdit
        Left = 96
        Top = 33
        Width = 484
        Height = 21
        Hint = '/ '#65374' / '#12391#22258#12416#12392#27491#35215#34920#29694#12497#12479#12540#12531
        Anchors = [akLeft, akTop, akRight]
        BevelEdges = [beLeft, beTop, beRight]
        EditLabel.Width = 67
        EditLabel.Height = 13
        EditLabel.Caption = #12510#12473#12463'/'#12497#12479#12540#12531
        LabelPosition = lpLeft
        PopupMenu = UserModule.EditPopupMenuE
        TabOrder = 2
      end
      object RefDirButton: TButton
        Left = 582
        Top = 60
        Width = 36
        Height = 22
        Anchors = [akTop, akRight]
        Caption = '...'
        TabOrder = 5
        OnClick = RefDirButtonClick
      end
      object AddItemBtn: TButton
        Left = 8
        Top = 88
        Width = 55
        Height = 24
        Action = AddRegAction
        Anchors = [akLeft, akBottom]
        TabOrder = 6
      end
      object FindEdit: TLabeledEdit
        Left = 295
        Top = 91
        Width = 272
        Height = 21
        Anchors = [akLeft, akRight, akBottom]
        EditLabel.Width = 24
        EditLabel.Height = 13
        EditLabel.Caption = #26908#32034
        LabelPosition = lpLeft
        TabOrder = 11
        OnChange = FindEditChange
        OnKeyDown = FindEditKeyDown
      end
      object RefListBtn: TButton
        Left = 582
        Top = 33
        Width = 36
        Height = 22
        Hint = #12522#12473#12488#12501#12449#12452#12523#12398#21442#29031
        Anchors = [akTop, akRight]
        Caption = '...'
        TabOrder = 3
        OnClick = RefListBtnClick
      end
    end
    object ExePanel: TPanel
      Left = 0
      Top = 206
      Width = 624
      Height = 44
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 3
      DesignSize = (
        624
        44)
      object Label1: TLabel
        Left = 197
        Top = 15
        Width = 77
        Height = 13
        Alignment = taRightJustify
        Anchors = [akTop, akRight]
        Caption = #21516#21517#26178#12398#20966#29702
      end
      object Bevel1: TBevel
        Left = 0
        Top = 0
        Width = 624
        Height = 4
        Align = alTop
        Shape = bsTopLine
      end
      object MoveBtn: TButton
        Left = 543
        Top = 9
        Width = 75
        Height = 26
        Action = ExeMoveAction
        Anchors = [akTop, akRight]
        Default = True
        TabOrder = 3
      end
      object CopyBtn: TButton
        Left = 460
        Top = 9
        Width = 75
        Height = 26
        Action = ExeCopyAction
        Anchors = [akTop, akRight]
        TabOrder = 2
      end
      object SameNameComboBox: TComboBox
        Left = 278
        Top = 12
        Width = 156
        Height = 21
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 1
      end
      object CreDistrDirCheckBox: TCheckBox
        Left = 8
        Top = 14
        Width = 153
        Height = 17
        Anchors = [akTop, akRight]
        Caption = #25391#12426#20998#12369#20808#12434#33258#21205#20316#25104
        TabOrder = 0
        OnClick = CreDistrDirCheckBoxClick
      end
    end
  end
  object PrvPanel: TPanel
    Left = 0
    Top = 255
    Width = 624
    Height = 96
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object PrvListBox: TListBox
      Left = 0
      Top = 17
      Width = 624
      Height = 79
      Style = lbVirtualOwnerDraw
      Align = alClient
      BorderStyle = bsNone
      ItemHeight = 13
      TabOrder = 1
      OnData = PrvListBoxData
      OnDblClick = PrvListBoxDblClick
      OnDrawItem = PrvListBoxDrawItem
    end
    object PrvListHeader: THeaderControl
      Tag = 1
      Left = 0
      Top = 0
      Width = 624
      Height = 17
      Sections = <
        item
          ImageIndex = -1
          Style = hsOwnerDraw
          Width = 200
        end
        item
          ImageIndex = -1
          Style = hsOwnerDraw
          Width = 200
        end>
      OnDrawSection = PrvListHeaderDrawSection
      OnSectionClick = PrvListHeaderSectionClick
      OnSectionResize = PrvListHeaderSectionResize
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 351
    Width = 624
    Height = 20
    Panels = <
      item
        Width = 120
      end
      item
        Width = 300
      end
      item
        Width = 120
      end
      item
        Width = 50
      end>
  end
  object ActionList1: TActionList
    Left = 32
    Top = 16
    object ExeCopyAction: TAction
      Caption = #12467#12500#12540
      OnExecute = ExeCopyActionExecute
      OnUpdate = ExeActionUpdate
    end
    object ExeMoveAction: TAction
      Caption = #31227#21205
      OnExecute = ExeMoveActionExecute
      OnUpdate = ExeActionUpdate
    end
    object AddRegAction: TAction
      Caption = #30331#37682
      OnExecute = AddRegActionExecute
      OnUpdate = AddRegActionUpdate
    end
    object ChgRegAction: TAction
      Caption = #22793#26356
      OnExecute = ChgRegActionExecute
      OnUpdate = ChgRegActionUpdate
    end
    object DelRegAction: TAction
      Caption = #21066#38500
      OnExecute = DelRegActionExecute
      OnUpdate = ChgRegActionUpdate
    end
    object FindDownAction: TAction
      OnExecute = FindDownActionExecute
      OnUpdate = FindDownActionUpdate
    end
    object FindUpAction: TAction
      OnExecute = FindUpActionExecute
      OnUpdate = FindUpActionUpdate
    end
    object MakeFileAction: TAction
      Caption = #25391#12426#20998#12369#30331#37682#12501#12449#12452#12523#12434#20316#25104'(&M)'
      OnExecute = MakeFileActionExecute
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 152
    Top = 16
    object SaveRegItem: TMenuItem
      Action = MakeFileAction
    end
  end
end
