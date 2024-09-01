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
		new VectorLayer({ source: new VectorSource() })
	],
	overlays: [ overlay ],
	view: new View({ center: fromLonLat([2, 46.5]), zoom: 6.3 })
});

map.addControl(new ZoomToExtent({ extent: map.getView().calculateExtent(map.getSize()) }));






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






map.on("pointermove", (event) => {
	if(map.hasFeatureAtPixel(event.pixel))
		map.getTargetElement().style.cursor = "pointer";
	else
		map.getTargetElement().style.cursor = "";
});






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
		overlay.setPosition(event.coordinate);
	}
	else
		fermer_overlay();
});

boutonFermer.addEventListener( "click", () => { fermer_overlay() });
