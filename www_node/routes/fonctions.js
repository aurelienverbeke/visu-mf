function recuperer_token () {
	return fetch("https://portail-api.meteofrance.fr/token", {
			method: "POST",
			headers: { "Authorization": `Basic ${process.env.APPLICATION_ID}`},
			body: "grant_type=client_credentials"
		})
		.then((res) => res.json())
		.then((json) => json["access_token"]);
}



module.exports = recuperer_token;
