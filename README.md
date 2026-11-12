# Battlebot telemetri

Telemetrisystem for battleboten. En ESP32 på boten måler akselerasjon og batterispenning og sender dataene trådløst til en ESP32 koblet til PC-en, der de vises på en nettside i sanntid.

## Programmene

- **`Tele/`**: kjører på ESP-en på boten. Leser IMU, batterispenning og radiokontrolleren (ELRS), styrer motorene og sender telemetri over ESP-NOW 20 ganger i sekundet.
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

## Data fra mottakeren (til nettsiden)

Mottaker-ESP-en kobles til PC-en med USB og skriver ut én JSON-linje per pakke, ca. 20 linjer i sekundet.

### Tilkobling

| Innstilling | Verdi |
|---|---|
| Baudrate | **115200** |
| Format | 8N1 |
| DTR / RTS | **Av** (ellers kan ESP-en resettes når porten åpnes) |
| Linjeslutt | `\n` |

Bare ett program kan ha porten åpen om gangen. Lukk serial monitor i VS Code før nettsiden kobler til.

### Eksempel på en linje

```json
{"time_ms":7150,"ax_g":-0.381,"ay_g":0.829,"battery_v":12.45,"seq":142,"az_g":-0.388,"mode":"DRIVE","link":true,"battery_dead":false,"startup_lock":false,"weapon":"LOADED","left_us":1620,"right_us":1580,"weapon_us":1000}
```

### Feltene

| Felt | Type | Enhet / verdier | Betydning |
|---|---|---|---|
| `time_ms` | tall | ms | Tid siden boten startet. Starter på 0 igjen hvis boten restarter |
| `seq` | tall | | Løpenummer. Hopp i tallet betyr tapte pakker |
| `ax_g`, `ay_g`, `az_g` | tall | g | Akselerasjon. Ligger boten flatt, er `az_g` ca. 1 |
| `battery_v` | tall | V | Batterispenning (12 V blysyre, fullt ca. 12,7 V) |
| `mode` | tekst | `"SAFE"`, `"DRIVE"`, `"DRIVE+WEAPON"` | Modusbryteren på kontrolleren |
| `link` | true/false | | Forbindelse mellom kontrolleren og boten |
| `battery_dead` | true/false | | Boten er stoppet fordi batteriet var under 11,6 V |
| `startup_lock` | true/false | | Boten venter på at modusbryteren settes til SAFE etter oppstart |
| `weapon` | tekst | `"LOADED"`, `"FIRING"`, `"FAULT"`, `"MANUAL"` | Våpenets tilstand (se under) |
| `left_us`, `right_us` | tall | µs, 1000–2000 | Signal til hjulmotorene. 1500 = stopp, over = fremover, under = bakover |
| `weapon_us` | tall | µs, 1000–2000 | Signal til våpenmotoren. 1000 = stopp |

Våpentilstandene:

| Tilstand | Betydning |
|---|---|
| `LOADED` | Ladet og klar, venter på fyr-knappen |
| `FIRING` | Motoren går. Fyrer og lader på nytt |
| `FAULT` | Motoren nådde ikke endebryteren i tide og er stoppet. Nullstilles med SAFE |
| `MANUAL` | Battleshort: motoren styres manuelt fra kontrolleren |

### Lese dataene (Web Serial)

Fungerer i Chrome og Edge. Siden må kjøres fra `localhost` eller `https`, og porten må åpnes med et knappetrykk.

```js
document.getElementById("connect").onclick = async () => {
  const port = await navigator.serial.requestPort();
  await port.open({ baudRate: 115200 });
  await port.setSignals({ dataTerminalReady: false, requestToSend: false });

  const reader = port.readable.pipeThrough(new TextDecoderStream()).getReader();
  let buf = "";
  while (true) {
    const { value, done } = await reader.read();
    if (done) break;
    buf += value;
    const lines = buf.split("\n");
    buf = lines.pop();                    // ufullstendig linje venter på resten
    for (const line of lines) {
      if (!line.startsWith("{")) continue; // hopp over oppstartsmeldinger
      let data;
      try { data = JSON.parse(line); } catch { continue; }
      update(data);                       // oppdaterer siden
    }
  }
};
```

### Ting å være klar over

- **Linjer som ikke starter med `{`** (f.eks. `Receiver MAC: ...` og `Receiver ready!` ved oppstart) skal hoppes over.
- **Nye felt kan dukke opp senere.** Les feltene med navn (`data.battery_v`), og ignorer felt siden ikke kjenner.
- **Ingen nye linjer på ca. 1 sekund** betyr at boten er slått av, utenfor rekkevidde eller har restartet. Vis det tydelig, for eksempel ved å gråe ut verdiene.
- **`link: false`** betyr at boten har mistet kontakten med *kontrolleren*, men telemetrien kommer fortsatt frem. Boten står da stille.
- **`time_ms` som plutselig blir mindre** betyr at boten har restartet.
- **Tapte pakker** er normalt i blant. Sjekk `seq` om det er viktig å vite.
