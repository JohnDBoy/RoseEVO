object FormDBCFG: TFormDBCFG
  Left = 203
  Top = 348
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'DB server config'
  ClientHeight = 518
  ClientWidth = 233
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '굴림'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClick = StopTimer
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object Panel1: TPanel
    Left = 0
    Top = 472
    Width = 233
    Height = 46
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    OnClick = StopTimer
    object Label5: TLabel
      Left = 2
      Top = 8
      Width = 33
      Height = 12
      Caption = 'Timer'
      OnClick = StopTimer
    end
    object ProgressBar1: TProgressBar
      Left = 0
      Top = 30
      Width = 233
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
      Left = 125
      Top = 1
      Width = 105
      Height = 25
      Caption = 'Ok'
      TabOrder = 2
      OnClick = ButtonOKClick
    end
  end
  object GroupBox1: TGroupBox
    Left = 4
    Top = 8
    Width = 225
    Height = 257
    Caption = 'Database Server'
    TabOrder = 1
    object Label4: TLabel
      Left = 6
      Top = 104
      Width = 76
      Height = 12
      Caption = 'DB password'
      OnClick = StopTimer
    end
    object Label3: TLabel
      Left = 6
      Top = 83
      Width = 46
      Height = 12
      Caption = 'DB User'
      OnClick = StopTimer
    end
    object Label2: TLabel
      Left = 6
      Top = 50
      Width = 54
      Height = 12
      Caption = 'DB Name'
      OnClick = StopTimer
    end
    object Label1: TLabel
      Left = 6
      Top = 22
      Width = 71
      Height = 12
      Caption = 'DB Server IP'
      OnClick = StopTimer
    end
    object Label10: TLabel
      Left = 6
      Top = 138
      Width = 55
      Height = 12
      Caption = 'LOG User'
      OnClick = StopTimer
    end
    object Label11: TLabel
      Left = 6
      Top = 158
      Width = 85
      Height = 12
      Caption = 'LOG password'
      OnClick = StopTimer
    end
    object Label12: TLabel
      Left = 5
      Top = 193
      Width = 54
      Height = 12
      Caption = 'Mall User'
      OnClick = StopTimer
    end
    object Label13: TLabel
      Left = 5
      Top = 215
      Width = 84
      Height = 12
      Caption = 'Mall password'
      OnClick = StopTimer
    end
    object EditDBPassword: TEdit
      Left = 94
      Top = 99
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      PasswordChar = '*'
      TabOrder = 3
      Text = '1111'
      OnClick = StopTimer
    end
    object EditDBUser: TEdit
      Left = 94
      Top = 77
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 2
      Text = 'icarus'
      OnClick = StopTimer
    end
    object EditDBName: TEdit
      Left = 94
      Top = 45
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = 'SHO'
      OnClick = StopTimer
    end
    object EditDBIP: TEdit
      Left = 94
      Top = 16
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 0
      Text = '192.168.1.241'
      OnClick = StopTimer
    end
    object EditLogPassword: TEdit
      Left = 94
      Top = 154
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      PasswordChar = '*'
      TabOrder = 4
      OnClick = StopTimer
    end
    object EditLogUser: TEdit
      Left = 94
      Top = 132
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 5
      Text = '?'
      OnClick = StopTimer
    end
    object EditMallUser: TEdit
      Left = 94
      Top = 188
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 6
      Text = '?'
      OnClick = StopTimer
    end
    object EditMallPW: TEdit
      Left = 94
      Top = 210
      Width = 121
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      PasswordChar = '*'
      TabOrder = 7
      OnClick = StopTimer
    end
  end
  object GroupBox2: TGroupBox
    Left = 4
    Top = 373
    Width = 225
    Height = 87
    Caption = 'World Server'
    TabOrder = 2
    object Label6: TLabel
      Left = 8
      Top = 27
      Width = 86
      Height = 12
      Caption = 'World Server IP'
      OnClick = StopTimer
    end
    object Label8: TLabel
      Left = 8
      Top = 59
      Width = 97
      Height = 12
      Caption = 'World Server Port'
      OnClick = StopTimer
    end
    object EditWorldServerIP: TEdit
      Left = 113
      Top = 23
      Width = 102
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 0
      Text = '192.168.1.11'
      OnClick = StopTimer
    end
    object EditLoginWorldPORT: TEdit
      Left = 160
      Top = 55
      Width = 55
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = '19000'
      OnClick = StopTimer
    end
  end
  object GroupBox3: TGroupBox
    Left = 4
    Top = 275
    Width = 225
    Height = 85
    Caption = 'Account Server'
    TabOrder = 3
    object Label7: TLabel
      Left = 8
      Top = 26
      Width = 101
      Height = 12
      Caption = 'Account Server IP'
      OnClick = StopTimer
    end
    object Label9: TLabel
      Left = 8
      Top = 56
      Width = 112
      Height = 12
      Caption = 'Account Server Port'
      OnClick = StopTimer
    end
    object EditAccountServerIP: TEdit
      Left = 114
      Top = 21
      Width = 102
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 0
      Text = '211.234.107.200'
      OnClick = StopTimer
    end
    object EditAccountServerPORT: TEdit
      Left = 161
      Top = 51
      Width = 55
      Height = 20
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 1
      Text = '19001'
      OnClick = StopTimer
    end
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 208
  end
end
