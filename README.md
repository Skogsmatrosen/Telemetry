# Battlebot telemetri

Telemetrisystem for battleboten. En ESP32 på boten måler akselerasjon og batterispenning og sender dataene trådløst til en ESP32 koblet til PC-en, der de vises på en nettside i sanntid.

## Programmene

- **`Tele/`**: kjører på ESP-en på boten. Leser IMU og batterispenning og sender det over ESP-NOW 20 ganger i sekundet.
- **`Reciever/`**: kjører på ESP-en som står i PC-en. Tar imot pakkene og skriver dem ut på USB, én linje per pakke, som nettsiden leser.

## Åpne prosjektet

1. Installer [Git](https://git-scm.com) og [VS Code](https://code.visualstudio.com) med utvidelsen **PlatformIO IDE**.
2. Klon repoet til en mappe **uten mellomrom i stien**, f.eks. `C:\dev\`:
   ```
   git clone <lenke til repoet>
   ```
3. Åpne `Telemetry.code-workspace`: dobbeltklikk på fila i mappa, eller bruk File → Open Workspace from File i VS Code.
4. Velg prosjekt i PlatformIO-velgeren nederst i statuslinjen, og bygg (✓) eller last opp (→) med knappene der.

## Hente oppdateringer

Fra repoets mappe:
```
git pull
```
eller trykk **Sync Changes** i Source Control-panelet i VS Code.
