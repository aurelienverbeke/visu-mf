export PORT="3000"

export DIR_BASE="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )/"
export DIR_SRCS="${DIR_BASE}src/"
export DIR_BIN="${DIR_BASE}bin/"
export DIR_WWW="${DIR_BASE}www/"
export DIR_TABLES_BUFR="${DIR_BASE}lib/tables_bufr/"
export DIR_BUFRS_MOSA_Z="${DIR_BASE}bufrs/mosaiques/z/"
export DIR_BUFRS_MOSA_LAME_EAU="${DIR_BASE}bufrs/mosaiques/lame_eau/"
export DIR_IMAGES_MOSA_Z="${DIR_WWW}public/donnees/mosaiques/z/"
export DIR_IMAGES_MOSA_LAME_EAU="${DIR_WWW}public/donnees/mosaiques/lame_eau/"
export DIR_DONNEES_OBS_6_MIN="${DIR_WWW}public/donnees/obs/6_min/"
export CHEMIN_ECHELLES="${DIR_BASE}lib/echelles.json"
export CHEMIN_RADARS_PRESENTS_MOSA_Z="${DIR_WWW}public/donnees/mosaiques/z/radars_presents.json"
export CHEMIN_RADARS_PRESENTS_MOSA_LAME_EAU="${DIR_WWW}public/donnees/mosaiques/lame_eau/radars_presents.json"
export CHEMIN_METADONNEES_RADAR="${DIR_BASE}lib/metadonnees_radar.csv"

# heures
export DUREE_CONSERVEE=5

export APPLICATION_ID="YWNzZFp4amRTRjNtWnIxY1p1X0FpWlhhZ0w4YTpKeXY5TEt5RWZVRkZyV1BvTmUyV1Q4aTMyTU1h"
