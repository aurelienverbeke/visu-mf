import Map from 'ol/Map.js';
import OSM from 'ol/source/OSM.js';
import TileLayer from 'ol/layer/Tile.js';
import View from 'ol/View.js';
import {fromLonLat} from 'ol/proj';
import ZoomToExtent from 'ol/control/ZoomToExtent.js';
import VectorLayer from 'ol/layer/Vector.js';
import VectorSource from 'ol/source/Vector.js';
import Feature from 'ol/Feature.js';
import Point from 'ol/geom/Point.js';
import Overlay from 'ol/Overlay.js';
import {Circle, Fill, Stroke, Style} from 'ol/style.js';





/*
 *
 * Creation de la carte
 *
 */

// overlay de detail sur la station
let overlay = new Overlay({
	element: document.getElementById("popup"),
	autoPan: {
		animation: { duration: 250 }
	}
});

let map = new Map({
	target: "map",
	layers: [
		new TileLayer({ source: new OSM() }),
		new VectorLayer({
			source: new VectorSource(),
			style: new Style({
				image: new Circle({
					fill: new Fill({ color: [255, 255, 255, 0.5] }),
					stroke: new Stroke({
						color: "#007bff",
						width: 2
					}),
					radius: 6
				})
			})
		})
	],
	overlays: [ overlay ],
	view: new View({ center: fromLonLat([2, 46.5]), zoom: 5.9 })
});

map.addControl(new ZoomToExtent({ extent: map.getView().calculateExtent(map.getSize()) }));

// ajout des stations a la carte
fetch("/obs/liste-stations")
	.then((res) => res.json())
	.then((stations) => {
		stations.forEach((station) => {
			map.getLayers().item(1).getSource().addFeature(new Feature({
				geometry: new Point(fromLonLat([parseFloat(station["Longitude"]), parseFloat(station["Latitude"])])),
				id: station["Id_station"],
				nom: station["Nom_usuel"]
			}));
		});
	});

// curseur en "petite main" quand on peut cliquer sur la station
map.on("pointermove", (event) => {
	if(map.hasFeatureAtPixel(event.pixel))
		map.getTargetElement().style.cursor = "pointer";
	else
		map.getTargetElement().style.cursor = "";
});





/*
 *
 * Gestion de l'overlay des stations
 *
 */

function fermer_overlay () {
	overlay.setPosition(undefined);
	boutonFermer.blur();
}

let boutonFermer = document.getElementById("fermeture-popup");

map.on("click", (event) => {
	let featuresSousClic = map.getFeaturesAtPixel(event.pixel);
	if(featuresSousClic.length==1) {
		let nomStation = featuresSousClic[0].get("nom");
		document.getElementById("nom-station-popup").innerHTML = `Station : ${nomStation}`;
		document.getElementById("lien-6-min-popup").href = `/obs/tableau-6-min/${featuresSousClic[0].get('id')}`;
		overlay.setPosition(event.coordinate);
	}
	else
		fermer_overlay();
});

boutonFermer.addEventListener("click", () => { fermer_overlay() });





/*
 *
 * Recherche d'un lieu
 *
 */

document.getElementById("lancer-recherche").addEventListener(
	"click",
	(event) => {
		let adresseRecherchee = document.getElementById("barre-recherche").value;

		fetch(`https://nominatim.openstreetmap.org/search?q=${encodeURIComponent(adresseRecherchee)}&format=json`)
			.then((res) => res.json())
			.then((json) => {
				if(json.length > 0) {
					map.getView().setCenter(fromLonLat([parseFloat(json[0]["lon"]), parseFloat(json[0]["lat"])]));
					map.getView().setZoom(1.8*Math.pow(parseInt(json[0]["place_rank"]), 0.7));
				}
			});
	}
);

document.getElementById("barre-recherche").addEventListener(
	"keyup",
	(event) => {
		if(event.keyCode == 13)
			document.getElementById("lancer-recherche").dispatchEvent(new MouseEvent("click"));
	}
);

// initialisation
document.getElementById("barre-recherche").value = "";
