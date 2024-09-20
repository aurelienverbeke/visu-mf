let url = window.location.href;
if(url.at(-1) == "/")
	url = url.slice(-9, -1);
else
	url = url.slice(-8);
