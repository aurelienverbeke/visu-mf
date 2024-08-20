/*
const map = new ol.Map({
	target: "map",
	layers: [
		new ol.layer.Tile({source: new ol.source.OSM()}),
		new ol.layer.Image({source: new ol.source.ImageStatic({
					url: "/images/image.png",
					imageExtent: ol.proj.transformExtent([ -9.965, 38.252758, 7.516418, 53.67 ], "EPSG:4326", "EPSG:3857")
				})})
	],
	view: new ol.View({ center: ol.proj.fromLonLat([0.004799, 46.295436]), zoom: 6 })
});
*/

const proj4 = require("proj4");

proj4.defs["EPSG:3106001"]="+title=thomas +proj=stere +lat_0=90 +lat_ts=45 +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs";
projection = new ol.Projection("EPSG:3106001");
var limitesRadar= new ol.Bounds(-619652.074,-5062818.338,916347.926,-3526818.338);
var tailleRadar=new OpenLayers.Size(1536,1536);

const map = new ol.Map({
	target: "map",
	layers: [
		new ol.layer.Tile({source: new ol.source.OSM()}),
		new ol.layer.Image({source: new ol.source.ImageStatic({
							url: "/images/image.png",
							imageExtent: limitesRadar
						}),
					opacity: 0.5})
	],
	view: new ol.View({ projection: projection, center: [0, 45], zoom: 6 })
});
