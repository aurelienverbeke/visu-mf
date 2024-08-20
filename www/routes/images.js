const express = require("express");
const router = express.Router();
const fs = require("fs");





router.get("/mosaiques/z/",
	(req, res) => {
		var heures = [ new Date(Date.now()-7200000) ]; // on enleve 2 heures, l'unite est la ms
		heures[0].setUTCMinutes(heures[0].getUTCMinutes() + (5 - heures[0].getUTCMinutes()%5)); // on se cale sur un multiple de 5
		
		while(heures.at(-1).valueOf() < Date.now()-300000)
			heures.push(new Date(heures.at(-1).valueOf() + 300000));
		
		heures = heures.map((elem) => {
			let annee = elem.getUTCFullYear().toString().padStart(4, "0");
			let mois = (elem.getUTCMonth()+1).toString().padStart(2, "0");
			let jour = elem.getUTCDate().toString().padStart(2, "0");
			let heure = elem.getUTCHours().toString().padStart(2, "0");
			let minute = elem.getUTCMinutes().toString().padStart(2, "0");
			
			return annee+mois+jour+heure+minute;
		});
		
		
		let heuresDispo = fs.readdirSync(process.env.DIR_IMAGES_MOSA_Z).map((fichier) => fichier.substring(0, 12));

		res.render("mosaique", { title: "Mosaïque Z", heures: heures, heures_dispo: heuresDispo, type: "z", echelles: require(process.env.CHEMIN_ECHELLES) });
	}
);

router.get("/mosaiques/lame_eau/",
	(req, res) => {
		var heures = [ new Date(Date.now()-7200000) ]; // on enleve 2 heures, l'unite est la ms
		heures[0].setUTCMinutes(heures[0].getUTCMinutes() + (5 - heures[0].getUTCMinutes()%5)); // on se cale sur un multiple de 5
		
		while(heures.at(-1).valueOf() < Date.now()-300000)
			heures.push(new Date(heures.at(-1).valueOf() + 300000));
		
		heures = heures.map((elem) => {
			let annee = elem.getUTCFullYear().toString().padStart(4, "0");
			let mois = (elem.getUTCMonth()+1).toString().padStart(2, "0");
			let jour = elem.getUTCDate().toString().padStart(2, "0");
			let heure = elem.getUTCHours().toString().padStart(2, "0");
			let minute = elem.getUTCMinutes().toString().padStart(2, "0");
			
			return annee+mois+jour+heure+minute;
		});
		
		
		let heuresDispo = fs.readdirSync(process.env.DIR_IMAGES_MOSA_LAME_EAU).map((fichier) => fichier.substring(0, 12));

		res.render("mosaique", { title: "Mosaïque lame d'eau", heures: heures, heures_dispo: heuresDispo, type: "lame_eau", echelles: require(process.env.CHEMIN_ECHELLES) });
	}
);





module.exports = router;
