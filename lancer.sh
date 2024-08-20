#!/bin/bash



export DIR_BASE="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )/"
export DIR_SRCS="${DIR_BASE}src/"
export DIR_BIN="${DIR_BASE}bin/"
export DIR_TABLES_BUFR="${DIR_BASE}lib/tables_bufr/"
export DIR_BUFRS_MOSA_Z="${DIR_BASE}bufrs/mosaiques/z/"
export DIR_BUFRS_MOSA_LAME_EAU="${DIR_BASE}bufrs/mosaiques/lame_eau/"
export DIR_IMAGES_MOSA_Z="${DIR_BASE}www/public/images/mosaiques/z/"
export DIR_IMAGES_MOSA_LAME_EAU="${DIR_BASE}www/public/images/mosaiques/lame_eau/"
export DIR_WWW="${DIR_BASE}www/"
export CHEMIN_ECHELLES="${DIR_BASE}lib/echelles.json"

export APPLICATION_ID="YWNzZFp4amRTRjNtWnIxY1p1X0FpWlhhZ0w4YTpKeXY5TEt5RWZVRkZyV1BvTmUyV1Q4aTMyTU1h"



function enchainement {
	python3 ${DIR_SRCS}/gestion_mf.py
	${DIR_BIN}creer_images
}

function demon {
	while true
	do
		enchainement &
		sleep 60s
	done
}

mkdir -p ${DIR_BUFRS_MOSA_Z}
mkdir -p ${DIR_BUFRS_MOSA_LAME_EAU}
mkdir -p ${DIR_IMAGES_MOSA_Z}
mkdir -p ${DIR_IMAGES_MOSA_LAME_EAU}
mkdir -p ${DIR_BIN}

if make && npm run build --prefix ${DIR_WWW}
then
	demon &
	npm run serverstart --prefix ${DIR_WWW}
fi
