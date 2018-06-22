object FindTagForm: TFindTagForm
  Left = 0
  Top = 0
  HelpContext = 83
  BorderIcons = [biSystemMenu]
  ClientHeight = 281
  ClientWidth = 264
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListPanel: TPanel
    Left = 0
    Top = 0
    Width = 264
    Height = 281
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object InpPanel: TPanel
      Left = 0
      Top = 0
      Width = 264
      Height = 22
      Align = alTop
      Alignment = taLeftJustify
      BevelOuter = bvLowered
      TabOrder = 0
      StyleElements = [seBorder]
      object InpPaintBox: TPaintBox
        Left = 1
        Top = 1
        Width = 262
        Height = 20
        Align = alClient
        OnPaint = InpPaintBoxPaint
      end
    end
    object ResListBox: TListBox
      Left = 0
      Top = 22
      Width = 264
      Height = 259
      Style = lbVirtualOwnerDraw
      Align = alLeft
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      DoubleBuffered = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 13
      MultiSelect = True
      ParentDoubleBuffered = False
      ParentFont = False
      PopupMenu = PopupMenu1
      TabOrder = 1
      StyleElements = []
      OnData = ResListBoxData
      OnDblClick = ResListBoxDblClick
      OnDrawItem = ResListBoxDrawItem
      OnKeyDown = ResListBoxKeyDown
      OnKeyPress = ResListBoxKeyPress
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 48
    Top = 112
    object TagViewItem: TMenuItem
      Action = TagViewAction
    end
    object TagJumpItem: TMenuItem
      Action = TagJumpAction
    end
    object Sep_1: TMenuItem
      Caption = '-'
    end
    object CopyItem: TMenuItem
      Action = CopyAction
    end
    object SelAllItem: TMenuItem
      Action = SelAllAction
      ShortCut = 16449
    end
  end
  object ActionList1: TActionList
    Left = 48
    Top = 48
    object TagViewAction: TAction
      Caption = #12499#12517#12450#12540#12391#12480#12452#12524#12463#12488#12479#12464#12472#12515#12531#12503'(&V)'
      OnExecute = TagViewActionExecute
      OnUpdate = PopupItemUpdate
    end
    object TagJumpAction: TAction
      Caption = #12456#12487#12451#12479#12391#12480#12452#12524#12463#12488#12479#12464#12472#12515#12531#12503'(&E)'
      OnExecute = TagJumpActionExecute
      OnUpdate = PopupItemUpdate
    end
    object CopyAction: TAction
      Caption = #12467#12500#12540'(&C)'
      OnExecute = CopyActionExecute
      OnUpdate = PopupItemUpdate
    end
    object SelAllAction: TAction
      Caption = #12377#12409#12390#36984#25246'(&A)'
      OnExecute = SelAllActionExecute
      OnUpdate = PopupItemUpdate
    end
  end
end
