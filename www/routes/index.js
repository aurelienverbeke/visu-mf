var express = require('express');
var router = express.Router();
const https = require("https");
const http = require("http");
const csvtojson = require("csvtojson");
const fs = require("fs");






router.get('/', function(req, res, next) {
  res.render('index', { title: 'Visu-MF' });
});





router.get("/metadonnees-radar",
	(req, res) => {
		fs.readFile(
			process.env.CHEMIN_METADONNEES_RADAR,
			(err, data) => {
				if(err)
					throw err;
				else
					csvtojson({ delimiter: ";" }).fromString(data.toString()).then((json) => { res.json(json); });
			}
		);
	}
);





router.get("/nsmt-40",
	(req, res) => {
		fetch("http://www.meteo.fr/extranets-alim/BULLETINS/Special/etat_radar.txt")
			.then((res) => res.text())
			.then((text) => res.render("texte", { title: "NSMT-40", texte: text.replaceAll(/(?:\r\n|\r|\n)/g, '<br>') }));
	}
);


module.exports = router;
