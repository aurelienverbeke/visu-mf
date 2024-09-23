const express = require("express");
const router = express.Router();
const fs = require("fs");
const https = require("https");
const csvtojson = require("csvtojson");
const recuperer_token = require("./fonctions");





router.get("/carte/",
	(req, res) => {
		res.render("obs", { title: "Carte des stations" });
	}
);





router.get("/liste-stations/",
	(req, res) => {
		let listeStations = "";
		
		recuperer_token().then((token) => {
			fetch("https://portail-api.meteofrance.fr/public/DPPaquetObs/v1/liste-stations",
					{ headers: { "accept": "*/*", "Authorization": `Bearer ${token}`} })
				.then((res) => res.text())
				.then((text) => {
					csvtojson({ delimiter: ";" }).fromString(text).then((json) => { res.send(json); });
				});
		});
	}
);





router.get("/tableau-6-min/:id",
	(req, res) => {
		res.render("6_min", { title: "Atmogramme 6 min" });
	}
);





router.get("/6-min/:id",
	(req, res) => {
		recuperer_token().then((token) => {
			let reqListeStations = fetch(`http://localhost:${process.env.PORT}/obs/liste-stations`)
				.then((res) => res.json());

			let reqMesures = fetch(`https://portail-api.meteofrance.fr/public/DPPaquetObs/v1/paquet/infrahoraire-6m?id_station=${req.params.id}&format=json`,
					{ headers: { "accept": "*/*", "Authorization": `Bearer ${token}`} })
				.then((res) => res.json());

			Promise.all([reqListeStations, reqMesures])
				.then((valeurs) => {
					let donnees = { };

					valeurs[0].forEach((station) => {
						if(station["Id_station"] == req.params.id) {
							donnees["station"] = { id: station["Id_station"], nom: station["Nom_usuel"] };
						}
					});

					donnees["donnees"] = valeurs[1].map((periode) => {
						return {
							heure: periode["validity_time"],
							temperature: periode["t"] - 273.15,
							humidite: periode["u"],
							direction_vent: periode["dd"],
							vitesse_vent: periode["ff"] * 3.6,
							direction_vent_rafales: periode["dxi10"],
							vitesse_vent_rafales: periode["fxi10"] * 3.6,
							pluie: periode["rr_per"],
							neige: periode["sss"]
						};
					}).reverse();

					res.json(donnees);
				});
		});
	}
);





module.exports = router;
