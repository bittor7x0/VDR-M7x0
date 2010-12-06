'*********************************************************************************
'script		: jpg2iGrame.vbs
'description: recursive set cover art in MP3 file
'
'needed tools: both need to me in a subfolder with the mane of the tool
' - netpbm: http://netpbm.sourceforge.net (Windows: http://sourceforge.net/project/showfiles.php?group_id=23617&package_id=16357)
' - mjpegtools: http://mjpeg.sourceforge.net (Windows: http://freenet-homepage.de/CharlesWeb/download.html)
'
'usage		: simply start it :-)
' You will be asked for the folder with the JPG files. The script will create 
' the iFrame file in the same folder with the same name as the JPG file.
'
'date			: 24.NOV.2008
'*********************************************************************************
' Based on Mp3Playlister_singleList.vbs by la_boost@yahoo.com
' Flags for the options parameter
'****** History ******************************************************************
' 13.DEZ.2008
' - New Parameter runQuiet and batchMode (Thanks to tobse)
' - New ReadJPG function that sould be able to work with all JPGs (Thanks to kille)


'***********************************
'BEGIN
'***********************************
Option Explicit

Const BIF_returnonlyfsdirs   = &H0001
Const BIF_dontgobelowdomain  = &H0002
Const BIF_statustext         = &H0004
Const BIF_returnfsancestors  = &H0008
Const BIF_editbox            = &H0010
Const BIF_validate           = &H0020
Const BIF_browseforcomputer  = &H1000
Const BIF_browseforprinter   = &H2000
Const BIF_browseincludefiles = &H4000

Const ForReading = 1, ForWriting = 2, ForAppending = 8
Dim fso, WshShell, cptTot, objArgs, arrFiles(), sExtToGet, objExplorer
Dim driveLetter, pathToScan, fold, nTime, sAppName, txtDefPic, doRepalce
Dim SW ,SH, ScreenRatio, system1, system2, aspect, framerate, sPath, runQuiet, batchMode, defaultPath
Set fso = CreateObject("Scripting.FileSystemObject")
Set WshShell = WScript.CreateObject("WScript.Shell")
sAppName = "jpg2iFrame - Recursive *.jpg to *.iFrame converter"

'-- Run in quiet-mode true/false
runQuiet = false

'-- Default working directory - used if batchMode=true
batchMode = false
defaultPath = "C:\Path"

'-- lowercase file extension to search for
sExtToGet = "jpg"		

'-- Replace existing covers?
doRepalce = true

'-- Defaults
SW = 720

'NTSC
'SH = 480
'system1 = ""
'system2 = "n"
'framerate = "30000:1001"

'PAL
SH = 576
system1 = "--pal"
system2 = "p"
framerate = "25:1"

'4:3
'ScreenRatio = 4 / 3
'aspect = "2"

'16:9
ScreenRatio = 16 / 9
aspect = "3"


'-------------------- Do not edit after this line ------------------------------
'-- Script Path
sPath = Replace(WScript.ScriptFullName, WScript.ScriptName, "") 

'-- Delete old log file
'if fso.FileExists(sPath & "\jpg2iFrame.log")=true then
'	fso.DeleteFile sPath & "\jpg2iFrame.log"
'end if


'-- Ask for the path
If batchMode= false Then
	pathToScan = BrowseForFolder( _
        "Ordner mit JPG Dateien auswählen", _
        BIF_returnonlyfsdirs , _
        sAppName)
else
	pathToScan = defaultPath
End If

nTime = Timer
'-- Open progress window
If runQuiet= false Then
  Set objExplorer = CreateObject("InternetExplorer.Application")
    objExplorer.Navigate "about:blank"   
    objExplorer.ToolBar = 0
    objExplorer.StatusBar = 1
    objExplorer.Width = 800
    objExplorer.Height = 600 
    objExplorer.Visible = 1             
    objExplorer.Document.Title = sAppName
End If



'-- start scanning
Call startScanning()

'-- clean
Set fso = nothing
Set WshShell = nothing		

'Wscript.Sleep 5000

'objExplorer.Quit

'***********************************
'END
'***********************************


'***********************************
'FUNCTIONS:
'***********************************

Sub echoText(text)
	If runQuiet= false Then
		If text="*$CLR$*" Then
			objExplorer.Document.Body.InnerHTML = ""
		else
			objExplorer.Document.Body.InnerHTML = objExplorer.Document.Body.InnerHTML & text
		End if
	End If
End Sub


Sub startScanning()
	Dim i, cpt
	cptTot = 0 
	If fso.FolderExists(pathToScan) Then
		ReDim arrFiles(0)
		Set fold = fso.Getfolder(pathToScan)
		'-- recurse folder
		echoText "One moment scanning folders '"&pathToScan&"'....</br>"
		Call DoIt(fold)		
	Else
	 	WshShell.Popup "Folder """& pathToScan &""" does not exist. ", 5, sAppName, 48
	 	Wscript.quit
	End If	
		
	'-- save playlist if more than 0 entry in it
	If (UBound(arrFiles) > 0) Then
		echoText "Found " & UBound(arrFiles) & " folders. Start adding covers..</br>"
		Call addAlbumArt(arrFiles)		
	End If		

	If runQuiet = false Then	
		WshShell.Popup "Finished. "  & chr(13) & chr(13) & cptTot & _
					" files processed " & _
					Chr(13) & Chr(13) & showTime(nTime) _
					, 0, sAppName, 64	
	End If
End Sub 
'*********************************************************************************

Sub AddFiles(fold)
'-- process all mp3 files in the fold folder
	Dim strExt, mpFiles, strName, foldName, foldPath, f
	
	foldPath = fold.Path
	Set mpfiles = fold.Files
	
	For each f in mpfiles
		strName = f.Name
		strExt = LCase(fso.GetExtensionName(strName))
		If strExt = sExtToGet Then
			arrFiles(cptTot) = foldPath &"\"& UCase(Left(strName, 1)) & Mid(strName,2,Len(strName))
			ReDim Preserve arrFiles(UBound(arrFiles)+1)
			cptTot = cptTot + 1	'-- global counter for processed files
		End If
	Next

End Sub
'*********************************************************************************
   
Sub addAlbumArt(arrFiles)
	Dim txtFile, i, j, txtAlbum, oExec, txt, stTmp, size, ScaleW, ScaleH, cmd, intReturn
	
	j = ubound(arrFiles)-1
	for i=0 to j
'		echoText "*$CLR$*"
		echoText "</br>"
		echoText "****************************** Next file " & i & "/" & j & " *********************************</br>"
		echoText arrFiles(i) & "</br>"
		txtAlbum = left(arrFiles(i),instrrev(arrFiles(i),"."))
		if fso.FileExists(txtAlbum & "iFrame")=true and doRepalce=False then
			echoText "Skipping, picture.iFrame allready there</br>"
		Else
			size=ReadJPG(arrFiles(i))		
			echoText "Size: " & size(0) & "x" & size(1) & "</br>"
			if size(0) = -1 or size(1) = -1 then
				echoText "Die Größe des JPG konnte nicht ermittelt werden. Bild wird übersprungen</br>"
			else
				if (size(0) / size(1)) <= ScreenRatio Then
					size(0) = size(1) * ScreenRatio
				Else
					size(1) = size(0) / ScreenRatio
				End if
				ScaleW = SW / size(0)
				ScaleH = SH / size(1)
				
				cmd =       ".\netpbm\jpegtopnm.exe "& chr(34) & arrFiles(i) & chr(34) &" |"
				cmd = cmd & ".\netpbm\pnmscale.exe --xscale=" & Replace(CStr(ScaleW),",",".") & " --yscale=" & Replace(CStr(ScaleH),",",".") & " |"
				cmd = cmd & ".\netpbm\pnmpad.exe --black --width " & SW & " --height " & SH & " |"
				cmd = cmd & ".\netpbm\ppmntsc.exe " & system1 & " |"
				cmd = cmd & ".\mjpegtools\ppmtoy4m.exe -F " & framerate & " -I p -S 420mpeg2 |"
				cmd = cmd & ".\mjpegtools\mpeg2enc.exe -f 3 -b 12500 -a " & aspect & " -q 1 -n " & system2 &" -o " & chr(34) & txtAlbum & "iFrame" & chr(34)

				intReturn=WshShell.Run("cmd /c " & cmd,0,true)
				if intReturn <> 0 Then 
					echoText "Fehler bei der Konvertierung!</br>"
					echoText cmd & "</br>"
				else
					echoText "'" & right(txtAlbum,len(txtAlbum)-instrrev(txtAlbum,"\")) & "iFrame ' erfolgreich erstellt.</br>"
				end if

	'			Set oExec    = WshShell.Exec("cmd /c " & cmd)
				'Print messages from 'Exec
	'			Do While Not oExec.StdOut.AtEndOfStream
	'				echoText oExec.StdOut.ReadLine()
	'			Loop 
			end if
		end if
	next
End Sub
'*********************************************************************************
   
Sub DoIt(fold)
'-- recursive scan
	Dim sfold, sfoo
   Call AddFiles(fold)			'process files in current folder
	Set sfold = fold.subfolders 
	for each sfoo in sfold 		'process files in subfolders
		Call DoIt(sfoo)
	Next
End Sub  
'*********************************************************************************

Function showTime(nTime)
	showTime = "Elapsed time : " & Round((Timer - nTime),2) &" seconds"
End Function
'*********************************************************************************
' Using the shell's BrowseForFolder method to
' return the full path to the selected object
' title = Text shown in the dialog box
' flag = One of the values for controlling the 
'        BrowseForFolder behavior
' dir = Preselected directory (can be "")
Function BrowseForFolder(title, flag, dir)
    On Error Resume Next

    Dim oShell, oItem, tmp
 
    ' Create WshShell object.
    Set oShell = WScript.CreateObject("Shell.Application")

    ' Invoke Browse For Folder dialog box.
    Set oItem = oShell.BrowseForFolder(&H0, title, flag, dir)
    If Err.Number <> 0 Then
        If Err.Number = 5 Then
            BrowseForFolder= "-5"
            Err.Clear
            Set oShell = Nothing
            Set oItem = Nothing
            Exit Function
        End If
    End If

    ' Now we try to retrieve the full path.
    BrowseForFolder = oItem.ParentFolder.ParseName(oItem.Title).Path

    ' Handling: Cancel button and selecting a drive
    If Err<> 0 Then
        If Err.Number = 424 Then           ' Handle Cancel button.
            BrowseForFolder = "-1"
        Else
            Err.Clear
            ' Handle situation in which user selects a drive.
            ' Extract drive letter from the title--first search
            ' for a colon (:).
            tmp = InStr(1, oItem.Title, ":")
            If tmp > 0 Then           ' A : is found; use two 
                                      ' characters and add \.
                BrowseForFolder = _   
                    Mid(oItem.Title, (tmp - 1), 2) & "\"
            End If
        End If
    End If

    Set oShell = Nothing
    Set oItem = Nothing
    On Error GoTo 0
End Function

Function BrowseForFile(pstrPath, pstrFilter)
	Set objDialog = CreateObject("MSComDlg.CommonDialog")
	objDialog.Filter = pstrFilter
	objDialog.InitDir = pstrPath
	objDialog.MaxFileSize = 256
	objDialog.Flags = &H80000 + &H4 + &H8
	intResult = objDialog.ShowOpen()
	BrowseForFile = objDialog.FileName
End Function

Function SelectFile(strFileSpec)
	Set objIE = CreateObject("InternetExplorer.Application")
    ' Function to display file open dialog and return selected file.
    objIE.Navigate("about:blank")
    Do Until objIE.ReadyState = 4
        If (Wscript.Version > 5) Then
            Wscript.Sleep 100
        End If
    Loop
    objIE.Document.Write("<html><body><input id=file type=file></body></html>")
    objIE.Document.all.file.focus
    If (Wscript.Version > 5) Then
        wsh.SendKeys strFileSpec & "{Tab}"
        Wscript.Sleep 200
    End If
    objIE.Document.all.file.click
    SelectFile = objIE.Document.all.file.value
    If (SelectFile = strFileSpec) Then
        SelectFile = ""
    End If
End Function

Function SelectFileOld(pstrPath, pstrFilter)
		Set objDialog = CreateObject("UserAccounts.CommonDialog")  
		objDialog.Filter = pstrFilter
		objDialog.Flags = &H0200  
		objDialog.FilterIndex = 1  
		objDialog.InitialDir = pstrPath  
		intResult = objDialog.ShowOpen  
  		If intResult = 0 Then  
			WScript.Echo "Kein Bild gewählt"
		else
			SelectFile = objDialog.FileName
		End If  
End Function

Function ReadJPG(file)
    Dim fso, ts, s, HW, Marker, LenHex, LenDec, SOF1, SOF2, SOS

	' die Marker als Strings zum besseren Vergleich
    SOF1   = Chr(255) & Chr (192)  ' "ÿÀ"
    SOF2   = Chr(255) & Chr (194)  ' "ÿÂ"
    SOS    = Chr(255) & Chr (218)  ' "ÿÚ"

    ' Variablen initialisieren
    HW     = Array("","")
    HW(0)  = -1
    HW(1)  = -1
    s      = ""
    Marker = ""
    LenHex = ""
    LenDec = 0

    ' Datei öffnen
    Set fso = CreateObject("Scripting.FileSystemObject")
    Set ts = fso.OpenTextFile( file)

    ' Der erste Marker im JPEG-Header ist immer 0xFFD8 (SOI), also einfach mal ignorieren
    ts.Skip(2)

    ' Nun einfach alle Abschnitte durchgehen, bis der SOF-Marker (0xFFC0 oder 0xFFC2) erreicht wurde.
    do
      ' Marker einlesen
      Marker = ts.Read(2)

      ' Wenn der SOS-Marker (0xFFDA) erreicht ist, kommt nichts spannendes mehr, also abbrechen.
      if Marker = SOS then
        echoText "Kein SOF-Marker => Bild ist kaputt :("
        exit do
      end if

      ' Gucken, obs der SOF-Marker ist
      if Marker <> SOF1 and Marker <> SOF2 then
        ' Anscheinend nicht, also sind die beiden nächsten Bytes die Länge des Abschnitts
        LenHex = ts.Read(2)
        LenDec = HexToDec(HexAt(LenHex,1) & HexAt(LenHex,2))
        ' Abschnitt überspringen: die Länge des Abschnitts ist inkl. der beiden schon
        ' gelesenen Bytes, die müssen also abgezogen werden
        ts.Skip(LenDec-2)
      else
        ' Okay, SOF gefunden, dann nur noch 3 Bytes überspringen, die 4 Bytes danach sind
        ' dann die gesuchten Werte.
		ts.Skip(3)
		s = ts.Read(4)
      end if
    loop until Marker = SOF1 or Marker = SOF2
    ts.Close

    if s <> "" then
        HW(0) = HexToDec(HexAt(s,3) & HexAt(s,4))
        HW(1) = HexToDec(HexAt(s,1) & HexAt(s,2))
    else
     echoText "Beim ermitteln der Größe des Bildes ist ein Fehler aufgetreten. Bild kann nicht konvertiert werden."
    end if
    ReadJPG = HW
End Function

Function HexToDec(cadhex) 
    HexToDec = CLng("&H" & cadhex) 
End Function 


Function HexAt(s, n)
	HexAt = right("0" & Hex(Asc(Mid(s, n, 1))),2)
End Function 

'*** End