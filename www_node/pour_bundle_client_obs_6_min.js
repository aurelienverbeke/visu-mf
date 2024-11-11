let url = window.location.href;
if(url.at(-1) == "/")
	var idStation = url.slice(-9, -1);
else
	var idStation = url.slice(-8);





fetch(`/obs/6-min/${idStation}`)
	.then((res) => res.json())
	.then((json) => {
		// on ne garde que les 3 derniers heures
		json["donnees"] = json["donnees"].filter((periode) => {
			let heure = Date.parse(periode["heure"]);
			let heureActuelle = Date.now();
			return heureActuelle-heure <= 1000*3600*3;
		});

		//let heures = json["donnees"].map((periode)=>periode["heure"].slice(11, 16));
		let heures = json["donnees"].map((periode)=>Date.parse(periode["heure"]));
		let temperatures = json["donnees"].map((periode) => Math.round(periode["temperature"]*10)/10);
		let pluies = json["donnees"].map((periode) => periode["pluie"]);
		let humidites = json["donnees"].map((periode) => periode["humidite"]);

		let chart = Highcharts.chart("graphique", {
			chart: {
				zooming: {
					type: "xy"
				}
			},
			time: {
				useUTC: false
			},
			title: {
				text: `Station : ${json["station"]["nom"]}`,
				align: "left"
			},
			xAxis: [
				{
					categories: heures,
					crosshair: true,
					type: "datetime",
					labels: {
						format: "{value:%H:%M}"
					}
				}
			],
			yAxis: [
				{
					title: {
						text: "Température",
						style: {
							color: Highcharts.getOptions().colors[1]
						}
					},
					labels: {
						format: "{value}°C",
						style: {
							color: Highcharts.getOptions().colors[1]
						}
					},
					opposite: true,
					softMin: 5,
					softMax: 30,
					tickInterval: 5,
					endOnTick: false
				},
				{
					title: {
						text: "Précipitations",
						style: {
							color: Highcharts.getOptions().colors[0]
						}
					},
					labels: {
						format: "{value} mm",
						style: {
							color: Highcharts.getOptions().colors[0]
						}
					},
					gridLineWidth: 0,
					min: 0,
					softMax: 5,
					maxPadding: 0.2,
					startOnTick: false,
					endOnTick: false
				},
				{
					title: {
						text: "Humidité",
						style: {
							color: Highcharts.getOptions().colors[8]
						}
					},
					labels: {
						format: "{value} %",
						style: {
							color: Highcharts.getOptions().colors[8]
						}
					},
					gridLineWidth: 0,
					softMin: 40,
					max: 100,
					startOnTick: false,
					endOnTick: false,
					height: "28%",
					tickInterval: 20
				}
			],
			tooltip: {
				shared: true
			},
			series: [
				{
					name: "Température",
					type: "spline",
					data: temperatures,
					yAxis: 0,
					color: Highcharts.getOptions().colors[1],
					tooltip: {
						valueSuffix: " °C"
					}
				},
				{
					name: "Précipitations",
					type: "column",
					yAxis: 1,
					data: pluies,
					color: Highcharts.getOptions().colors[0],
					tooltip: {
						valueSuffix: " mm"
					}
				},
				{
					name: "Humidité",
					type: "spline",
					data: humidites,
					yAxis: 2,
					color: Highcharts.getOptions().colors[8],
					tooltip: {
						valueSuffix: " %"
					}
				}
			]
		});
	});
