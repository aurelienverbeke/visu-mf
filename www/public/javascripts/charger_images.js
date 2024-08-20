for ( let element of document.getElementsByClassName("heure-dispo") ) {
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



for ( let element of document.getElementsByClassName("heure-non-dispo") ) {
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
