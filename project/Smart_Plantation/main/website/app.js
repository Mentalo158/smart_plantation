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

// Funktion, um den ADC-Wert auf der Seite zu aktualisieren
function updateAdcValue() {
    fetchAdcValue().then(adcValue => {
        if (adcValue !== null) {
            // Aktualisiere den Fortschrittsbalken
            const moistureProgress = document.getElementById('moistureProgress');
            moistureProgress.style.width = `${adcValue}%`; // Setze die Breite basierend auf dem ADC-Wert

            // Zeige den Wert als Text an (optional)
            moistureProgress.textContent = `Moisture Level: ${adcValue.toFixed(2)}%`;
        }
    });
}

// Rufe die Funktion alle 1 Sekunde auf, um den Wert zu aktualisieren
setInterval(updateAdcValue, 1000);
