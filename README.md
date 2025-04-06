# 🔐 SafeProject – Elektronischer Zahlensafe mit Arduino

Ein Arduino-Projekt zur Simulation eines elektronischen Safes, der nur durch Eingabe eines richtigen vierstelligen Codes geöffnet werden kann – basierend auf dem Spiel „Number Baseball“.

## 🔧 Verwendete Hardware

- Arduino Uno oder Funduino
- 4x4 Keypad
- LCD 16x2 mit I²C-Modul
- Servomotor (z. B. SG90)
- Lichtsensor (KY-010)
- LED + 330Ω Widerstand
- Breadboard & Jumper-Kabel
- Optional: 3D-gedrucktes Gehäuse

## 🎮 Funktionen

- Zufällige Generierung eines vierstelligen Codes ohne Duplikate
- Eingabe des Codes über ein Tastenfeld (Keypad)
- Rückmeldung in Form von "Strike", "Ball" oder "Out" auf dem LCD
- Servo öffnet den Safe bei korrektem Code, schließt per Taste „D“
- LED leuchtet nur bei ausreichender Helligkeit (Lichtsensor)
- Erweiterbar mit eigenem Verschlüsselungsalgorithmus (z. B. Caesar)

## 🛠 Aufbau



## 💡 Beispielcode

```cpp
// Beispiel zur Caesar-Verschlüsselung
String plainCode = "1234";
String encryptedCode = "";

for (int i = 0; i < plainCode.length(); i++) {
  char c = plainCode[i];
  char shifted = ((c - '0' + 3) % 10) + '0';
  encryptedCode += shifted;
}
// Ausgabe: 4567
```

## 📦 Projektstruktur

- `SafeProject.ino` – Hauptcode für Arduino
- `README.md` – Diese Projektbeschreibung
- `/images/` – (Hier kannst du Fotos oder Diagramme hinzufügen)
- Optional: 3D-Dateien, z. B. `SafeCase.3mf`

## 🚀 Anleitung

1. Repository herunterladen oder klonen
2. Datei `SafeProject.ino` in der Arduino IDE öffnen
3. Board und Port auswählen, hochladen
4. Serielle Konsole öffnen für Debug-Infos

## 📚 Lizenz

MIT License – frei zur privaten oder schulischen Nutzung.
