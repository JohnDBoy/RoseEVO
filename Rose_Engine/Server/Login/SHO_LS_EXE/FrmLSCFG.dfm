object FormLSCFG: TFormLSCFG
  Left = 1920
  Top = 550
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Config'
  ClientHeight = 406
  ClientWidth = 250
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '굴림체'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClick = StopTimer
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object Label1: TLabel
    Left = 33
    Top = 220
    Width = 96
    Height = 12
    Caption = 'Limit User Count'
  end
  object StaticTextDB: TStaticText
    Left = 53
    Top = 106
    Width = 76
    Height = 16
    Alignment = taCenter
    Caption = 'DB Server IP'
    TabOrder = 0
    OnClick = StopTimer
  end
  object StaticTextCLIENT: TStaticText
    Left = 1
    Top = 192
    Width = 130
    Height = 16
    Alignment = taCenter
    Caption = 'Client Listen Port NO'
    Enabled = False
    TabOrder = 1
    OnClick = StopTimer
  end
  object StaticTextSERVER: TStaticText
    Left = 1
    Top = 138
    Width = 130
    Height = 16
    Alignment = taCenter
    Caption = 'Server Listen Port NO'
    TabOrder = 2
    OnClick = StopTimer
  end
  object Panel1: TPanel
    Left = 0
    Top = 361
    Width = 250
    Height = 45
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 3
    OnClick = StopTimer
    object Label5: TLabel
      Left = 2
      Top = 8
      Width = 30
      Height = 12
      Caption = 'Timer'
    end
    object ProgressBar1: TProgressBar
      Left = 0
      Top = 29
      Width = 250
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
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = '10'
      OnClick = StopTimer
    end
    object ButtonOK: TButton
      Left = 138
      Top = 1
      Width = 105
      Height = 25
      Caption = 'Ok'
      TabOrder = 2
      OnClick = ButtonOKClick
    end
  end
  object EditDBIP: TEdit
    Left = 133
    Top = 102
    Width = 113
    Height = 20
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 4
    Text = '192.168.1.112'
    OnClick = StopTimer
  end
  object EditClientPORT: TEdit
    Left = 133
    Top = 187
    Width = 113
    Height = 20
    Enabled = False
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 5
    Text = '29000'
    OnClick = StopTimer
  end
  object EditServerPORT: TEdit
    Left = 133
    Top = 133
    Width = 113
    Height = 20
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 6
    Text = '19000'
    OnClick = StopTimer
  end
  object StaticTextRIGHT: TStaticText
    Left = 34
    Top = 165
    Width = 94
    Height = 16
    Alignment = taCenter
    Caption = 'Min Login Right'
    Enabled = False
    TabOrder = 7
    OnClick = StopTimer
  end
  object EditLoginRIGHT: TEdit
    Left = 132
    Top = 160
    Width = 113
    Height = 20
    Enabled = False
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 8
    Text = '2'
    OnClick = StopTimer
  end
  object EditLimitUserCNT: TEdit
    Left = 132
    Top = 217
    Width = 114
    Height = 20
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 9
    Text = '0'
    OnClick = StopTimer
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 250
    Height = 91
    Align = alTop
    Caption = 'GUMS'
    TabOrder = 10
    object StaticTextGumsIP: TStaticText
      Left = 48
      Top = 42
      Width = 82
      Height = 16
      Alignment = taCenter
      Caption = 'GUM Server IP'
      Enabled = False
      TabOrder = 0
      OnClick = StopTimer
    end
    object EditGumsIP: TEdit
      Left = 132
      Top = 38
      Width = 113
      Height = 20
      Enabled = False
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = '192.168.20.204'
      OnClick = StopTimer
    end
    object EditGumsPORT: TEdit
      Left = 132
      Top = 62
      Width = 113
      Height = 20
      Enabled = False
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 2
      Text = '20000'
      OnClick = StopTimer
    end
    object StaticTextGumsPORT: TStaticText
      Left = 16
      Top = 66
      Width = 112
      Height = 16
      Alignment = taCenter
      Caption = 'GUM Server Port NO'
      Enabled = False
      TabOrder = 3
      OnClick = StopTimer
    end
    object CheckBoxGUMS: TCheckBox
      Left = 43
      Top = 15
      Width = 102
      Height = 17
      Alignment = taLeftJustify
      Caption = 'Use GUM Server'
      TabOrder = 4
      OnClick = CheckBoxGUMSClick
    end
  end
  object CheckBoxWS: TCheckBox
    Left = 16
    Top = 256
    Width = 233
    Height = 17
    Caption = 'Show only world server'
    TabOrder = 11
    OnClick = StopTimer
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 288
    Width = 250
    Height = 73
    Align = alBottom
    Caption = 'Monitor Password'
    TabOrder = 12
    object EditPW: TEdit
      Left = 6
      Top = 14
      Width = 238
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      PasswordChar = '*'
      TabOrder = 0
      Text = 'EditPW'
      OnClick = StopTimer
    end
    object ButtonPW: TButton
      Left = 56
      Top = 40
      Width = 129
      Height = 25
      Caption = 'Change Password'
      TabOrder = 1
      OnClick = ButtonPWClick
    end
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Top = 95
  end
end
