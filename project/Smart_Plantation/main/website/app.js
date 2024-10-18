document.addEventListener("htmx:afterSwap", (event) => {
    if (event.target.id === "moistureText") {
        const moistureText = event.target.innerText; // Lese den Text für die Bodenfeuchte
        const moistureValue = parseFloat(moistureText.match(/[\d\.]+/)[0]); // Extrahiere den Feuchtigkeitswert

        // Update progress bar width and color
        const moistureProgress = document.getElementById("moistureProgress");
        moistureProgress.style.width = `${moistureValue}%`;

        // Change color from brown to blue
        const brownToBlueRatio = moistureValue / 100; // Adjust based on your scale
        moistureProgress.style.backgroundColor = `rgb(${165 * (1 - brownToBlueRatio)}, 44, ${255 * brownToBlueRatio})`;
    }
});


        // Color Picker und RGB Werte
        const colorPicker = document.getElementById('colorPicker');
        const colorBox = document.getElementById('colorBox');
        const redInput = document.getElementById('red');
        const greenInput = document.getElementById('green');
        const blueInput = document.getElementById('blue');

        function hexToRgb(hex) {
            const bigint = parseInt(hex.slice(1), 16);
            const r = (bigint >> 16) & 255;
            const g = (bigint >> 8) & 255;
            const b = bigint & 255;
            return { r, g, b };
        }

        colorPicker.addEventListener('input', function () {
            const selectedColor = colorPicker.value;
            const rgb = hexToRgb(selectedColor);
            redInput.value = rgb.r;
            greenInput.value = rgb.g;
            blueInput.value = rgb.b;

            // Setze die Hintergrundfarbe der Box auf die ausgewählte Farbe
            colorBox.style.backgroundColor = selectedColor;
        });