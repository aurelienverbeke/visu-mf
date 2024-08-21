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
import { fromLonLat } from 'ol/proj';
import { register } from 'ol/proj/proj4';
import { transform } from 'ol/proj';





if(type == "z")
	document.getElementById("nav-mosa-z").classList.add("active");
if(type == "lame_eau")
	document.getElementById("nav-mosa-lame-eau").classList.add("active");





proj4.defs("EPSG:3106001", "+title=thomas +proj=stere +lat_0=90 +lat_ts=45 +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs");
register(proj4);
window.projection = new Projection({ code: "EPSG:3106001" });

window.map = new Map({
	target: "map",
	layers: [ new TileLayer({ source: new OSM(), visible: true, toujoursAfficher: true}) ],
	view: new View({ center: transform([148847, -4360000], projection, "EPSG:3857"), zoom: 5.9 })
});

map.addLayer(
	new VectorLayer({
		source: new VectorSource({
			features: [ new Feature({ geometry: new Point(map.getView().getCenter()) }) ]
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
		nom: "filigrane"
	})
);

map.on(
	"moveend",
	(event) => {
		map.getLayers().forEach((layer) => {
			if(layer.get("nom") == "filigrane")
				layer.getSource().getFeatures()[0].getGeometry().setCoordinates(map.getView().getCenter());
		})
	}
);

window.ajouter_mosa = (heure) => {
	map.addLayer(
		new ImageLayer({
			source: new Static({
				url: "/images/mosaiques/" + type + "/" + heure + ".png",
				imageExtent: [ -619652.074, -5062818.338, 916347.926, -3526818.338 ],
				projection: projection,
				interpolate: false
			}),
			opacity: 1,
			visible: false,
			heure: heure,
			toujoursAfficher: false
		})
	);
}





for(let element of document.getElementsByClassName("heure-dispo")) {
	let heure = element.id;
	ajouter_mosa(heure);

	element.addEventListener(
		"mouseover",
		(event) => {
			map.getLayers().forEach((layer) => {
				if(layer.get("heure") == heure)
					layer.set("visible", true);
				else {
					if(layer.get("toujoursAfficher") == false)
						layer.set("visible", false);
				}
			});
		}
	);
}

for(let element of document.getElementsByClassName("heure-non-dispo")) {
	let heure = element.id;

	element.addEventListener(
		"mouseover",
		(event) => {
			map.getLayers().forEach((layer) => {
				if(layer.get("toujoursAfficher") == false)
					layer.set("visible", false);
				if(layer.get("nom") == "filigrane")
					layer.set("visible", true);
			});
		}
	);
}




window.idInterval = undefined;

document.getElementById("bouton-lancer").addEventListener(
	"mouseup",
	(event) => {
		let nombreImagesAnimation = parseInt(document.getElementById("selection-profondeur").value) * 12;
		
		let images = Array.from(document.getElementsByClassName("heure"));
		images = images.slice(images.length-nombreImagesAnimation);

		var indiceEnCours = nombreImagesAnimation - 1;
		var indicePrecedent = nombreImagesAnimation - 2;
		idInterval = setInterval(
			() => {
				images[indiceEnCours].dispatchEvent(new MouseEvent("mouseover"));
				images[indicePrecedent].classList.remove("souligne");
				images[indiceEnCours].classList.add("souligne");
				indicePrecedent = indiceEnCours;
				indiceEnCours++;
				if(indiceEnCours == nombreImagesAnimation)
					indiceEnCours = 0;
			},
			500
		);
	}
);

document.getElementById("bouton-pause").addEventListener(
	"mouseup",
	(event) => {
		clearInterval(idInterval);
		idInterval = undefined;

		Array.from(document.getElementsByClassName("heure")).forEach(
			(elem) => { elem.classList.remove("souligne"); }
		);
	}
);
