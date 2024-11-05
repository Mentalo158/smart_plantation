document.addEventListener("DOMContentLoaded", () => {
    // Abrufen und Setzen der gespeicherten Konfigurationswerte beim Laden der Seite
    fetch("/config")
        .then(response => response.json())
        .then(config => {
            // Setze den Temperaturschwellenwert
            document.getElementById("tempThreshold").value = config.temp_threshold;

            // Setze die RGB-Werte in die Farb-Inputs
            document.getElementById("red").value = config.red;
            document.getElementById("green").value = config.green;
            document.getElementById("blue").value = config.blue;

            // Konvertiere RGB in Hex und setze die Farbe im Farb-Picker
            const hexColor = rgbToHex(config.red, config.green, config.blue);
            document.getElementById("colorPicker").value = hexColor;
            document.getElementById("colorBox").style.backgroundColor = hexColor;
        })
        .catch(error => console.error("Fehler beim Laden der Konfiguration:", error));
});

// Funktion zum Umwandeln von RGB in Hex-Format
function rgbToHex(r, g, b) {
    return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1).toUpperCase();
}

// Farb-Update und RGB-Werte für das Farb-Picker-Element
const colorPicker = document.getElementById('colorPicker');
const colorBox = document.getElementById('colorBox');
const redInput = document.getElementById('red');
const greenInput = document.getElementById('green');
const blueInput = document.getElementById('blue');

colorPicker.addEventListener('input', function () {
    const selectedColor = colorPicker.value;
    const rgb = hexToRgb(selectedColor);
    redInput.value = rgb.r;
    greenInput.value = rgb.g;
    blueInput.value = rgb.b;

    // Setze die Hintergrundfarbe der Box auf die ausgewählte Farbe
    colorBox.style.backgroundColor = selectedColor;
});

function hexToRgb(hex) {
    const bigint = parseInt(hex.slice(1), 16);
    const r = (bigint >> 16) & 255;
    const g = (bigint >> 8) & 255;
    const b = bigint & 255;
    return { r, g, b };
}

// POST-Request für das Konfigurationsformular
document.getElementById("configForm").addEventListener("submit", function(event) {
    event.preventDefault();

    // Werte für Temperatur-Schwellenwert und LED-Farben abrufen
    const tempThreshold = document.getElementById("tempThreshold").value || 0;
    const red = redInput.value;
    const green = greenInput.value;
    const blue = blueInput.value;

    // Formulardaten im passenden Format für den Handler
    const data = `temp_threshold=${encodeURIComponent(tempThreshold)}&red=${encodeURIComponent(red)}&green=${encodeURIComponent(green)}&blue=${encodeURIComponent(blue)}`;

    // POST-Request an config_set_handler
    fetch("/config_set", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: data
    })
    .then(response => response.text())
    .then(result => {
        console.log("Antwort vom Server:", result);
        alert(result); // Rückmeldung an den Benutzer
    })
    .catch(error => console.error("Fehler beim Senden der Konfiguration:", error));
});
