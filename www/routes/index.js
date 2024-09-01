var express = require('express');
var router = express.Router();
const https = require("https");
const csvtojson = require("csvtojson");
const fs = require("fs");





router.get('/', function(req, res, next) {
  res.render('index', { title: 'Visu-MF' });
});





router.get("/token",
	(req, res) => {
		let reqMf = https.request(
                        {
                                headers: { "Authorization": `Basic ${process.env.APPLICATION_ID}`},
                                method: "POST",
                                host: "portail-api.meteofrance.fr",
                                path: "/token"
                        },
                        (resMf) => {
                                resMf.on("data", (data) => { res.send(JSON.parse(data.toString())["access_token"]); });
                        }
                );

                reqMf.write("grant_type=client_credentials");
                reqMf.end();
	}
);





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




module.exports = router;
