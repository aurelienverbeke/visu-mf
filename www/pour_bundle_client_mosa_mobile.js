const proj4 = require("proj4");

import Map from 'ol/Map.js';
import OSM from 'ol/source/OSM.js';
import TileLayer from 'ol/layer/Tile.js';
import View from 'ol/View.js';
import ImageLayer from 'ol/layer/Image.js';
import Static from 'ol/source/ImageStatic.js';
import Projection from 'ol/proj/Projection.js';
import Size from 'ol/size';
import VectorLayer from 'ol/layer/Vector.js';
import VectorSource from 'ol/source/Vector.js';
import Style from 'ol/style/Style.js';
import Text from 'ol/style/Text.js';
import Fill from 'ol/style/Fill.js';
import Feature from 'ol/Feature.js';
import Point from 'ol/geom/Point.js';
import ZoomToExtent from 'ol/control/ZoomToExtent.js';
import { fromLonLat } from 'ol/proj';
import { register } from 'ol/proj/proj4';
import { transform } from 'ol/proj';
import Icon from 'ol/style/Icon.js';
import Overlay from 'ol/Overlay.js';

const csvtojson = require("csvtojson");

import Sortable from 'sortablejs';





// on met la bonne page en surbrillance dans la barre de navigation
if(type == "z")
	document.getElementById("nav-mosa-z").classList.add("active");
if(type == "lame_eau")
	document.getElementById("nav-mosa-lame-eau").classList.add("active");





/*
 *
 * Creation de la carte
 *
 */

// overlay d'affichage du radar quand clic dessus
let overlay = new Overlay({
        element: document.getElementById("popup"),
        autoPan: {
                animation: { duration: 250 }
        }
});



// projection imposee par la pixmap meteo-france
proj4.defs("EPSG:3106001", "+title=thomas +proj=stere +lat_0=90 +lat_ts=45 +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs");
register(proj4);
window.projection = new Projection({ code: "EPSG:3106001" });



// carte
window.map = new Map({
	target: "map",
	layers: [
		new TileLayer({ source: new OSM(), visible: true, toujoursAfficher: true, zIndex: 0, groupe: "fond_carte", nom: "fond_carte"}),
		new VectorLayer({
			source: new VectorSource(),
			style: new Style({
				image: new Icon({
					src: "/images/radar.svg",
					width: 20,
					height: 20,
					color: [0, 120, 0, 1]
				})
			}),
			visible: true,
			toujoursAfficher: false,
			nom: "radars_presents",
			groupe: "radars_filigrane",
			zIndex: 1
		}),
		new VectorLayer({
			source: new VectorSource(),
			style: new Style({
				image: new Icon({
					src: "/images/radar.svg",
					width: 20,
					height: 20,
					color: [140, 0, 0, 1]
				})
			}),
			visible: true,
			toujoursAfficher: false,
			nom: "radars_absents",
			groupe: "radars_filigrane",
			zIndex: 1
		})
	],
	overlays: [ overlay ],
	view: new View({ center: transform([148847, -4360000], projection, "EPSG:3857"), zoom: 5 })
});



// bouton de remise a zero de la vue
map.addControl(new ZoomToExtent({ extent: map.getView().calculateExtent(map.getSize()) }));



// filigrane d'image manquante
map.addLayer(
	new VectorLayer({
		source: new VectorSource({
			features: [ new Feature({
				geometry: new Point(map.getView().getCenter()),
				nom: "filigrane"
			}) ]
		}),
		style: new Style({
			text: new Text({
				font: "50px sans-serif",
				text: "Image\nindisponible",
				fill: new Fill({color: "black"})
			})
		}),
		toujoursAfficher: false,
		visible: false,
		nom: "filigrane",
		groupe: "radars_filigrane",
		zIndex: 1
	})
);



// actualisation de la position du filigrane quand la carte est bougee
map.on(
	"moveend",
	(event) => {
		map.getLayers().forEach((layer) => {
			if(layer.get("nom") == "filigrane")
				layer.getSource().getFeatures()[0].getGeometry().setCoordinates(map.getView().getCenter());
		})
	}
);



// affichage et fermeture de l'overlay de details sur le radar clique
function fermer_overlay () {
        overlay.setPosition(undefined);
        boutonFermer.blur();
}

let boutonFermer = document.getElementById("fermeture-popup");

map.on("click", (event) => {
        let featuresSousClic = map.getFeaturesAtPixel(event.pixel);
        if(featuresSousClic.length==1) {
                let nomRadar = featuresSousClic[0].get("nom");
                document.getElementById("nom-radar-popup").innerHTML = `${nomRadar}`;
                overlay.setPosition(event.coordinate);
        }
        else
                fermer_overlay();
});

boutonFermer.addEventListener( "click", () => { fermer_overlay() });





/*
 *
 * Recuperation de la liste des radars et de ceux presents dans la mosaique
 * On utilise des XMLHttpRequest pour bloquer l'execution tant qu'on n'a pas recu les reponses
 *
 */

// liste des radars
let req = new XMLHttpRequest();
req.onreadystatechange = () => {
	if(req.readyState == 4)
		JSON.parse(req.response).forEach((radar) => {
			map.getLayers().item(1).getSource().addFeature(new Feature({
				geometry: new Point(fromLonLat([parseFloat(radar["lon"]), parseFloat(radar["lat"])])),
				omm: radar["omm"],
				nom: radar["nom"]
			}));
		});
};
req.open("GET", "/metadonnees-radar", false);
req.send();



// radars presents dans la mosaique
let radarsPresents = undefined;

req = new XMLHttpRequest();
req.onreadystatechange = () => {
	if(req.readyState == 4)
		radarsPresents = JSON.parse(req.response);
};
req.open("GET", "/donnees/mosaiques/" + type + "/radars_presents.json", false);
req.send();





/*
 *
 * Ajout des images et des changements d'images quand on selectionne une heure
 * Il faut mettre a jour les radars presents
 *
 */

let selectionHeure = document.getElementById("selection-heure");

// image existante
for(let element of document.getElementsByClassName("heure-dispo")) {
	map.addLayer(
		new ImageLayer({
			source: new Static({
				url: "/donnees/mosaiques/" + type + "/" + element.id + ".png",
				imageExtent: [ -619652.074, -5062818.338, 916347.926, -3526818.338 ],
				projection: projection,
				interpolate: false
			}),
			visible: false,
			heure: element.id,
			toujoursAfficher: false,
			groupe: "image",
			zIndex: 2,
			nom: "image"
		})
	);
}

selectionHeure.addEventListener(
	"change",
	(event) => {
		let heureSelectionnee = selectionHeure.selectedOptions[0];
		let heureDispo = heureSelectionnee.classList.contains("heure-dispo");

		if(heureDispo) {
			map.getLayers().forEach((layer) => {
				if(layer.get("heure") == heureSelectionnee.id || layer.get("nom") == "radars_presents" || layer.get("nom") == "radars_absents")
					layer.set("visible", true);
				else {
					if(layer.get("toujoursAfficher") == false)
						layer.set("visible", false);
				}
			});

			let radarsPresentsAncien = map.getLayers().item(1).getSource().getFeatures();
			let radarsAbsentsAncien = map.getLayers().item(2).getSource().getFeatures();
			let radarsPresentsNouveau = [];
			let radarsAbsentsNouveau = [];

			let trier = (radar) => {
				if(radarsPresents[heureSelectionnee.id].includes(parseInt(radar.get("omm")))) {
					radarsPresentsNouveau.push(radar.clone());
				}
				else {
					radarsAbsentsNouveau.push(radar.clone());
				}
			};

			radarsPresentsAncien.forEach(trier);
			radarsAbsentsAncien.forEach(trier);

			map.getLayers().item(1).getSource().clear();
			map.getLayers().item(2).getSource().clear();

			map.getLayers().item(1).getSource().addFeatures(radarsPresentsNouveau);
			map.getLayers().item(2).getSource().addFeatures(radarsAbsentsNouveau);
		}

		else {
			map.getLayers().forEach((layer) => {
				if(layer.get("toujoursAfficher") == false)
					layer.set("visible", false);
				if(layer.get("nom") == "filigrane")
					layer.set("visible", true);
			});
		}
	}
);





/*
 *
 * Gestion de l'animation des images
 *
 */

window.idInterval = undefined;

function lancer_animation () {
	if(idInterval)
		clearInterval(idInterval);

	// indices des deux heures choisies
	let indiceHeureDebut = document.getElementById("selection-debut").selectedIndex;
	let indiceHeureFin = document.getElementById("selection-fin").selectedIndex;
	
	// indices des options dans la selection d'heure qui sont entre les deux heures choisies
	// let images = Array.from({ length: indiceHeureFin-indiceHeureDebut+1 }, (_,i) => i+indiceHeureDebut);

	let indiceEnCours = indiceHeureFin;
	let indicePrecedent = indiceHeureFin - 1;
	idInterval = setInterval(
		() => {
			selectionHeure.selectedIndex = indiceEnCours;
			selectionHeure.dispatchEvent(new Event("change"));
			indicePrecedent = indiceEnCours;
			indiceEnCours++;
			if(indiceEnCours > indiceHeureFin)
				indiceEnCours = indiceHeureDebut;
		},
		500 / Math.sqrt(document.getElementById("vitesse").valueAsNumber) // une fonction qui marche pas mal
	);
}

document.getElementById("bouton-lancer").addEventListener("click", (event) => {
	if(idInterval) {
		clearInterval(idInterval);
		idInterval = undefined;
		
		document.getElementById("icone-pause").hidden = true;
		document.getElementById("icone-lancer").hidden = false;
	}
	
	else {
		document.getElementById("icone-lancer").hidden = true;
		document.getElementById("icone-pause").hidden = false;
		
		lancer_animation();
	}
});

document.getElementById("vitesse").addEventListener(
	"input",
	(event) => {
		if(idInterval)
			lancer_animation();
	}
);

// initialisation
document.getElementById("vitesse").valueAsNumber = 1;
document.getElementById("selection-fin").selectedIndex = document.getElementsByClassName("heure").length - 1;
document.getElementById("selection-debut").selectedIndex = 0;





/*
 *
 * Configuration des couches
 *
 */

// ordre
Sortable.create(document.getElementById("liste-couches"), {
	handle: ".poignee-couches",
	animation: 150,
	onUpdate: (event) => {
		let ordreCouches = {};

		let listeHtml = document.getElementById("liste-couches").children;

		for(let i=0 ; i<listeHtml.length ; i++) {
			ordreCouches[listeHtml.item(i).id] = i;
		}

		map.getLayers().forEach((layer) => {
			if(layer.get("groupe") == "fond_carte")
				layer.setZIndex(ordreCouches["couche-fond-carte"]);
			else if(layer.get("groupe") == "radars_filigrane")
				layer.setZIndex(ordreCouches["couche-radars"]);
			else if(layer.get("groupe") == "image")
				layer.setZIndex(ordreCouches["couche-image"]);
		});
	}
});

// opacite
document.getElementById("opacite-fond-carte").addEventListener(
	"input",
	(event) => {
		let opacite = document.getElementById("opacite-fond-carte").valueAsNumber;

		map.getLayers().forEach((layer) => {
			if(layer.get("groupe") == "fond_carte")
				layer.setOpacity(opacite);
		});
	}
)
document.getElementById("opacite-fond-carte").valueAsNumber = 0.8;
document.getElementById("opacite-fond-carte").dispatchEvent(new InputEvent("input"));

document.getElementById("opacite-radars").addEventListener(
	"input",
	(event) => {
		let opacite = document.getElementById("opacite-radars").valueAsNumber;

		map.getLayers().forEach((layer) => {
			if(layer.get("groupe") == "radars_filigrane" && layer.get("nom") != "filigrane")
				layer.setOpacity(opacite);
		});
	}
)
document.getElementById("opacite-radars").valueAsNumber = 1;
document.getElementById("opacite-radars").dispatchEvent(new InputEvent("input"));

document.getElementById("opacite-image").addEventListener(
	"input",
	(event) => {
		let opacite = document.getElementById("opacite-image").valueAsNumber;

		map.getLayers().forEach((layer) => {
			if(layer.get("groupe") == "image")
				layer.setOpacity(opacite);
		});
	}
)
document.getElementById("opacite-image").valueAsNumber = 0.8;
document.getElementById("opacite-image").dispatchEvent(new InputEvent("input"));





/*
 *
 * Affichage de la premiere image
 *
 */

selectionHeure.selectedIndex = selectionHeure.length - 1;
selectionHeure.dispatchEvent(new Event("change"));
