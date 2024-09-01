import requests
import json
from datetime import datetime as dt
from datetime import timedelta as td
import os
import sys



def print_json(json_txt):
    print(json.dumps(json.loads(json_txt), indent=4))





# suppression des images datant de plus de la duree selectionnee
heureMin = (dt.utcnow() - td(hours=int(os.environ["DUREE_CONSERVEE"]))).strftime("%Y%m%d%H%M")

for fichier in os.listdir(os.environ["DIR_IMAGES_MOSA_Z"]):
    if(fichier[:-4] < heureMin):
        os.system(f"rm {os.environ['DIR_IMAGES_MOSA_Z']}{fichier}")

for fichier in os.listdir(os.environ["DIR_IMAGES_MOSA_LAME_EAU"]):
    if(fichier[:-4] < heureMin):
        os.system(f"rm {os.environ['DIR_IMAGES_MOSA_LAME_EAU']}{fichier}")





# recuperation du token qui servira aux requetes de recuperation des images
reponse = requests.post("https://portail-api.meteofrance.fr/token",\
                            data={"grant_type":"client_credentials"},\
                            headers={"Authorization":f"Basic {os.environ['APPLICATION_ID']}"})
token = reponse.json()["access_token"]





# recuperation du BUFR mosaique Z
reponse = requests.get("https://public-api.meteofrance.fr/public/DPRadar/v1/mosaiques/METROPOLE/observations/REFLECTIVITE",\
                                headers={"accept":"application/json", "Authorization":f"Bearer {token}"})
if "links" in reponse.json():
    heure = reponse.json()["links"][1]["validity_time"]
    heure = dt.strptime(heure, "%Y-%m-%dT%H:%M:%SZ").strftime("%Y%m%d%H%M")

    if not os.path.isfile(f"{os.environ['DIR_IMAGES_MOSA_Z']}{heure}.png"):
        reponse = requests.get("https://public-api.meteofrance.fr/public/DPRadar/v1/mosaiques/METROPOLE/observations/REFLECTIVITE/produit?maille=1000",\
                                        headers={"accept":"application/json", "Authorization":f"Bearer {token}"})

        nomFichier = os.environ["DIR_BUFRS_MOSA_Z"] + heure + ".bufr.gz"
        with open(nomFichier, "wb") as fp:
            fp.write(reponse.content)





# recuperation du BUFR mosaique lame d'eau
reponse = requests.get("https://public-api.meteofrance.fr/public/DPRadar/v1/mosaiques/METROPOLE/observations/LAME_D_EAU",\
                                headers={"accept":"application/json", "Authorization":f"Bearer {token}"})
if "links" in reponse.json():
    heure = reponse.json()["links"][2]["validity_time"]
    heure = dt.strptime(heure, "%Y-%m-%dT%H:%M:%SZ").strftime("%Y%m%d%H%M")

    if not os.path.isfile(f"{os.environ['DIR_IMAGES_MOSA_LAME_EAU']}{heure}.png"):
        reponse = requests.get("https://public-api.meteofrance.fr/public/DPRadar/v1/mosaiques/METROPOLE/observations/LAME_D_EAU/produit?maille=1000",\
                                        headers={"accept":"application/json", "Authorization":f"Bearer {token}"})

        nomFichier = os.environ["DIR_BUFRS_MOSA_LAME_EAU"] + heure + ".bufr.gz"
        with open(nomFichier, "wb") as fp:
            fp.write(reponse.content)





sys.exit(0)
