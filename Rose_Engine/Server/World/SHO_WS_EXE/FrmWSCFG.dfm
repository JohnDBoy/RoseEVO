object FormWSCFG: TFormWSCFG
  Left = 209
  Top = 212
  BorderStyle = bsToolWindow
  Caption = 'Config'
  ClientHeight = 509
  ClientWidth = 271
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClick = StopTimer
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label6: TLabel
    Left = 4
    Top = 277
    Width = 131
    Height = 13
    Caption = 'Zone Server Listen Port NO'
    OnClick = StopTimer
  end
  object Label4: TLabel
    Left = 62
    Top = 221
    Width = 73
    Height = 13
    Caption = 'Login Server IP'
    OnClick = StopTimer
  end
  object Label5: TLabel
    Left = 34
    Top = 248
    Width = 101
    Height = 13
    Caption = 'Login Server Port NO'
    OnClick = StopTimer
  end
  object EditZoneListenPORT: TEdit
    Left = 144
    Top = 273
    Width = 121
    Height = 21
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 0
    Text = '19000'
    OnClick = StopTimer
  end
  object Panel1: TPanel
    Left = 0
    Top = 463
    Width = 271
    Height = 46
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    OnClick = StopTimer
    object Label8: TLabel
      Left = 2
      Top = 8
      Width = 26
      Height = 13
      Caption = 'Timer'
      OnClick = StopTimer
    end
    object ProgressBar1: TProgressBar
      Left = 0
      Top = 30
      Width = 271
      Height = 16
      Align = alBottom
      Min = 0
      Max = 100
      TabOrder = 0
    end
    object EditWaitTIME: TEdit
      Left = 39
      Top = 4
      Width = 41
      Height = 21
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = '10'
      OnClick = StopTimer
    end
    object ButtonOK: TButton
      Left = 162
      Top = 1
      Width = 105
      Height = 25
      Caption = 'Ok'
      TabOrder = 2
      OnClick = ButtonOKClick
    end
  end
  object EditLoginServerIP: TEdit
    Left = 144
    Top = 217
    Width = 121
    Height = 21
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 2
    Text = '211.234.107.200'
    OnClick = StopTimer
  end
  object EditLoginServerPORT: TEdit
    Left = 144
    Top = 245
    Width = 121
    Height = 21
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 3
    Text = '19000'
    OnClick = StopTimer
  end
  object GroupBoxEX: TGroupBox
    Left = 2
    Top = 308
    Width = 268
    Height = 105
    Caption = 'External Infomation'
    TabOrder = 4
    object Label3: TLabel
      Left = 76
      Top = 22
      Width = 59
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'World Name'
      OnClick = StopTimer
    end
    object Label11: TLabel
      Left = 65
      Top = 50
      Width = 70
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'Client Listen IP'
      OnClick = StopTimer
    end
    object Label7: TLabel
      Left = 37
      Top = 78
      Width = 98
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'Client Listen Port NO'
      OnClick = StopTimer
    end
    object EditUserListenPORT: TEdit
      Left = 141
      Top = 74
      Width = 121
      Height = 21
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 0
      Text = '29100'
      OnClick = StopTimer
    end
    object EditExtIP: TEdit
      Left = 141
      Top = 46
      Width = 121
      Height = 21
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = '211.115.95.11'
      OnClick = StopTimer
    end
    object EditWorldNAME: TEdit
      Left = 141
      Top = 18
      Width = 121
      Height = 21
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 2
      Text = 'World 1'
      OnClick = StopTimer
    end
  end
  object GroupBoxDB: TGroupBox
    Left = 1
    Top = 8
    Width = 269
    Height = 196
    Caption = 'Database Server'
    TabOrder = 5
    object Label1: TLabel
      Left = 76
      Top = 17
      Width = 62
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'DB Server IP'
      OnClick = StopTimer
    end
    object Label2: TLabel
      Left = 62
      Top = 44
      Width = 76
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'DB Table Name'
      OnClick = StopTimer
    end
    object Label12: TLabel
      Left = 79
      Top = 78
      Width = 58
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'DB Account'
      OnClick = StopTimer
    end
    object Label13: TLabel
      Left = 72
      Top = 106
      Width = 64
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'DB Password'
      OnClick = StopTimer
    end
    object Label10: TLabel
      Left = 53
      Top = 171
      Width = 85
      Height = 13
      Caption = 'Log DB Password'
      OnClick = StopTimer
    end
    object Label9: TLabel
      Left = 58
      Top = 143
      Width = 79
      Height = 13
      Caption = 'Log DB Account'
      OnClick = StopTimer
    end
    object EditDBServerIP: TEdit
      Left = 142
      Top = 13
      Width = 121
      Height = 21
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 0
      Text = '192.168.1.241'
      OnClick = StopTimer
    end
    object EditDBTableNAME: TEdit
      Left = 142
      Top = 41
      Width = 121
      Height = 21
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = 'SHO'
      OnClick = StopTimer
    end
    object EditDBAccount: TEdit
      Left = 142
      Top = 75
      Width = 121
      Height = 21
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 2
      Text = 'icarus'
      OnClick = StopTimer
    end
    object EditDBPW: TEdit
      Left = 142
      Top = 103
      Width = 121
      Height = 21
      Anchors = [akTop, akRight]
      ImeName = '한국어(한글) (MS-IME98)'
      PasswordChar = '*'
      TabOrder = 3
      Text = '1111'
      OnClick = StopTimer
    end
    object EditLogAccount: TEdit
      Left = 142
      Top = 139
      Width = 121
      Height = 21
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 4
      Text = '?'
      OnClick = StopTimer
    end
    object EditLogPW: TEdit
      Left = 142
      Top = 168
      Width = 121
      Height = 21
      ImeName = '한국어(한글) (MS-IME98)'
      PasswordChar = '*'
      TabOrder = 5
      Text = '1111'
      OnClick = StopTimer
    end
  end
  object CheckBoxCreateCHAR: TCheckBox
    Left = 8
    Top = 430
    Width = 153
    Height = 17
    Caption = 'Block Create Character'
    TabOrder = 6
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 264
  end
end
