// Funktion, um den aktuellen ADC-Wert vom Server abzurufen
async function fetchAdcValue() {
    try {
        const response = await fetch('/adc'); // Anfrage an die API
        if (response.status === 200) {
            const data = await response.json(); // Ergebnis als JSON parsen
            return data.adc_value; // ADC-Wert zurückgeben
        } else {
            console.warn('No ADC data available.');
            return null;
        }
    } catch (error) {
        console.error('Error fetching ADC value:', error);
        return null;
    }
}

// Funktion, um Temperatur- und Feuchtigkeitswerte vom Server abzurufen
async function fetchTemperatureAndHumidity() {
    try {
        const response = await fetch('/temperature'); // Anfrage an die API
        if (response.status === 200) {
            const data = await response.json(); // Ergebnis als JSON parsen
            return {
                temperature: data.temperature,
                humidity: data.humidity,
            }; // Temperatur und Luftfeuchtigkeit zurückgeben
        } else {
            console.warn('No temperature and humidity data available.');
            return null;
        }
    } catch (error) {
        console.error('Error fetching temperature and humidity:', error);
        return null;
    }
}

// Funktion, um die Werte auf der Seite zu aktualisieren
async function updateValues() {
    const adcValue = await fetchAdcValue();
    if (adcValue !== null) {
        // Aktualisiere den Fortschrittsbalken
        const moistureProgress = document.getElementById('moistureProgress');
        moistureProgress.style.width = `${adcValue}%`; // Setze die Breite basierend auf dem ADC-Wert
        moistureProgress.textContent = `Moisture Level: ${adcValue.toFixed(2)}%`;
    }

    const tempHumidityData = await fetchTemperatureAndHumidity();
    if (tempHumidityData) {
        // Aktualisiere Temperatur- und Feuchtigkeitsanzeigen
        document.getElementById('temperatureDisplay').textContent = `Temperatur: ${tempHumidityData.temperature.toFixed(2)} °C`;
        document.getElementById('humidityDisplay').textContent = `Luftfeuchtigkeit: ${tempHumidityData.humidity.toFixed(2)} %`;
    }
}

// Rufe die Funktion alle 1 Sekunde auf, um die Werte zu aktualisieren
setInterval(updateValues, 1000);
