document.addEventListener("DOMContentLoaded", () => {
    fetch("/config")
        .then(response => response.json())
        .then(config => {
            console.log("Config geladen:", config);

            if (config.days === undefined) config.days = 0;

            const coolingButton = document.getElementById("toggleCooling");
            coolingButton.textContent = (config.temp_enabled ? "Kühlung ausschalten" : "Kühlung einschalten").trim();

            const moistureButton = document.getElementById("toggleMoisture");
            moistureButton.textContent = (config.moisture_enabled ? "Automatische Bewässerung ausschalten" : "Automatische Bewässerung einschalten").trim();

            document.getElementById("tempThreshold").value = config.temp_threshold || 0;
            document.getElementById("moistureThreshold").value = config.moisture_threshold || 0;
            document.getElementById("lightIntensity").value = config.light_intensity || 0;
            document.getElementById("luxValue").value = config.luminance || 0;

            document.getElementById("red").value = config.red || 0;
            document.getElementById("green").value = config.green || 0;
            document.getElementById("blue").value = config.blue || 0;
            document.getElementById("colorPicker").value = rgbToHex(config.red || 0, config.green || 0, config.blue || 0);

            const lightModeButton = document.getElementById("switchLightMode");
            lightModeButton.textContent = (config.use_luminance_or_light_intensity ? "Verwende Lux" : "Verwende Intensität").trim();

            const lightOptionButton = document.getElementById("toggleLightOption");
            lightOptionButton.textContent = (config.use_dynamic_lightning ? "Licht Option Aktivieren" : "Licht Option Deaktivieren").trim();

            const dayIds = ["sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"];
            dayIds.forEach((day, index) => {
                const time = config.times?.[index] || { hour: 0, minute: 0 };
                const formattedTime = `${String(time.hour).padStart(2, "0")}:${String(time.minute).padStart(2, "0")}`;
                document.getElementById(`time-${day}`).value = formattedTime;

                const checkbox = document.getElementById(day);
                checkbox.checked = !!(config.days & (1 << index));
            });

            // Initiale Aktivierung/Deaktivierung von Feldern
            initializeFieldStates(config);
        })
        .catch(error => {
            console.error("Fehler beim Laden der Konfiguration:", error);
        });
});

function initializeFieldStates(config) {
    // Kühlung
    const coolingEnabled = config.temp_enabled;
    document.getElementById("tempThreshold").disabled = !coolingEnabled;

    // Bewässerung
    const moistureEnabled = config.moisture_enabled;
    document.getElementById("moistureThreshold").disabled = !moistureEnabled;

    // Lichtmodi
    const useLux = config.use_luminance_or_light_intensity;
    document.getElementById("luxValue").disabled = !useLux;
    document.getElementById("lightIntensity").disabled = useLux;

    // Dynamisches Licht
    const useDynamicLighting = config.use_dynamic_lightning;
    document.getElementById("luxValue").disabled = useDynamicLighting;
    document.getElementById("lightIntensity").disabled = useDynamicLighting;
}

function rgbToHex(r, g, b) {
    return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1).toUpperCase();
}

function hexToRgb(hex) {
    const bigint = parseInt(hex.slice(1), 16);
    return {
        r: (bigint >> 16) & 255,
        g: (bigint >> 8) & 255,
        b: bigint & 255
    };
}

document.getElementById('colorPicker').addEventListener('input', function () {
    const selectedColor = document.getElementById('colorPicker').value.trim();
    const rgb = hexToRgb(selectedColor);
    document.getElementById('red').value = rgb.r || 0;
    document.getElementById('green').value = rgb.g || 0;
    document.getElementById('blue').value = rgb.b || 0;
});

// Kühlung-Toggle
document.getElementById("toggleCooling").addEventListener("click", function () {
    const coolingButton = document.getElementById("toggleCooling");
    const currentState = coolingButton.textContent.trim();
    const newState = currentState === "Kühlung einschalten" ? "Kühlung ausschalten" : "Kühlung einschalten";
    coolingButton.textContent = newState;

    // Aktivieren/Deaktivieren des Temperatur-Eingabefeldes
    document.getElementById("tempThreshold").disabled = newState === "Kühlung einschalten";
});

// Automatische Bewässerung-Toggle
document.getElementById("toggleMoisture").addEventListener("click", function () {
    const moistureButton = document.getElementById("toggleMoisture");
    const currentState = moistureButton.textContent.trim();
    const newState = currentState === "Automatische Bewässerung einschalten" ? "Automatische Bewässerung ausschalten" : "Automatische Bewässerung einschalten";
    moistureButton.textContent = newState;

    // Aktivieren/Deaktivieren des Moisture-Eingabefeldes
    document.getElementById("moistureThreshold").disabled = newState === "Automatische Bewässerung einschalten";
});

// Lichtmodus-Button
document.getElementById('switchLightMode').addEventListener('click', function () {
    const lightModeButton = document.getElementById('switchLightMode');
    const currentMode = lightModeButton.textContent.trim();
    const newMode = currentMode === "Verwende Lux" ? "Verwende Intensität" : "Verwende Lux";
    lightModeButton.textContent = newMode;

    // Umschalten der Aktivierung zwischen Lux- und Lichtintensitäts-Eingabefeldern
    const useLux = newMode === "Verwende Lux";
    document.getElementById('luxValue').disabled = !useLux;
    document.getElementById('lightIntensity').disabled = useLux;
});

// Lichtoption-Toggle
document.getElementById('toggleLightOption').addEventListener('click', function () {
    const lightOptionButton = document.getElementById('toggleLightOption');
    const currentState = lightOptionButton.textContent.trim();
    const newState = currentState === "Licht Option Deaktivieren" ? "Licht Option Aktivieren" : "Licht Option Deaktivieren";
    lightOptionButton.textContent = newState;

    // Deaktivieren beider Felder, wenn dynamisches Licht aktiviert ist
    const useDynamicLighting = newState === "Licht Option Aktivieren";
    document.getElementById('luxValue').disabled = useDynamicLighting;
    document.getElementById('lightIntensity').disabled = useDynamicLighting;
});

// Formular-Submit
document.getElementById("configForm").addEventListener("submit", function (event) {
    event.preventDefault();

    const tempThreshold = parseInt(document.getElementById("tempThreshold").value.trim()) || 0;
    const moistureThreshold = parseInt(document.getElementById("moistureThreshold").value.trim()) || 0;
    const lightIntensity = parseInt(document.getElementById("lightIntensity").value.trim()) || 0;
    const luxValue = parseInt(document.getElementById("luxValue").value.trim()) || 0;

    const red = parseInt(document.getElementById("red").value.trim()) || 0;
    const green = parseInt(document.getElementById("green").value.trim()) || 0;
    const blue = parseInt(document.getElementById("blue").value.trim()) || 0;

    const dayIds = ["sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"];
    const hours = [];
    const minutes = [];
    dayIds.forEach(day => {
        const [hour, minute] = document.getElementById(`time-${day}`).value.trim().split(":").map(Number);
        hours.push(hour || 0);
        minutes.push(minute || 0);
    });

    let days = 0;
    dayIds.forEach((day, index) => {
        const checkbox = document.getElementById(day);
        if (checkbox.checked) {
            days |= (1 << index);
        }
    });

    const coolingButton = document.getElementById("toggleCooling").textContent.trim() === "Kühlung ausschalten";
    const useLuminanceOrLightIntensity = document.getElementById('switchLightMode').textContent.trim() === "Verwende Lux";
    const useDynamicLighting = document.getElementById('toggleLightOption').textContent.trim() === "Licht Option Aktivieren";
    const moistureButton = document.getElementById("toggleMoisture").textContent.trim() === "Automatische Bewässerung ausschalten";

    const data = {
        temp_threshold: tempThreshold,
        moisture_threshold: moistureThreshold,
        temp_enabled: coolingButton,
        moisture_enabled: moistureButton,
        light_intensity: lightIntensity,
        luminance: luxValue,
        use_luminance_or_light_intensity: useLuminanceOrLightIntensity,
        use_dynamic_lightning: useDynamicLighting,
        red: red,
        green: green,
        blue: blue,
        hours: hours,
        minutes: minutes,
        days: days
    };

    fetch("/config_set", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    })
        .then(response => response.text())
        .then(result => {
            console.log("Antwort vom Server:", result);
            alert(result);
        })
        .catch(error => console.error("Fehler beim Senden der Konfiguration:", error));
});
