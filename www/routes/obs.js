const express = require("express");
const router = express.Router();
const fs = require("fs");
const https = require("https");
const csvtojson = require("csvtojson");





router.get("/carte/",
	(req, res) => {
		res.render("obs", { title: "Carte des stations" });
	}
);





router.get("/liste-stations",
        (req, res) => {
		let listeStations = "";

                let reqToken = https.request(
                        {
                                headers: { "Authorization": `Basic ${process.env.APPLICATION_ID}`},
                                method: "POST",
                                host: "portail-api.meteofrance.fr",
                                path: "/token"
                        },
                        (resToken) => {
                                resToken.on("data", (dataToken) => {
					let token = JSON.parse(dataToken.toString())["access_token"];

					let reqListeStations = https.request(
						{
							headers: { "accept": "*/*", "Authorization": `Bearer ${token}`},
							method: "GET",
							host: "portail-api.meteofrance.fr",
							path: "/public/DPPaquetObs/v1/liste-stations"
						},
						(resListeStations) => {
							resListeStations.on("data", (dataListeStations) => { listeStations += dataListeStations.toString() });
							resListeStations.on("end", () => {
								csvtojson({ delimiter: ";" }).fromString(listeStations).then((json) => { res.send(json); });
							});
						}
					);
					reqListeStations.end();
				});
                        }
                );
                reqToken.write("grant_type=client_credentials");
                reqToken.end();
        }
);





module.exports = router;
