object FormGSCFG: TFormGSCFG
  Left = 121
  Top = 415
  Width = 387
  Height = 554
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'Game server config'
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '굴림체'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 379
    Height = 99
    Align = alTop
    Caption = 'Game Server '
    TabOrder = 0
    OnClick = StopTimer
    object Label5: TLabel
      Left = 44
      Top = 45
      Width = 66
      Height = 12
      Caption = 'Server Name'
      OnClick = StopTimer
    end
    object Label6: TLabel
      Left = 247
      Top = 17
      Width = 66
      Height = 12
      Anchors = [akTop, akRight]
      Caption = 'Listen Port'
      OnClick = StopTimer
    end
    object Label1: TLabel
      Left = 12
      Top = 19
      Width = 96
      Height = 12
      Caption = 'Client Listen IP'
      OnClick = StopTimer
    end
    object Label2: TLabel
      Left = 90
      Top = 73
      Width = 18
      Height = 12
      Caption = 'Age'
    end
    object Label3: TLabel
      Left = 174
      Top = 75
      Width = 6
      Height = 12
      Caption = '~'
    end
    object Label4: TLabel
      Left = 250
      Top = 45
      Width = 63
      Height = 12
      Anchors = [akTop, akRight]
      Caption = 'Channel NO'
    end
    object Label8: TLabel
      Left = 258
      Top = 72
      Width = 53
      Height = 12
      Anchors = [akTop, akRight]
      Caption = 'Max Users'
    end
    object EditWorldNAME: TEdit
      Left = 118
      Top = 43
      Width = 120
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 0
      Text = 'World1-1'
      OnClick = StopTimer
    end
    object EditClientPORT: TEdit
      Left = 317
      Top = 14
      Width = 54
      Height = 20
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = '29200'
      OnClick = StopTimer
    end
    object EditClientIP: TEdit
      Left = 118
      Top = 14
      Width = 120
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 2
      Text = '211.115.95.XXX'
      OnClick = StopTimer
    end
    object EditLowAGE: TEdit
      Left = 118
      Top = 71
      Width = 37
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 3
      Text = '0'
    end
    object EditHighAGE: TEdit
      Left = 200
      Top = 71
      Width = 38
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 4
      Text = '0'
    end
    object EditChannelNO: TEdit
      Left = 317
      Top = 42
      Width = 54
      Height = 20
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 5
      Text = '1'
    end
    object EditMaxUSER: TEdit
      Left = 317
      Top = 69
      Width = 54
      Height = 20
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 6
      Text = '2048'
    end
  end
  object ListViewZONE: TListView
    Left = 0
    Top = 99
    Width = 379
    Height = 376
    Align = alClient
    Checkboxes = True
    Columns = <
      item
        Caption = 'Zone NO'
        Width = 60
      end
      item
        AutoSize = True
        Caption = 'Zone name'
      end>
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = '굴림체'
    Font.Style = []
    GridLines = True
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    ParentFont = False
    PopupMenu = PopupMenu1
    TabOrder = 1
    ViewStyle = vsReport
    OnClick = StopTimer
  end
  object Panel1: TPanel
    Left = 0
    Top = 475
    Width = 379
    Height = 52
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    OnClick = StopTimer
    object Label7: TLabel
      Left = 3
      Top = 11
      Width = 30
      Height = 12
      Caption = 'Timer'
      OnClick = StopTimer
    end
    object ButtonOK: TButton
      Left = 115
      Top = 6
      Width = 150
      Height = 25
      Caption = 'Ok'
      TabOrder = 0
      OnClick = ButtonOKClick
    end
    object ButtonRESET: TButton
      Left = 299
      Top = 6
      Width = 73
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Reset'
      TabOrder = 1
      OnClick = ButtonRESETClick
    end
    object EditWaitTIME: TEdit
      Left = 36
      Top = 8
      Width = 41
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 2
      Text = '10'
      OnClick = StopTimer
    end
    object ProgressBar1: TProgressBar
      Left = 0
      Top = 36
      Width = 379
      Height = 16
      Align = alBottom
      Min = 0
      Max = 100
      TabOrder = 3
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 352
    Top = 128
  end
  object PopupMenu1: TPopupMenu
    Left = 352
    Top = 176
    object Selectall1: TMenuItem
      Caption = 'Select all'
      OnClick = Selectall1Click
    end
    object Select1: TMenuItem
      Caption = 'Check'
      OnClick = Select1Click
    end
    object Uncheck1: TMenuItem
      Caption = 'Uncheck'
      OnClick = Uncheck1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Reset1: TMenuItem
      Caption = 'Reset'
      OnClick = Reset1Click
    end
    object Inverselyselect1: TMenuItem
      Caption = 'Check Inversely'
      OnClick = Inverselyselect1Click
    end
  end
end
