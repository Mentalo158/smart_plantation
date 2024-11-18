document.addEventListener("DOMContentLoaded", () => {
    fetch("/config")
        .then(response => response.json())
        .then(config => {
            // Debugging: Logge die geladene Konfiguration
            console.log("Config geladen:", config);

            if (config.days === undefined) config.days = 0; // Default für Days

            // Temperatur und RGB-Werte laden
            document.getElementById("tempThreshold").value = config.temp_threshold || 0;
            document.getElementById("red").value = config.red || 255;
            document.getElementById("green").value = config.green || 255;
            document.getElementById("blue").value = config.blue || 255;

            // RGB in Hex umwandeln
            const hexColor = rgbToHex(config.red, config.green, config.blue);
            document.getElementById("colorPicker").value = hexColor;

            const dayIds = ["sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"];
            dayIds.forEach((day, index) => {
                // Uhrzeit für den jeweiligen Wochentag setzen
                const hour = config.times[index].hour || 0; // Hole die Stunde aus times
                const minute = config.times[index].minute || 0; // Hole die Minute aus times
                const formattedTime = `${String(hour).padStart(2, "0")}:${String(minute).padStart(2, "0")}`;
                document.getElementById(`time-${day}`).value = formattedTime;

                // Wochentage anhand der Bitmaske auswählen
                const checkbox = document.getElementById(day);
                if (config.days & (1 << index)) {
                    checkbox.checked = true; // Setze Checkbox, wenn der Tag aktiv ist
                } else {
                    checkbox.checked = false; // Andernfalls Checkbox deaktivieren
                }
            });
        })
        .catch(error => {
            console.error("Fehler beim Laden der Konfiguration:", error);
        });
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

// POST-Request für das Konfigurationsformular
document.getElementById("configForm").addEventListener("submit", function (event) {
    event.preventDefault();

    // Abrufen von Schwellenwert, Farbe und Zeitkonfigurationen
    const tempThreshold = document.getElementById("tempThreshold").value || 0;
    const red = document.getElementById("red").value;
    const green = document.getElementById("green").value;
    const blue = document.getElementById("blue").value;

    // Erfasse Stunden und Minuten für jeden Wochentag
    const dayIds = ["sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"];
    const hours = [];
    const minutes = [];
    dayIds.forEach((day) => {
        const [hour, minute] = document.getElementById(`time-${day}`).value.split(":").map(Number);
        hours.push(hour);
        minutes.push(minute || 0); // Wenn Minute `null` oder `undefined`, auf 0 setzen
    });

    // Wochentage als eine Zahl kodieren (Bitmaske)
    let days = 0;
    dayIds.forEach((day, index) => {
        const checkbox = document.getElementById(day);
        if (checkbox.checked) {
            days |= (1 << index);  // Setze das entsprechende Bit für den Wochentag
        }
    });

    // Erstelle das JSON-Objekt
    const data = {
        temp_threshold: tempThreshold,
        red: red,
        green: green,
        blue: blue,
        hours: hours,
        minutes: minutes,
        days: days
    };

    // POST-Request an den config_set_handler senden
    fetch("/config_set", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    })
        .then(response => response.text())
        .then(result => {
            console.log("Antwort vom Server:", result);
            alert(result); // Rückmeldung an den Benutzer
        })
        .catch(error => console.error("Fehler beim Senden der Konfiguration:", error));
});
