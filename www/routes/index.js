var express = require('express');
var router = express.Router();
const https = require("https");
const http = require("http");
const csvtojson = require("csvtojson");
const fs = require("fs");






router.get('/', function(req, res, next) {
	if(req.useragent["isMobile"])
		var page = "mobile/index";
	else
		var page = "index"

	res.render(page, { title: 'Visu-MF' });
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
			.then((resText) => resText.text())
			.then((text) => {
				if(req.useragent["isMobile"])
					var page = "mobile/texte";
				else
					var page = "texte"

				res.render(page, { title: "NSMT-40", texte: text.replaceAll(/(?:\r\n|\r|\n)/g, '<br>') });
			});
	}
);


module.exports = router;
