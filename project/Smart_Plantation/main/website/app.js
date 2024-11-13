document.addEventListener("DOMContentLoaded", () => {
    // Abrufen und Setzen der gespeicherten Konfigurationswerte beim Laden der Seite
    fetch("/config")
        .then(response => response.json())
        .then(config => {
            // Setze den Temperaturschwellenwert
            document.getElementById("tempThreshold").value = config.temp_threshold;

            // Setze die RGB-Werte und Farb-Picker
            document.getElementById("red").value = config.red;
            document.getElementById("green").value = config.green;
            document.getElementById("blue").value = config.blue;
            const hexColor = rgbToHex(config.red, config.green, config.blue);
            document.getElementById("colorPicker").value = hexColor;

            // Setze Stunde und Minute
            document.getElementById("hour").value = config.hour || 0;
            document.getElementById("minute").value = config.minute || 0;
            updateTimeField(config.hour, config.minute);

            // Setze die Wochentage-Checkboxen
            const daysCheckboxes = document.querySelectorAll("#days input[type=checkbox]");
            daysCheckboxes.forEach((checkbox) => {
                if (config.days & parseInt(checkbox.value)) {
                    checkbox.checked = true;
                }
            });
        })
        .catch(error => console.error("Fehler beim Laden der Konfiguration:", error));
});

// Umwandlung von RGB zu Hex
function rgbToHex(r, g, b) {
    return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1).toUpperCase();
}

// Hex zu RGB umwandeln
function hexToRgb(hex) {
    const bigint = parseInt(hex.slice(1), 16);
    const r = (bigint >> 16) & 255;
    const g = (bigint >> 8) & 255;
    const b = bigint & 255;
    return { r, g, b };
}

// Farb-Update-Event
document.getElementById('colorPicker').addEventListener('input', function () {
    const selectedColor = colorPicker.value;
    const rgb = hexToRgb(selectedColor);
    
    // Setze RGB-Werte in die jeweiligen Eingabefelder
    document.getElementById('red').value = rgb.r;
    document.getElementById('green').value = rgb.g;
    document.getElementById('blue').value = rgb.b;
});

// Aktualisiert das 'time'-Eingabefeld basierend auf Stunden und Minuten
function updateTimeField(hour, minute) {
    const timeField = document.getElementById("time");
    const formattedHour = String(hour).padStart(2, "0");
    const formattedMinute = String(minute).padStart(2, "0");
    timeField.value = `${formattedHour}:${formattedMinute}`;
}

// Event-Listener für Änderungen im 'time'-Eingabefeld
document.getElementById("time").addEventListener("input", function () {
    const [hour, minute] = this.value.split(":").map(Number);
    document.getElementById("hour").value = hour;
    document.getElementById("minute").value = minute;
});

// POST-Request für das Konfigurationsformular
document.getElementById("configForm").addEventListener("submit", function (event) {
    event.preventDefault();

    // Abrufen von Schwellenwert, Farbe und Zeitkonfigurationen
    const tempThreshold = document.getElementById("tempThreshold").value || 0;
    const red = document.getElementById("red").value;
    const green = document.getElementById("green").value;
    const blue = document.getElementById("blue").value;
    const hour = document.getElementById("hour").value || 0;
    const minute = document.getElementById("minute").value || 0;

    // Wochentage als eine Zahl kodieren
    let days = 0;
    document.querySelectorAll("#days input[type=checkbox]").forEach((checkbox) => {
        if (checkbox.checked) {
            days |= parseInt(checkbox.value);
        }
    });

    // Formulardaten erstellen
    const data = `temp_threshold=${encodeURIComponent(tempThreshold)}&red=${encodeURIComponent(red)}&green=${encodeURIComponent(green)}&blue=${encodeURIComponent(blue)}&hour=${encodeURIComponent(hour)}&minute=${encodeURIComponent(minute)}&days=${encodeURIComponent(days)}`;

    // POST-Request an den config_set_handler
    fetch("/config_set", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: data
    })
        .then(response => response.text())
        .then(result => {
            console.log("Antwort vom Server:", result);
            alert(result); // Rückmeldung an den Benutzer
        })
        .catch(error => console.error("Fehler beim Senden der Konfiguration:", error));
});
