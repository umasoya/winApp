object LoupeForm: TLoupeForm
  Left = 0
  Top = 0
  HelpContext = 7
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = #12523#12540#12506
  ClientHeight = 186
  ClientWidth = 196
  Color = clBtnFace
  Constraints.MinHeight = 48
  Constraints.MinWidth = 32
  UseDockManager = True
  DoubleBuffered = True
  DragKind = dkDock
  DragMode = dmAutomatic
  ParentFont = True
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poDesigned
  OnHide = FormHide
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ImgPanel: TPanel
    Left = 0
    Top = 0
    Width = 196
    Height = 159
    Align = alClient
    BevelOuter = bvNone
    Color = clBlack
    ParentBackground = False
    TabOrder = 0
    object LoupeImage: TImage
      Left = 0
      Top = 0
      Width = 196
      Height = 159
      Align = alClient
      Center = True
      OnMouseMove = LoupeImageMouseMove
    end
  end
  object OptPanel: TPanel
    Left = 0
    Top = 159
    Width = 196
    Height = 27
    Align = alBottom
    AutoSize = True
    BevelOuter = bvNone
    TabOrder = 1
    OnMouseMove = LoupeImageMouseMove
    object MagToolBar: TToolBar
      Left = 0
      Top = 0
      Width = 196
      Height = 27
      Align = alBottom
      AutoSize = True
      BorderWidth = 2
      ButtonHeight = 19
      ButtonWidth = 26
      List = True
      ShowCaptions = True
      TabOrder = 0
      object ToolButton1: TToolButton
        Tag = 1
        Left = 0
        Top = 0
        Caption = #215'1'
        Grouped = True
        ImageIndex = 0
        Style = tbsCheck
        OnClick = ToolBtnClick
      end
      object ToolButton2: TToolButton
        Tag = 2
        Left = 26
        Top = 0
        Caption = #215'2'
        Grouped = True
        ImageIndex = 1
        Style = tbsCheck
        OnClick = ToolBtnClick
      end
      object ToolButton3: TToolButton
        Tag = 4
        Left = 52
        Top = 0
        Caption = #215'4'
        Grouped = True
        ImageIndex = 2
        Style = tbsCheck
        OnClick = ToolBtnClick
      end
      object ToolButton4: TToolButton
        Tag = 8
        Left = 78
        Top = 0
        Caption = #215'8'
        Grouped = True
        ImageIndex = 3
        Style = tbsCheck
        OnClick = ToolBtnClick
      end
    end
  end
end
