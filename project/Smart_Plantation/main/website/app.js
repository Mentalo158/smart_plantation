document.addEventListener("htmx:afterSwap", (event) => {
    if (event.target.id === "moistureText") {
        const moistureText = event.target.innerText;
        const moistureValue = parseFloat(moistureText.match(/[\d\.]+/)[0]);

        // Update progress bar width and color
        const moistureProgress = document.getElementById("moistureProgress");
        moistureProgress.style.width = `${moistureValue}%`;

        // Change color from brown to blue
        const brownToBlueRatio = moistureValue / 100; // Adjust based on your scale
        moistureProgress.style.backgroundColor = `rgb(${165 * (1 - brownToBlueRatio)}, 44, ${255 * brownToBlueRatio})`;
    }
});
