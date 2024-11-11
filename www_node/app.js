#! /usr/bin/env node

var createError = require("http-errors");
var express = require("express");
var path = require("path");
var cookieParser = require("cookie-parser");
var logger = require("morgan");
var https = require("https");
var http = require("http");
var debug = require("debug")("serveur:*");
var fs = require("fs");
var useragent = require("express-useragent")





var app = express();





// view engine setup
app.set("views", path.join(__dirname, "views"));
app.set("view engine", "pug");





app.use(logger("dev"));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, "public")));
app.use(express.static(path.join(__dirname, "node_modules")));
app.use(useragent.express());





var routerIndex = require("./routes/index");
var routerImages = require("./routes/images");
var routerObs = require("./routes/obs");

app.use("/", routerIndex);
app.use("/images", routerImages);
app.use("/obs", routerObs);





// catch 404 and forward to error handler
app.use(function(req, res, next) {
	next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
	// set locals, only providing error in development
	res.locals.message = err.message;
	res.locals.error = req.app.get("env") === "development" ? err : {};

	// render the error page
	res.status(err.status || 500);
	res.render("error");
});





// creation du serveur HTTP

const options = {
	key: fs.readFileSync("serveur.key"),
	cert: fs.readFileSync("serveur.crt"),
};

var port = parseInt(process.env.PORT);
var serveur = http.createServer(app);

serveur.listen(port);

serveur.on("error", (error) => {
	if (error.syscall !== "listen")
		throw error;

	var bind = typeof port === "string"
		? "Pipe " + port
		: "Port " + port;

	switch (error.code) {
		case "EACCES":
			console.error(bind + " necessite des privileges root");
			process.exit(1);
			break;
		case "EADDRINUSE":
			console.error(bind + " est deja utilise");
			process.exit(1);
			break;
		default:
			throw error;
	}
});

serveur.on("listening", () => {
	var adresse = serveur.address();
	
	var bind = typeof adresse === "string"
		? "pipe " + adresse
		: "port " + adresse.port;
	
	debug("Le serveur ecoute sur le " + bind);
});
