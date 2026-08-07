#!/bin/bash

echo "🚀 Optimiere System für Performance-Tests..."

# 1. CPU Governor auf Performance setzen (benötigt root)
echo "⚙️ Setze CPU Governor auf performance..."
for cpu in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
    echo performance | sudo tee $cpu > /dev/null
done

# 2. Nicht-kritische Systemdienste stoppen
echo "⏹️ Stoppe nicht-kritische Dienste..."
SERVICES=(
    cups                  # Druckerdienst
    cups-browsed          # Netzwerk-Drucker
    bluetooth             # Bluetooth
    snapd                 # Snap-Pakete
    apt-daily             # Automatische Updates
    apt-daily-upgrade     # Automatische Upgrade-Checks
    unattended-upgrades   # Hintergrund-Updates
    ModemManager          # Mobilfunk-Modems
    whoopsie              # Fehlerberichte
    udisks2               # Laufwerks-Erkennung (Vorsicht bei externen Platten)
)

for service in "${SERVICES[@]}"; do
    sudo systemctl stop $service 2>/dev/null && echo "  ✓ $service gestoppt" || echo "  - $service nicht gefunden/inaktiv"
done

# 3. Desktop-Suche/Indexierung stoppen (falls vorhanden)
echo "⏹️ Stoppe Indexierungs-Dienste..."
pkill -f tracker 2>/dev/null
pkill -f baloo 2>/dev/null

# 4. Cloud-Sync & Browser (falls im Nutzer-Kontext)
echo "⏹️ Stoppe Nutzer-Programme..."
pkill -f dropbox 2>/dev/null
pkill -f google-drive 2>/dev/null
pkill -f onedrive 2>/dev/null
pkill -f firefox 2>/dev/null
pkill -f chrome 2>/dev/null

# 5. Swap vorübergehend deaktivieren (verhindert Auslagerung während des Tests)
echo "🔒 Deaktiviere Swap temporär..."
sudo swapoff -a

# 6. Page Cache leeren (für sauberen Start)
echo "🧹 Leere Page Cache..."
echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null

echo "✅ System optimiert!"
