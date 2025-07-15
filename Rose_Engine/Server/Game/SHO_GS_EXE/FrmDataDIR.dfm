object FormDataDIR: TFormDataDIR
  Left = 269
  Top = 278
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Select data folder'
  ClientHeight = 403
  ClientWidth = 267
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 329
    Width = 267
    Height = 74
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    OnClick = StopTimer
    OnResize = Panel1Resize
    object Label1: TLabel
      Left = 4
      Top = 32
      Width = 26
      Height = 13
      Caption = 'Timer'
      OnClick = StopTimer
    end
    object EditDataDIR: TEdit
      Left = 0
      Top = 1
      Width = 265
      Height = 21
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 0
      OnClick = StopTimer
    end
    object ButtonOK: TButton
      Left = 121
      Top = 28
      Width = 104
      Height = 25
      Caption = 'Ok'
      ModalResult = 1
      TabOrder = 1
      OnClick = ButtonOKClick
    end
    object ProgressBar1: TProgressBar
      Left = 0
      Top = 58
      Width = 267
      Height = 16
      Align = alBottom
      Min = 0
      Max = 100
      TabOrder = 2
    end
    object EditWaitTIME: TEdit
      Left = 36
      Top = 29
      Width = 41
      Height = 21
      ImeName = '한국어(한글) (MS-IME98)'
      TabOrder = 3
      Text = '10'
      OnClick = StopTimer
    end
  end
  object DirectoryListBox1: TDirectoryListBox
    Left = 0
    Top = 21
    Width = 267
    Height = 308
    Align = alClient
    ImeName = '한국어(한글) (MS-IME98)'
    ItemHeight = 16
    TabOrder = 1
    OnChange = DirectoryListBox1Change
    OnClick = DirectoryListBox1Click
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 267
    Height = 21
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 2
    object ComboBoxLANG: TComboBox
      Left = 0
      Top = 0
      Width = 193
      Height = 21
      Style = csDropDownList
      ImeName = '한국어(한글) (MS-IME98)'
      ItemHeight = 13
      TabOrder = 0
      OnClick = ComboBoxLANGClick
      OnDropDown = ComboBoxLANGDropDown
      Items.Strings = (
        'Korean'
        'English'
        'Japanese'
        'Taiwanese'
        'Chinese'
        'Reserved1'
        'Reserved2'
        'Reserved3'
        'Reserved4')
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 224
    Top = 16
  end
end
