document.addEventListener("DOMContentLoaded", function (event) {

  var temperature = new JustGage({
    id: "temperature",
    title: "Temperature",
    value: {temperature},
    min: 0,
    max: 100,
    decimals: 0,
    gaugeWidthScale: 0.6
  });

  var humiduti = new JustGage({
    id: "humiduti",
    title: "Humiduti",
    value: {humiduti},
    min: 0,
    max: 100,
    decimals: 0,
    gaugeWidthScale: 0.6
  });

});
