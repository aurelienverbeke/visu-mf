#include <iostream>
#include <string>
#include "decodeur_bufr.h"
#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "json.hpp"
#include <fstream>
#include <cstdio>
#include <sys/stat.h>

namespace fs = std::filesystem;
using json = nlohmann::json;



int main(int argc, char *argv[])
{
	DecodeurBUFR decodeur(std::getenv("DIR_TABLES_BUFR"));
	int res;
	PixmapValeursFlottantes pixmapZ;
	PixmapValeursEntieres pixmapLameEau;
	std::vector<unsigned long> radarsPresents;

	std::string nomFichier;
	std::string heure;

	std::fstream f;

	json echelles;
	json radarsPresentsZ;
	json radarsPresentsLameEau;
	
	cv::Mat matrice(1536, 1536, CV_8UC4);

	struct stat buffer;

	


	if(stat(std::getenv("CHEMIN_ECHELLES"), &buffer) == 0)
	{
		f.open(std::getenv("CHEMIN_ECHELLES"), std::fstream::in);
		if(!f.is_open())
		{
			std::cout << "Erreur a la lecture des echelles\n";
			exit(EXIT_FAILURE);
		}
		echelles = json::parse(f);
		f.close();
	}

	if(stat(std::getenv("CHEMIN_RADARS_PRESENTS_MOSA_Z"), &buffer) == 0)
	{
		f.open(std::getenv("CHEMIN_RADARS_PRESENTS_MOSA_Z"), std::fstream::in);
		if(!f.is_open())
		{
			std::cout << "Erreur a l'ouverture des radars presents pour la mosaique Z\n";
			exit(EXIT_FAILURE);
		}
		radarsPresentsZ = json::parse(f);
		f.close();
	}

	if(stat(std::getenv("CHEMIN_RADARS_PRESENTS_MOSA_Z"), &buffer) == 0)
	{
		f.open(std::getenv("CHEMIN_RADARS_PRESENTS_MOSA_LAME_EAU"), std::fstream::in);
		if(!f.is_open())
		{
			std::cout << "Erreur a l'ouverture des radars presents pour la mosaique de lame d'eau\n";
			exit(EXIT_FAILURE);
		}
		radarsPresentsLameEau = json::parse(f);
		f.close();
	}





	for(const auto & fichierBufrPresent : fs::directory_iterator(std::getenv("DIR_BUFRS_MOSA_Z")))
	{
		nomFichier = fichierBufrPresent.path().string();
		heure = fichierBufrPresent.path().filename().string().substr(0, 12);

		res = decodeur.ouvrir_fichier(nomFichier);
		if(res == 0)
		{
			radarsPresents.clear();
			pixmapZ = decodeur.lire_pixmap_zhbas(ZBAS, &radarsPresents);

			radarsPresentsZ[heure] = radarsPresents;
			f.open(std::getenv("CHEMIN_RADARS_PRESENTS_MOSA_Z"), std::fstream::out | std::fstream::trunc);
			if(!f.is_open())
			{
				std::cout << "Erreur a l'ouverture des radars presents pour la mosaique Z\n";
				exit(EXIT_FAILURE);
			}
			f << std::setw(1) << std::setfill('\t') << radarsPresentsZ;
			f.close();

			for(unsigned short i=0 ; i<1536 ; i++)
			{
				for(unsigned short j=0 ; j<1536 ; j++)
				{
					double pixel = pixmapZ[i][j];
					
					cv::Vec4b & pixelMatrice = matrice.at<cv::Vec4b>(i, j);
					
					if(pixel == VALEUR_MANQUANTE_FLOAT)
					{
						for(auto borne : echelles["z"])
						{
							if(borne["bornes"] == "manquant")
							{
								pixelMatrice = { borne["couleur"][2].get<unsigned char>(),
										borne["couleur"][1].get<unsigned char>(),
										borne["couleur"][0].get<unsigned char>(),
										borne["couleur"][3].get<unsigned char>() };
								break;
							}
						}
					}
					else
					{
						for(auto borne : echelles["z"])
						{
							if(borne["bornes"] != "manquant" && borne["bornes"][0].get<double>() < pixel && pixel <= borne["bornes"][1].get<double>())
							{
								pixelMatrice = { borne["couleur"][2].get<unsigned char>(),
										borne["couleur"][1].get<unsigned char>(),
										borne["couleur"][0].get<unsigned char>(),
										borne["couleur"][3].get<unsigned char>() };
								break;
							}
						}
					}
				}
			}

			cv::imwrite(std::getenv("DIR_IMAGES_MOSA_Z") + heure + ".png", matrice);

			system((std::string("rm ") + nomFichier).c_str());
		}
		else
		{
			std::cout << "C++ : Probleme a l'ouverture de " << nomFichier << "\n";
		}
	}



	for(const auto & fichierBufrPresent : fs::directory_iterator(std::getenv("DIR_BUFRS_MOSA_LAME_EAU")))
	{
		nomFichier = fichierBufrPresent.path().string();
		heure = fichierBufrPresent.path().filename().string().substr(0, 12);
		
		res = decodeur.ouvrir_fichier(nomFichier);
		if(res == 0)
		{
			radarsPresents.clear();
			pixmapLameEau = decodeur.lire_pixmap_lame_eau_code(CUMUL);

			radarsPresentsZ[heure] = radarsPresents;
			f.open(std::getenv("CHEMIN_RADARS_PRESENTS_MOSA_LAME_EAU"), std::fstream::out | std::fstream::trunc);
			if(!f.is_open())
			{
				std::cout << "Erreur a l'ouverture des radars presents pour la mosaique de lame d'eau\n";
				exit(EXIT_FAILURE);
			}
			f << std::setw(1) << std::setfill('\t') << radarsPresentsLameEau;
			f.close();

			for(unsigned short i=0 ; i<1536 ; i++)
			{
				for(unsigned short j=0 ; j<1536 ; j++)
				{
					cv::Vec4b & pixelMatrice = matrice.at<cv::Vec4b>(i, j);
					
					if(pixmapLameEau[i][j] == VALEUR_MANQUANTE_2_OCTETS)
					{
						for(auto borne : echelles["lame_eau"])
						{
							if(borne["bornes"] == "manquant")
							{
								pixelMatrice = { borne["couleur"][2].get<unsigned char>(),
										borne["couleur"][1].get<unsigned char>(),
										borne["couleur"][0].get<unsigned char>(),
										borne["couleur"][3].get<unsigned char>() };
								break;
							}
						}
					}
					else
					{
						double pixel = (double)pixmapLameEau[i][j] * 0.01;
						
						for(auto borne : echelles["lame_eau"])
						{
							if(borne["bornes"] != "manquant" && borne["bornes"][0].get<double>() < pixel && pixel <= borne["bornes"][1].get<double>())
							{
								pixelMatrice = { borne["couleur"][2].get<unsigned char>(),
										borne["couleur"][1].get<unsigned char>(),
										borne["couleur"][0].get<unsigned char>(),
										borne["couleur"][3].get<unsigned char>() };
								break;
							}
						}
					}
				}
			}

			cv::imwrite(std::getenv("DIR_IMAGES_MOSA_LAME_EAU") + heure + ".png", matrice);

			system((std::string("rm ") + nomFichier).c_str());
		}
		else
		{
			std::cout << "C++ : Probleme a l'ouverture de " << nomFichier << "\n";
		}
	}





	exit(0);
}
