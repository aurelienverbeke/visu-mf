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



proj4.defs("EPSG:3106001", "+title=thomas +proj=stere +lat_0=90 +lat_ts=45 +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs");
register(proj4);
window.projection = new Projection({ code: "EPSG:3106001" });

window.map = new Map({
	target: "map",
	layers: [ new TileLayer({ source: new OSM(), visible: true, toujoursAfficher: true}) ],
	view: new View({ center: transform([148847.926, -4294818.338], projection, "EPSG:3857"), zoom: 5.5 })
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


function ajouter_mosa(heure) {
	map.addLayer(
		new ImageLayer({
			source: new Static({
				url: "/images/mosaiques/z/" + heure + ".png",
				imageExtent: [ -619652.074, -5062818.338, 916347.926, -3526818.338 ],
				projection: projection,
				interpolate: false
			}),
			opacity: 0.5,
			visible: false,
			heure: heure,
			toujoursAfficher: false
		})
	);
}



window.ajouter_mosa = ajouter_mosa;
