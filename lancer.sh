#!/bin/bash



source charger_env.sh



function enchainement {
	python3 ${DIR_SRCS}gestion_mf.py
	${DIR_BIN}creer_images
}

function demon {
	while true
	do
		enchainement &
		sleep 60s
	done
}

function fichier_a_jour {
	ancienSha="a"
	
	if [ -e "$1.sha256" ]
	then
		ancienSha=$(cat "$1.sha256")
	fi

	nouveauSha=$(sha256sum "$1" | cut -d " " -f 1)
	
	if [[ "$ancienSha" == "$nouveauSha" ]]
	then
		echo "true"
	else
		echo "false"
	fi
}

mkdir -p ${DIR_BUFRS_MOSA_Z}
mkdir -p ${DIR_BUFRS_MOSA_LAME_EAU}
mkdir -p ${DIR_IMAGES_MOSA_Z}
mkdir -p ${DIR_IMAGES_MOSA_LAME_EAU}
mkdir -p ${DIR_BIN}

if make && ( [ "$(fichier_a_jour ${DIR_WWW}pour_bundle_client_mosa.js)" = "true" ] || npm run build_mosa --prefix ${DIR_WWW} ) && ( "$(fichier_a_jour ${DIR_WWW}pour_bundle_client_obs.js)" = "true" ] || npm run build_obs --prefix ${DIR_WWW} )
then
	sha256sum "${DIR_WWW}pour_bundle_client_mosa.js" | cut -d " " -f 1 > "${DIR_WWW}pour_bundle_client_mosa.js.sha256"
	sha256sum "${DIR_WWW}pour_bundle_client_obs.js" | cut -d " " -f 1 > "${DIR_WWW}pour_bundle_client_obs.js.sha256"
	demon &
	npm run serverstart --prefix ${DIR_WWW} &
fi
