#include "decodeur_bufr.h"


bool operator<(const LigneDescripteur & lhs, const Descripteur & rhs)
{
	if(lhs.x > rhs.x)
		return false;
	else if(lhs.x == rhs.x)
		return (lhs.y < rhs.y);
	return true;
}
bool operator<(const Descripteur & lhs, const LigneDescripteur & rhs)
{
	if(lhs.x > rhs.x)
		return false;
	else if(lhs.x == rhs.x)
		return (lhs.y < rhs.y);
	return true;
}
bool operator<(const LigneDescripteur & lhs, const LigneDescripteur & rhs)
{
	if(lhs.x > rhs.x)
		return false;
	else if(lhs.x == rhs.x)
		return (lhs.y < rhs.y);
	return true;
}
bool operator<(const Descripteur & lhs, const Descripteur & rhs)
{
	if(lhs.x > rhs.x)
		return false;
	else if(lhs.x == rhs.x)
		return (lhs.y < rhs.y);
	return true;
}

bool operator==(const Descripteur & descripteur, const std::initializer_list<unsigned char> fxy)
{
	if(fxy.size() != 3)
		throw std::length_error("Il faut 3 nombres f, x et y");

	return (descripteur.f == *(fxy.begin())
		&& descripteur.x == *(fxy.begin()+1)
		&& descripteur.y == *(fxy.begin()+2));
}
bool operator==(const std::initializer_list<unsigned char> fxy, const Descripteur & descripteur)
{
	if(fxy.size() != 3)
		throw std::length_error("Il faut 3 nombres f, x et y");

	return (descripteur.f == *(fxy.begin())
		&& descripteur.x == *(fxy.begin()+1)
		&& descripteur.y == *(fxy.begin()+2));
}
bool operator!=(const Descripteur & descripteur, const std::initializer_list<unsigned char> fxy)
{
	if(fxy.size() != 3)
		throw std::length_error("Il faut 3 nombres f, x et y");

	return !(descripteur.f == *(fxy.begin())
		&& descripteur.x == *(fxy.begin()+1)
		&& descripteur.y == *(fxy.begin()+2));
}
bool operator!=(const std::initializer_list<unsigned char> fxy, const Descripteur & descripteur)
{
	if(fxy.size() != 3)
		throw std::length_error("Il faut 3 nombres f, x et y");

	return !(descripteur.f == *(fxy.begin())
		&& descripteur.x == *(fxy.begin()+1)
		&& descripteur.y == *(fxy.begin()+2));
}










DecodeurBUFR::DecodeurBUFR(const std::string & _dossierTables)
{
	dossierTables = _dossierTables;
}










bool DecodeurBUFR::is_bufr(const unsigned long & position)
{
	return(std::strncmp(reinterpret_cast<const char *>(buffer.data()+position), "BUFR", 4)
		== 0);
}










void DecodeurBUFR::lire_table_b(const unsigned char & type, const unsigned char & numero,
				const unsigned short & centre)
{
	char description[250];
	char unite[100];
	int f;
	std::string nomFichier = "";
	FILE* fp;
	LigneDescripteur ligneDescripteur;

	
	
	if(type==TABLE_BUFR)
		nomFichier = dossierTables + "/bufrtabb_" + std::to_string(numero) + ".csv";
	else
		nomFichier = dossierTables + "/localtabb_" + std::to_string(centre)
				+ "_" + std::to_string(numero) + ".csv";


	
	fp = fopen(nomFichier.c_str(), "r");
	
	if(fp != NULL)
	{
		while(fscanf(fp, "%d;%hhu;%hhu;%250[^;];%100[^;];%hd;%lld;%hu",
				&f, &ligneDescripteur.x, &ligneDescripteur.y, description,
				unite, &ligneDescripteur.facteur, &ligneDescripteur.offset,
				&ligneDescripteur.nbbits)
			== 8)
		{
			ligneDescripteur.unite = std::string(unite);
			tableB.insert(ligneDescripteur);
		}
		
		fclose(fp);
	}
	else
	{
		std::cout << "lire_table_b : Probleme a l'ouverture de "
			<< nomFichier << "\n";
		exit(EXIT_FAILURE);
	}
	
	
	
	if(type==TABLE_BUFR)
		tablesBBUFR[numero] = tableB;
	else
		tablesBLocales[std::pair<unsigned short, unsigned char>(centre, numero)] = tableB;
}










void DecodeurBUFR::lire_table_d(const unsigned char & type, const unsigned char & numero,
				const unsigned short & centre)
{
	Descripteur descripteurSequence, descripteur;
	std::vector<Descripteur> elements;
	std::string nomFichier = "";
	FILE* fp;
	int res = 0;



	if(type==TABLE_BUFR)
		nomFichier = dossierTables + "/bufrtabd_" + std::to_string(numero) + ".csv";
	else
		nomFichier = dossierTables + "/localtabd_" + std::to_string(centre)
				+ "_" + std::to_string(numero) + ".csv";



	fp = fopen(nomFichier.c_str(), "r");
	
	if(fp != NULL)
	{
		res = 0;

		while(res != EOF)
		{
			elements.clear();
			
			res = fscanf(fp, "%hhu;%hhu;%hhu;%hhu;%hhu;%hhu",
					&descripteurSequence.f, &descripteurSequence.x,
					&descripteurSequence.y, &descripteur.f,
					&descripteur.x, &descripteur.y);

			if(res != 6)
				res = fscanf(fp, "%*[^\n]\n");
			else
			{
				elements.push_back(descripteur);
				
				while(fscanf(fp, " ; ; ;%hhu;%hhu;%hhu",
						&descripteur.f,&descripteur.x,
						&descripteur.y)
					== 3)
				{
					descripteur.nbBits = 0;
					descripteur.facteur = 0;
					descripteur.offset = 0;
					elements.push_back(descripteur);
				}

				tableD.insert({descripteurSequence,elements});
			}

		}
		
		fclose(fp);
	}
	else
	{
		std::cout << "lire_table_d : Probleme a l'ouverture de "
			<< nomFichier << "\n";
		exit(EXIT_FAILURE);
	}

	
	
	if(type==TABLE_BUFR)
		tablesDBUFR[numero] = tableD;
	else
		tablesDLocales[std::pair<unsigned short, unsigned char>(centre, numero)] = tableD;
}










void DecodeurBUFR::charger_tables(const unsigned char & numeroTableBUFR,
				const unsigned char & numeroTableLocale,
				const unsigned short & centre)
{
	std::pair<unsigned short, unsigned char> couple(centre, numeroTableLocale);
	

	
	// on verifie qu'on a lu toutes les tables necessaires
	auto itBufrB = tablesBBUFR.find(numeroTableBUFR);
	if(itBufrB == tablesBBUFR.end())
		lire_table_b(TABLE_BUFR, numeroTableBUFR);
	
	auto itBufrD = tablesDBUFR.find(numeroTableBUFR);
	if(itBufrD == tablesDBUFR.end())
		lire_table_d(TABLE_BUFR, numeroTableBUFR);
	
	auto itLocalB = tablesBLocales.find(couple);
	if(itLocalB == tablesBLocales.end())
		lire_table_b(TABLE_LOCALE, numeroTableLocale, centre);
	
	auto itLocalD = tablesDLocales.find(couple);
	if(itLocalD == tablesDLocales.end())
		lire_table_d(TABLE_LOCALE, numeroTableLocale, centre);


	
	// on charge en tables courantes les versions demandees
	tableB = tablesBBUFR[numeroTableBUFR];
	tableB.insert(tablesBLocales[couple].begin(), tablesBLocales[couple].end());
	tableD = tablesDBUFR[numeroTableBUFR];
	tableD.insert(tablesDLocales[couple].begin(), tablesDLocales[couple].end());
}










void DecodeurBUFR::lire_descripteurs(unsigned long position, const unsigned long & nbDescripteurs)
{
	Descripteur descripteur;

	descripteurs.clear();
	modifNbBits = 0;
	modifNbOctetsCcitt = 0;
	modifEchelle = 0;
	nbDescripteursATraiter = 0;
	changementsOffset.clear();
	
	for(unsigned long i=0 ; i<nbDescripteurs ; i++)
	{
		descripteur.f = buffer[position] >> 6;
		descripteur.x = buffer[position] & 0x3F;
		descripteur.y = buffer[position+1];

		// descripteur de groupe, il faut lire son contenu dans la table D
		if(descripteur.f == 3)
		{
			if(nbDescripteursATraiter>0)
			{
				descripteurs[iDescripteurRepetitionEnCours].x
					+= (tableD.find(descripteur)->second).size() - 1;
			}
			decoder_sequence(descripteur);
		}

		// descripteur de changement de nombre de bits de codage dans la section 4
		// on stocke sa valeur
		else if(descripteur.f==2 && descripteur.x==1)
			modifNbBits = (descripteur.y==0) ? 0 : ((short)descripteur.y - 128);

		// descripteur de changement de nombre de caracteres dans la section 4
		// on stocke sa valeur
		else if(descripteur.f==2 && descripteur.x==8)
			modifNbOctetsCcitt = descripteur.y;

		// descripteur de changement d'echelle
		// on stocke sa valeur
		else if(descripteur.f==2 && descripteur.x==2)
			modifEchelle = (descripteur.y==0) ? 0 : ((short)descripteur.y - 128);

		// descripteur de repetition
		else if(descripteur.f == 1)
		{
			nbDescripteursATraiter = descripteur.x;
			iDescripteurRepetitionEnCours = descripteurs.size();
		}

		// descripteur 2 ou 1 qui n'occupe pas de place dans la section 4
		if(descripteur.f==2 || descripteur.f==1)
		{
			descripteur.nbBits = 0;
			descripteur.facteur = 0;
			descripteur.offset = 0;
			descripteurs.push_back(descripteur);
			if(nbDescripteursATraiter>0)
				nbDescripteursATraiter--;
		}
		
		else if(descripteur.f==0)
		{
			// fonctionne puisqu'on a redefini les operateurs de comparaison
			// entre Descripteur et LigneDescripteur
			// et que TableD prend en compte cette redefinition
			auto ligne = tableB.find(descripteur);
			if(ligne==tableB.end())
			{
				std::cout << "Descripteur " << (long)descripteur.f << ";"
					<< (long)descripteur.x << ";"
					<< (long)descripteur.y << " manquant dans la table B\n";
				exit(EXIT_FAILURE);
			}

			descripteur.facteur = ligne->facteur;
			descripteur.nbBits = ligne->nbbits;
			descripteur.offset = ligne->offset;
				
			// c'est un nombre
			// on met un find car on peut avoir "Code table"
			// ou "Common Code table C-1" par exemple
			// mais tous contiennent "Code table"
			if(ligne->unite.compare("CCITT IA5")!=0
				&& ligne->unite.find("Code table")==std::string::npos
				&& ligne->unite.compare("Flag table")!=0)
			{
				descripteur.facteur += modifEchelle;
				descripteur.nbBits += modifNbBits;
			}
			
			// c'est un champ chaine de caractere dont la longueur
			// a ete modifiee
			else if(ligne->unite.compare("CCITT IA5")==0 && modifNbOctetsCcitt!=0)
				descripteur.nbBits = modifNbOctetsCcitt*8;

			descripteurs.push_back(descripteur);

			if(nbDescripteursATraiter>0)
				nbDescripteursATraiter--;
		}		
		
		// chaque descripteur est code sur 2 octets
		position += 2;
	}
}










void DecodeurBUFR::decoder_sequence(Descripteur descripteurSequence)
{
	for(Descripteur descripteur : tableD.find(descripteurSequence)->second)
	{
		// descripteur de groupe, on recommence
		if(descripteur.f == 3)
		{
			if(nbDescripteursATraiter>0)
			{
				descripteurs[iDescripteurRepetitionEnCours].x
					+= (tableD.find(descripteur)->second).size() - 1;
			}
			decoder_sequence(descripteur);
		}

		// descripteur de changement de nombre de bits de codage dans la section 4
		// on stocke sa valeur
		else if(descripteur.f==2 && descripteur.x==1)
			modifNbBits = (descripteur.y==0) ? 0 : ((short)descripteur.y - 128);

		// descripteur de changement de nombre de caracteres dans la section 4
		// on stocke sa valeur
		else if(descripteur.f==2 && descripteur.x==8)
			modifNbOctetsCcitt = descripteur.y;

		// descripteur de changement d'echelle
		// on stocke sa valeur
		else if(descripteur.f==2 && descripteur.x==2)
			modifEchelle = (descripteur.y==0) ? 0 : ((short)descripteur.y - 128);

		
		
		// descripteur 2 ou 1 qui n'occupe pas de place dans la section 4
		if(descripteur.f==2 || descripteur.f==1)
		{
			descripteur.nbBits = 0;
			descripteur.facteur = 0;
			descripteur.offset = 0;
			descripteurs.push_back(descripteur);
		}
		
		else if(descripteur.f==0)
		{
			// fonctionne puisqu'on a redefini les operateurs de comparaison
			// entre Descripteur et LigneDescripteur
			// et que TableD prend en compte cette redefinition
			auto ligne = tableB.find(descripteur);
			if(ligne==tableB.end())
			{
				std::cout << "Descripteur " << (long)descripteur.f << ";"
					<< (long)descripteur.x << ";"
					<< (long)descripteur.y << " manquant dans la table B\n";
				exit(EXIT_FAILURE);
			}

			descripteur.facteur = ligne->facteur;
			descripteur.nbBits = ligne->nbbits;
			descripteur.offset = ligne->offset;
				
			// c'est un nombre
			// on met un find car on peut avoir "Code table"
			// ou "Common Code table C-1" par exemple
			// mais tous contiennent "Code table"
			if(ligne->unite.compare("CCITT IA5")!=0
				&& ligne->unite.find("Code table")==std::string::npos
				&& ligne->unite.compare("Flag table")!=0)
			{
				descripteur.facteur += modifEchelle;
				descripteur.nbBits += modifNbBits;
			}
			
			// c'est un champ chaine de caractere dont la longueur
			// a ete modifiee
			else if(ligne->unite.compare("CCITT IA5")==0 && modifNbOctetsCcitt!=0)
				descripteur.nbBits = modifNbOctetsCcitt*8;

			descripteurs.push_back(descripteur);
		}		
	}
}










uint64_t DecodeurBUFR::lire_donnee(unsigned long long debut, unsigned long long fin)
{
	uint64_t resultat = 0;

	unsigned long numeroOctetDebut = 0;
	unsigned long numeroOctetFin = 0;

	unsigned short nbBitsADecaler = 0;

	unsigned short nbBits = fin - debut + 1;
	
	if(nbBits > 64)
	{
		fin = debut + 63;
		nbBits = 64;
	}

	numeroOctetDebut = debut / 8;
	numeroOctetFin = fin / 8;

	

	// on place les bits du dernier octet a la fin
	// ce peut etre seulement quelques bits du dernier octet
	// si on n'est pas aligne au debut de l'octet
	nbBitsADecaler = (7 - (fin % 8));
	resultat |= (buffer[numeroOctetFin] >> nbBitsADecaler);



	// on ajoute les octets en partant de la fin
	nbBitsADecaler = 8 - nbBitsADecaler;
	
	for (unsigned long i = numeroOctetFin-1; i >= numeroOctetDebut; i--)
	{
		resultat |= ((uint64_t)buffer[i] << nbBitsADecaler);
		nbBitsADecaler += 8;
	}


	// on efface les bits non voulus a gauche du bit de debut
	// dans le premier octet
	//
	// on utilise un ET logique avec des 1 sur les octets voulus et des 0 sinon
	// ainsi si on a une valeur sur 3 bits
	// on fait un ET logique avec
	// (1 << 3) - 1
	// = 00001000 - 1
	// = 00000111
	if(nbBits<64)
		resultat &= ((1ULL << nbBits) - 1);



	return resultat;
}










template <typename TypeRetour>
unsigned long DecodeurBUFR::lire_descripteur_section_4(unsigned long & i,
						unsigned long long position,
						TypeRetour* valeur)
{
	Descripteur descRechercheChangementOffset = descripteurs[i];
	descRechercheChangementOffset.nbBits = 0;
	descRechercheChangementOffset.facteur = 0;
	
	if(descripteurs[i].f == 0)
	{
		long long offset = descripteurs[i].offset;
		
		if(valeur != nullptr)
		{
			uint64_t valeurLue = lire_donnee(position,
							position + descripteurs[i].nbBits - 1);
			
			// marqueur de valeur manquante dans le BUFR
			// on ne prend en charge que les flottants
			// on renvoie VALEUR_MANQUANTE_FLOAT
			if(valeurLue == ((1ULL<<descripteurs[i].nbBits)-1)
					&& (typeid(TypeRetour)==typeid(float)
						|| typeid(TypeRetour)==typeid(double)
						|| typeid(TypeRetour)==typeid(long double)))
				*valeur = (TypeRetour)VALEUR_MANQUANTE_FLOAT;
			
			else
			{
				// on verifie si un descripteur 2 03 n'a pas modifie l'offset
				std::map<Descripteur, long long>::iterator redefinitionTrouvee;
				redefinitionTrouvee = changementsOffset.find(descRechercheChangementOffset);
				
				// on a effectivement une redefinition d'offset
				if(redefinitionTrouvee != changementsOffset.end())
					offset = redefinitionTrouvee->second;

				if(descripteurs[i].facteur==0)
					*valeur = (TypeRetour)((int64_t)valeurLue + offset);
				else
					*valeur = (TypeRetour)((double)((int64_t)valeurLue + offset)
								/ (double)(pow(10, descripteurs[i].facteur)));
			}
		}

		return descripteurs[i].nbBits;
	}



	// descripteur 2 03 de redefinition d'offset
	if(descripteurs[i].f==2 && descripteurs[i].x==3)
	{
		unsigned char nbBits = descripteurs[i].y;
		uint64_t nouveauOffset = 0;
		unsigned short nombreRedefinitions = 0;

		Descripteur descChangementOffset;

		// on reinitialise les redefinitions d'offset
		if(nbBits==0)
		{
			changementsOffset.clear();
			return 0;
		}
		
		i++;
		// le descripteur 2 03 255 marque la fin des redefinitions d'offset
		while(descripteurs[i] != D(2,3,255))
		{
			descChangementOffset = descripteurs[i];
			descChangementOffset.nbBits = 0;
			descChangementOffset.facteur = 0;
			descChangementOffset.offset = 0;
			
			nouveauOffset = lire_donnee(position, position + nbBits - 1);
			
			// le nombre est negatif
			if((nouveauOffset & (1ULL << (nbBits-1))) != 0)
			{
				// on efface le 1 de negativite
				nouveauOffset &= ((1ULL << (nbBits-1)) - 1);

				changementsOffset[descChangementOffset] = -nouveauOffset;
			}
			
			else
			{
				changementsOffset[descChangementOffset] = nouveauOffset;
			}

			i++;
			position += nbBits;
			nombreRedefinitions++;
		}

		return nombreRedefinitions*nbBits;
	}

	return 0;
}










char DecodeurBUFR::ouvrir_fichier(const std::string & nomFichier)
{	
	char resultat = 0;
	gzFile contenuGz;
	int longueurLue;

	contenuGz = gzopen(nomFichier.c_str(),"rb");

	buffer.clear();
	buffer.resize(MAXLENGTH);

	if(contenuGz != NULL)
	{
		longueurLue = gzread(contenuGz, buffer.data(), MAXLENGTH-1);
		if(longueurLue == -1)
			resultat = -1;
	}
	else
		resultat = 1;
	
	gzclose(contenuGz);



	return resultat;
}










PixmapValeursEntieres DecodeurBUFR::lire_pixmap_pam_pag_code(const unsigned char & numeroParametre,
							float* facteurHydram)
{
	unsigned short nbAzimuts = 0;
	unsigned short nbPortes = 0;

	PixmapValeursEntieres data;

	unsigned long long position = 0;
	unsigned long positionDebutMessage = 0;
	unsigned long positionDebutSection1 = 0;
	unsigned long positionDebutSection3 = 0;

	unsigned long tailleMessage = 0;
	unsigned long tailleSection1 = 0;
	unsigned long tailleSection2 = 0;
	unsigned long tailleSection3 = 0;

	unsigned char sousCategorieDonnees = 0;

	unsigned char numeroTableBUFR = 0;
	unsigned char numeroTableLocale = 0;
	unsigned short numeroCentreOMM = 0;

	unsigned char section2Existe = 0;
	
	unsigned long indiceDescripteurPixmap = 0;

	unsigned long nombreRepetitions = 0;
	unsigned char nombreDescripteursARepeter = 0;

	unsigned long tailleGroupe = 0;

	unsigned long indiceMax = 0;
	



	modifNbBits = 0;
	modifEchelle = 0;
	
	
	


	// tout au long du decodage, on s'assure de decoder en big-endian
	
	// les PAG et PAM sont des concatenations de BUFR
	// on s'assure qu'on commence bien au debut d'un BUFR
	while(is_bufr(position))
	{
		/*
		 *
		 * Section 0
		 *
		 */
		
		// on saute la marque BUFR
		position += 4;

		// lecture de la taille du message
		tailleMessage = (unsigned long)(
					buffer[position] << 16 |
					buffer[position+1] << 8 |
					buffer[position+2]);
		position += 3;

		// on saute la version
		position += 1;

		
		
		/*
		 *
		 * Section 1
		 *
		 */

		positionDebutSection1 = position;
		
		// lecture de la taille de la section 1
		tailleSection1 = (unsigned long)(
				buffer[position] << 16 |
				buffer[position+1] << 8 |
				buffer[position+2]);

		// numero de centre OMM
		position += 5;
		numeroCentreOMM = buffer[position];

		// recuperation de l'existence ou non d'une section 2
		// existe = 1 sur premier bit
		// n'existe pas = 0 sur premier bit
		position += 2;
		section2Existe = buffer[position] >> 7;

		// on avance jusqu'a la sous-categorie de donnees
		position += 2;
		
		// lecture de la sous categorie
		sousCategorieDonnees = buffer[position];
		
		// la sous-categorie de donnees correspond au parametre attendu
		if(sousCategorieDonnees == numeroParametre)
		{
			// numero de table BUFR
			position += 1;
			numeroTableBUFR = buffer[position];
			
			// numero de table locale
			position += 1;
			numeroTableLocale = buffer[position];
			
			// on avance jusqu'a la section 3 ou la section 2 si elle existe
			position = positionDebutSection1 + tailleSection1;
			
			// il y a une section 2
			// on recupere sa taille et on la saute
			if(section2Existe == 1)
			{
				tailleSection2 = (unsigned long)(
						buffer[position] << 16 |
						buffer[position+1] << 8 |
						buffer[position+2]);
				position += tailleSection2;
			}
					


			/*
			 *
			 * Section 3
			 * Descripteurs
			 *
			 */
			
			positionDebutSection3 = position;

			// lecture de la taille de section 3
			tailleSection3 = (unsigned long)(
					buffer[position] << 16 |
					buffer[position+1] << 8 |
					buffer[position+2] );

			// on se place au début des descripteurs de la section 3
			position += 7;

			// on lit l'ensemble des descripteurs
			charger_tables(numeroTableBUFR, numeroTableLocale, numeroCentreOMM);
			lire_descripteurs(position, (tailleSection3-7)/2);

			// on avance jusqu'a la section 4
			position = positionDebutSection3 + tailleSection3;



			/*
			 *
			 * Section 4
			 * Donnees
			 *
			 */
			
			// on s'aligne au debut des donnees
			position += 4;
			
			// on compte dorenavant en bits
			// les donnees dans la section 4 ne sont pas toutes alignees sur un debut
			// d'octet et elles ne font pas necessairement 8/16/24/... bits de large
			position *= 8;

			// on lit les descripteurs
			for(unsigned long i=0 ; i<descripteurs.size() ; i++)
			{
				// on a un descripteur de repetition
				// c'est bon signe pour la pixmap
				if(descripteurs[i].f == 1)
				{
					nombreDescripteursARepeter = descripteurs[i].x;

					// position n'est pas avancee,
					// le descripteur de repetition n'occupe pas de place
					// dans la section 4
					
					// on va chercher le nombre de repetitions
					// dans le descripteur suivant
					// generalement 0 31 192
					i++;
					position += lire_descripteur_section_4<unsigned long>(
									i,
									position,
									&nombreRepetitions);
					
					// a ce stade, position est aligne au debut
					// du groupe
					
					// on calcule la taille en bits du groupe de descripteurs
					// cette taille correspond a une repetition
					// ce sera par exemple 8 pour un Zh
					// et 16 pour un PhiDP
					tailleGroupe = 0;
					indiceMax = i+nombreDescripteursARepeter;
					for(i = i+1 ; i <= indiceMax ; i++)
					{
						if(descripteurs[i].f==0)
						{
							indiceDescripteurPixmap = i;
							tailleGroupe += lire_descripteur_section_4<char>(i,
											position);
						}
					}
					i--;

					// c'est la pixmap
					if(nombreRepetitions == nbPortes*nbAzimuts)
					{
						for(unsigned short azimut=0 ;
							azimut < nbAzimuts ;
							azimut++)
						{
							std::vector<unsigned short>
								radiale(nbPortes, 0);
				
							for(unsigned short porte = 0 ;
									porte < nbPortes ;
									porte++)
							{
								position +=
									lire_descripteur_section_4<unsigned short>(
										indiceDescripteurPixmap,
										position,
										&(radiale.data()[porte]));
							}
						
							// on stocke la radiale
							data.push_back(radiale);
						}
					}
					else
						position += nombreRepetitions*tailleGroupe;
				}
				
				// descripteur 0 30 021
				// nombre de portes
				else if(descripteurs[i] == D(0,30,21))
				{
					position += lire_descripteur_section_4<unsigned short>(i,
										position,
										&nbPortes);
				}
				
				// descripteur 0 30 022
				// nombre d'azimuts
				else if(descripteurs[i] == D(0,30,22))
				{
					position += lire_descripteur_section_4<unsigned short>(i,
										position,
										&nbAzimuts);
				}
				
				// descripteur 0 49 239
				// facteur Hydram en dBZ
				else if(descripteurs[i] == D(0,49,239))
				{
					position += lire_descripteur_section_4<float>(i,
									position,
									facteurHydram);
				}
				
				// autre descripteur dont on a pas besoin
				// on ne lit que la taille
				else
				{
					position += lire_descripteur_section_4<char>(i, position);
				}
			}

			// on ne decode que le parametre demande
			// ca ne sert a rien de parcourir la concatenation en entier
			break;
		}

		// on passe au message suivant
		positionDebutMessage += tailleMessage;
		position = positionDebutMessage; // on recommence a compter en octets
	}
	
	return data;
}









PixmapValeursFlottantes DecodeurBUFR::lire_pixmap_pam_pag_decode(
							const unsigned char & numeroParametre,
							float* facteurHydram)
{
	unsigned short nbAzimuts = 0;
	unsigned short nbPortes = 0;

	PixmapValeursFlottantes data;

	unsigned long long position = 0;
	unsigned long positionDebutMessage = 0;
	unsigned long positionDebutSection1 = 0;
	unsigned long positionDebutSection3 = 0;

	unsigned long tailleMessage = 0;
	unsigned long tailleSection1 = 0;
	unsigned long tailleSection2 = 0;
	unsigned long tailleSection3 = 0;

	unsigned char sousCategorieDonnees = 0;

	unsigned char numeroTableBUFR = 0;
	unsigned char numeroTableLocale = 0;
	unsigned short numeroCentreOMM = 0;

	unsigned char section2Existe = 0;
	
	unsigned long indiceDescripteurPixmap = 0;

	unsigned long nombreRepetitions = 0;
	unsigned char nombreDescripteursARepeter = 0;

	unsigned long tailleGroupe = 0;

	unsigned long indiceMax = 0;

	unsigned short temp = 0;
	




	modifNbBits = 0;
	modifEchelle = 0;





	// tout au long du decodage, on s'assure de decoder en big-endian
	
	// les PAG et PAM sont des concatenations de BUFR
	// on s'assure qu'on commence bien au debut d'un BUFR
	while(is_bufr(position))
	{
		/*
		 *
		 * Section 0
		 *
		 */
		
		// on saute la marque BUFR
		position += 4;

		// lecture de la taille du message
		tailleMessage = (unsigned long)(
					buffer[position] << 16 |
					buffer[position+1] << 8 |
					buffer[position+2]);
		position += 3;

		// on saute la version
		position += 1;

		
		
		/*
		 *
		 * Section 1
		 *
		 */

		positionDebutSection1 = position;
		
		// lecture de la taille de la section 1
		tailleSection1 = (unsigned long)(
				buffer[position] << 16 |
				buffer[position+1] << 8 |
				buffer[position+2]);
		
		// numero de centre OMM
		position += 5;
		numeroCentreOMM = buffer[position];

		// recuperation de l'existence ou non d'une section 2
		// existe = 1 sur premier bit
		// n'existe pas = 0 sur premier bit
		position += 2;
		section2Existe = buffer[position] >> 7;

		// on avance jusqu'a la sous-categorie de donnees
		position += 2;
		
		// lecture de la sous categorie
		sousCategorieDonnees = buffer[position];
		
		// la sous-categorie de donnees correspond au parametre attendu
		if(sousCategorieDonnees == numeroParametre)
		{
			// numero de table BUFR
			position += 1;
			numeroTableBUFR = buffer[position];
			
			// numero de table locale
			position += 1;
			numeroTableLocale = buffer[position];
			
			// on avance jusqu'a la section 3 ou la section 2 si elle existe
			position = positionDebutSection1 + tailleSection1;
			
			// il y a une section 2
			// on recupere sa taille et on la saute
			if(section2Existe == 1)
			{
				tailleSection2 = (unsigned long)(
						buffer[position] << 16 |
						buffer[position+1] << 8 |
						buffer[position+2]);
				position += tailleSection2;
			}
					


			/*
			 *
			 * Section 3
			 * Descripteurs
			 *
			 */
			
			positionDebutSection3 = position;

			// lecture de la taille de section 3
			tailleSection3 = (unsigned long)(
					buffer[position] << 16 |
					buffer[position+1] << 8 |
					buffer[position+2] );

			// on se place au début des descripteurs de la section 3
			position += 7;

			// on lit l'ensemble des descripteurs
			charger_tables(numeroTableBUFR, numeroTableLocale, numeroCentreOMM);
			lire_descripteurs(position, (tailleSection3-7)/2);

			// on avance jusqu'a la section 4
			position = positionDebutSection3 + tailleSection3;



			/*
			 *
			 * Section 4
			 * Donnees
			 *
			 */
			
			// on s'aligne au debut des donnees
			position += 4;
			
			// on compte dorenavant en bits
			// les donnees dans la section 4 ne sont pas toutes alignees sur un debut
			// d'octet et elles ne font pas necessairement 8/16/24/... bits de large
			position *= 8;

			// on lit les descripteurs
			for(unsigned long i=0 ; i<descripteurs.size() ; i++)
			{
				// on a un descripteur de repetition
				// c'est bon signe pour la pixmap
				if(descripteurs[i].f == 1)
				{
					nombreDescripteursARepeter = descripteurs[i].x;

					// position n'est pas avancee,
					// le descripteur de repetition n'occupe pas de place
					// dans la section 4
					
					// on va chercher le nombre de repetitions
					// dans le descripteur suivant
					// generalement 0 31 192
					i++;
					position += lire_descripteur_section_4<unsigned long>(
									i,
									position,
									&nombreRepetitions);
					
					// a ce stade, position est aligne au debut
					// du groupe
					
					// on calcule la taille en bits du groupe de descripteurs
					// cette taille correspond a une repetition
					// ce sera par exemple 8 pour un Zh
					// et 16 pour un PhiDP
					tailleGroupe = 0;
					indiceMax = i+nombreDescripteursARepeter;
					for(i = i+1 ; i <= indiceMax ; i++)
					{
						if(descripteurs[i].f==0)
						{
							indiceDescripteurPixmap = i;
							tailleGroupe += lire_descripteur_section_4<char>(i,
											position);
						}
					}
					i--;

					// c'est la pixmap
					if(nombreRepetitions == nbPortes*nbAzimuts)
					{
						//std::chrono::high_resolution_clock::time_point debut = std::chrono::high_resolution_clock::now();
						for(unsigned short azimut=0 ;
							azimut < nbAzimuts ;
							azimut++)
						{
							std::vector<double> radiale(nbPortes, 0);
						
							for(unsigned short porte = 0 ;
									porte < nbPortes ;
									porte++)
							{
								//debut = std::chrono::high_resolution_clock::now();
								position +=
									lire_descripteur_section_4<unsigned short>(
										indiceDescripteurPixmap,
										position,
										&temp);
								//std::cout << "Lire " << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-debut).count() << "\n";
								
								//debut = std::chrono::high_resolution_clock::now();
								radiale[porte] =
									convertir_valeur_reelle(
										numeroParametre,
										temp);
								//std::cout << "Convertir " << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-debut).count() << "\n";

							}
						
							// on stocke la radiale
							//debut = std::chrono::high_resolution_clock::now();
							data.push_back(radiale);
						}
						//std::cout << "Pixmap " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-debut).count() << "\n";
					}
					else
						position += nombreRepetitions*tailleGroupe;
				}
				
				// descripteur 0 30 021
				// nombre de portes
				else if(descripteurs[i] == D(0,30,21))
				{
					position += lire_descripteur_section_4<unsigned short>(i,
										position,
										&nbPortes);
				}
				
				// descripteur 0 30 022
				// nombre d'azimuts
				else if(descripteurs[i] == D(0,30,22))
				{
					position += lire_descripteur_section_4<unsigned short>(i,
										position,
										&nbAzimuts);
				}
				
				// descripteur 0 49 239
				// facteur Hydram en dBZ
				else if(descripteurs[i] == D(0,49,239))
				{
					position += lire_descripteur_section_4<float>(i,
									position,
									facteurHydram);
				}
				
				// autre descripteur dont on a pas besoin
				// on ne lit que la taille
				else
				{
					position += lire_descripteur_section_4<char>(i, position);
				}
			}

			// on ne decode que le parametre demande
			// ca ne sert a rien de parcourir la concatenation en entier
			break;
		}

		// on passe au message suivant
		positionDebutMessage += tailleMessage;
		position = positionDebutMessage; // on recommence a compter en octets
	}
	
	return data;
}










PixmapValeursFlottantes DecodeurBUFR::lire_pixmap_zhbas(const unsigned char & numeroParametre,
							std::vector<unsigned long>* radarsPresents)
{
	unsigned short nbAzimuts = 0;
	unsigned short nbPortes = 0;

	PixmapValeursFlottantes data;

	unsigned long long position = 0;
	unsigned long positionDebutSection1 = 0;
	unsigned long positionDebutSection3 = 0;

	unsigned long tailleSection1 = 0;
	unsigned long tailleSection2 = 0;
	unsigned long tailleSection3 = 0;

	unsigned char section2Existe = 0;

	unsigned char numeroTableBUFR = 0;
	unsigned char numeroTableLocale = 0;
	unsigned short numeroCentreOMM = 0;
	
	unsigned long indiceDescripteurParametre = 0;

	unsigned long nombreRepetitions = 0;
	unsigned char nombreDescripteursARepeter = 0;

	unsigned long indiceMax = 0;

	unsigned char numeroBlockOmm = 0;
	unsigned short numeroStationOmm = 0;
	bool radarAAjouter = false;
	




	modifNbBits = 0;
	modifEchelle = 0;





	// tout au long du decodage, on s'assure de decoder en big-endian
	
	// on s'assure qu'on commence bien au debut d'un BUFR
	if(is_bufr(0))
	{
		/*
		 *
		 * Section 0
		 *
		 */
		
		// on saute la section 0
		position += 8;

		
		
		/*
		 *
		 * Section 1
		 *
		 */

		positionDebutSection1 = position;
		
		// lecture de la taille de la section 1
		tailleSection1 = (unsigned long)(
				buffer[position] << 16 |
				buffer[position+1] << 8 |
				buffer[position+2]);

		// numero de centre OMM
		position += 4;
		numeroCentreOMM = buffer[position] << 8 | buffer[position+1];

		// recuperation de l'existence ou non d'une section 2
		// existe = 1 sur premier bit
		// n'existe pas = 0 sur premier bit
		position += 5;
		section2Existe = buffer[position] >> 7;

		// numero de table BUFR
		position += 4;
		numeroTableBUFR = buffer[position];
		
		// numero de table locale
		position += 1;
		numeroTableLocale = buffer[position];

		// on avance jusqu'a la section 3 ou la section 2 si elle existe
		position = positionDebutSection1 + tailleSection1;
		
		// il y a une section 2
		// on recupere sa taille et on la saute
		if(section2Existe == 1)
		{
			tailleSection2 = (unsigned long)(
					buffer[position] << 16 |
					buffer[position+1] << 8 |
					buffer[position+2]);
			position += tailleSection2;
		}
				


		/*
		 *
		 * Section 3
		 * Descripteurs
		 *
		 */
		
		positionDebutSection3 = position;

		// lecture de la taille de section 3
		tailleSection3 = (unsigned long)(
				buffer[position] << 16 |
				buffer[position+1] << 8 |
				buffer[position+2] );

		// on se place au début des descripteurs de la section 3
		position += 7;

		// on lit l'ensemble des descripteurs
		charger_tables(numeroTableBUFR, numeroTableLocale, numeroCentreOMM);
		lire_descripteurs(position, (tailleSection3-7)/2);

		// on avance jusqu'a la section 4
		position = positionDebutSection3 + tailleSection3;



		/*
		 *
		 * Section 4
		 * Donnees
		 *
		 */
		
		// on s'aligne au debut des donnees
		position += 4;
		
		// on compte dorenavant en bits
		// les donnees dans la section 4 ne sont pas toutes alignees sur un debut
		// d'octet et elles ne font pas necessairement 8/16/24/... bits de large
		position *= 8;

		// on lit les descripteurs
		for(unsigned long i=0 ; i<descripteurs.size() ; i++)
		{
			// on a un descripteur de repetition
			// c'est bon signe pour la pixmap
			if(descripteurs[i].f == 1)
			{
				nombreDescripteursARepeter = descripteurs[i].x;

				// position n'est pas avancee,
				// le descripteur de repetition n'occupe pas de place
				// dans la section 4
				
				// on va chercher le nombre de repetitions
				// dans le descripteur suivant
				// generalement 0 31 192
				i++;
				position += lire_descripteur_section_4<unsigned long>(
								i,
								position,
								&nombreRepetitions);
				
				
				
				// si la repetition concerne une pixmap, il faut recuperer le
				// parametre concerne (ZBAS, HBAS, PP)
				indiceMax = i + nombreDescripteursARepeter;
				for(unsigned long j=i+1 ; j <= indiceMax ; j++)
				{
					if(descripteurs[j].f==0)
					{
						// on memorise le descripteur concerne par la
						// repetition
						// il y en a un seul pour les pixmap
						// il servira a extraire la pixmap demandee
						// (ZBAS ou HBAS)
						indiceDescripteurParametre = j;
					}
				}
				


				// c'est la pixmap
				// et on se trouve sur celle qui a ete demandee (ZBAS ou HBAS)
				if(nombreRepetitions == nbPortes*nbAzimuts
					&& (
						((numeroParametre==ZBAS)
						 	&& (descripteurs[indiceDescripteurParametre]
								==D(0,21,1)))
						|| ((numeroParametre==HBAS)
							&& (descripteurs[indiceDescripteurParametre]
								==D(0,10,2)))
					))
				{
					for(unsigned short azimut=0 ;
						azimut < nbAzimuts ;
						azimut++)
					{
						std::vector<double>
							radiale(nbPortes, 0);
				
						for(unsigned short porte = 0 ;
								porte < nbPortes ;
								porte++)
						{
							position += lire_descripteur_section_4<double>(
									indiceDescripteurParametre,
									position,
									&(radiale.data()[porte]));
						}
						
						// on stocke la radiale
						data.push_back(radiale);
					}
				}
				else
				{
					for(unsigned long repet=0 ; repet<nombreRepetitions ; repet++)
					{
						for(unsigned long j=i+1 ; j<=indiceMax ; j++)
						{
							// dans une repetition sur chaque des radars,
							// des bits de calage sont inseres a chaque fin
							if(descripteurs[j].f == 1)
							{
								unsigned long nbBitsCalage = 0;
								
								j++;
								position += lire_descripteur_section_4<unsigned long>(j,
											position,
											&nbBitsCalage);
								position += nbBitsCalage;
								j++;
							}

							else if(descripteurs[j] == D(0,1,1)
									&& radarsPresents!=nullptr)
							{
								position += lire_descripteur_section_4<unsigned char>(j,
											position,
											&numeroBlockOmm);
								radarAAjouter = true;
							}
							
							else if(descripteurs[j] == D(0,1,2)
									&& radarsPresents!=nullptr)
								position += lire_descripteur_section_4<unsigned short>(j,
											position,
											&numeroStationOmm);

							else	
								position += lire_descripteur_section_4<char>(j,
											position);

							if(radarAAjouter)
							{
								radarsPresents->push_back(
										numeroBlockOmm*1000
										+ numeroStationOmm);
								radarAAjouter = false;
							}
						}
					}
				}

				i = indiceMax;
			}
			
			// descripteur 0 30 021
			// nombre de portes
			else if(descripteurs[i] == D(0,30,21))
			{
				position += lire_descripteur_section_4<unsigned short>(i,
									position,
									&nbPortes);
			}
			
			// descripteur 0 30 022
			// nombre d'azimuts
			else if(descripteurs[i] == D(0,30,22))
			{
				position += lire_descripteur_section_4<unsigned short>(i,
									position,
									&nbAzimuts);
			}
			
			// autre descripteur dont on a pas besoin
			// on ne lit que la taille
			else
			{
				//std::cout << (short)descripteurs[i].f << " " << (short)descripteurs[i].x << " " << (short)descripteurs[i].y << " : ";
				//double temp;
				//position += lire_descripteur_section_4<double>(i, position, &temp);
				position += lire_descripteur_section_4<char>(i, position);
				//std::cout << temp << "\n";
			}
		}
	}
	
	return data;
}










PixmapValeursEntieres DecodeurBUFR::lire_pixmap_lame_eau_code(const unsigned char & numeroParametre,
						std::vector<unsigned long>* radarsPresents)
{
	unsigned short nbAzimuts = 0;
	unsigned short nbPortes = 0;

	PixmapValeursEntieres data;

	unsigned long long position = 0;
	unsigned long positionDebutSection1 = 0;
	unsigned long positionDebutSection3 = 0;

	unsigned long tailleSection1 = 0;
	unsigned long tailleSection2 = 0;
	unsigned long tailleSection3 = 0;

	unsigned char section2Existe = 0;

	unsigned char numeroTableBUFR = 0;
	unsigned char numeroTableLocale = 0;
	unsigned short numeroCentreOMM = 0;
	
	unsigned long indiceDescripteurParametre = 0;

	unsigned long nombreRepetitions = 0;
	unsigned char nombreDescripteursARepeter = 0;

	unsigned long indiceMax = 0;

	unsigned char numeroBlockOmm = 0;
	unsigned short numeroStationOmm = 0;
	bool radarAAjouter = false;
	




	modifNbBits = 0;
	modifEchelle = 0;





	// tout au long du decodage, on s'assure de decoder en big-endian
	
	// on s'assure qu'on commence bien au debut d'un BUFR
	if(is_bufr(0))
	{
		/*
		 *
		 * Section 0
		 *
		 */
		
		// on saute la section 0
		position += 8;

		
		
		/*
		 *
		 * Section 1
		 *
		 */

		positionDebutSection1 = position;
		
		// lecture de la taille de la section 1
		tailleSection1 = (unsigned long)(
				buffer[position] << 16 |
				buffer[position+1] << 8 |
				buffer[position+2]);

		// numero de centre OMM
		position += 5;
		numeroCentreOMM = buffer[position];

		// recuperation de l'existence ou non d'une section 2
		// existe = 1 sur premier bit
		// n'existe pas = 0 sur premier bit
		position += 2;
		section2Existe = buffer[position] >> 7;

		// numero de table BUFR
		position += 3;
		numeroTableBUFR = buffer[position];
		
		// numero de table locale
		position += 1;
		numeroTableLocale = buffer[position];

		// on avance jusqu'a la section 3 ou la section 2 si elle existe
		position = positionDebutSection1 + tailleSection1;
		
		// il y a une section 2
		// on recupere sa taille et on la saute
		if(section2Existe == 1)
		{
			tailleSection2 = (unsigned long)(
					buffer[position] << 16 |
					buffer[position+1] << 8 |
					buffer[position+2]);
			position += tailleSection2;
		}
				


		/*
		 *
		 * Section 3
		 * Descripteurs
		 *
		 */
		
		positionDebutSection3 = position;

		// lecture de la taille de section 3
		tailleSection3 = (unsigned long)(
				buffer[position] << 16 |
				buffer[position+1] << 8 |
				buffer[position+2] );

		// on se place au début des descripteurs de la section 3
		position += 7;

		// on lit l'ensemble des descripteurs
		charger_tables(numeroTableBUFR, numeroTableLocale, numeroCentreOMM);
		lire_descripteurs(position, (tailleSection3-7)/2);

		//for(auto d : descripteurs)
		//	std::cout << (short)d.f << " " << (short)d.x << " " << (short)d.y << "\n";

		// on avance jusqu'a la section 4
		position = positionDebutSection3 + tailleSection3;



		/*
		 *
		 * Section 4
		 * Donnees
		 *
		 */
		
		// on s'aligne au debut des donnees
		position += 4;
		
		// on compte dorenavant en bits
		// les donnees dans la section 4 ne sont pas toutes alignees sur un debut
		// d'octet et elles ne font pas necessairement 8/16/24/... bits de large
		position *= 8;

		// on lit les descripteurs
		for(unsigned long i=0 ; i<descripteurs.size() ; i++)
		{
			// on a un descripteur de repetition
			// c'est bon signe pour la pixmap
			if(descripteurs[i].f == 1)
			{
				nombreDescripteursARepeter = descripteurs[i].x;

				// position n'est pas avancee,
				// le descripteur de repetition n'occupe pas de place
				// dans la section 4
				
				// on va chercher le nombre de repetitions
				// dans le descripteur suivant
				// generalement 0 31 192
				i++;
				position += lire_descripteur_section_4<unsigned long>(
								i,
								position,
								&nombreRepetitions);
				
				
				
				// si la repetition concerne une pixmap, il faut recuperer le
				// parametre concerne (CUMUL, QUALITE)
				indiceMax = i + nombreDescripteursARepeter;
				for(unsigned long j=i+1 ; j <= indiceMax ; j++)
				{
					if(descripteurs[j].f==0)
					{
						// on memorise le descripteur concerne par la
						// repetition
						// il y en a un seul pour les pixmap
						// il servira a extraire la pixmap demandee
						// (ZBAS ou HBAS)
						indiceDescripteurParametre = j;
					}
				}
				


				// c'est la pixmap
				// et on se trouve sur celle qui a ete demandee (CUMUL ou QUALITE)
				if(nombreRepetitions == nbPortes*nbAzimuts
					&& (
						((numeroParametre==CUMUL)
						 	&& (descripteurs[indiceDescripteurParametre]
								==D(0,30,4)))
						|| ((numeroParametre==QUALITE)
							&& (descripteurs[indiceDescripteurParametre]
								==D(0,49,204)))
					))
				{
					for(unsigned short azimut=0 ;
						azimut < nbAzimuts ;
						azimut++)
					{
						std::vector<unsigned short>
							radiale(nbPortes, 0);
				
						for(unsigned short porte = 0 ;
								porte < nbPortes ;
								porte++)
						{
							position += lire_descripteur_section_4<unsigned short>(
									indiceDescripteurParametre,
									position,
									&(radiale.data()[porte]));
						}
						
						// on stocke la radiale
						data.push_back(radiale);
					}
				}
				else
				{
					for(unsigned long repet=0 ; repet<nombreRepetitions ; repet++)
					{
						for(unsigned long j=i+1 ; j<=indiceMax ; j++)
						{
							// dans une repetition sur chaque des radars,
							// des bits de calage sont inseres a chaque fin
							if(descripteurs[j].f == 1)
							{
								unsigned long nbBitsCalage = 0;
								
								j++;
								position += lire_descripteur_section_4<unsigned long>(j,
											position,
											&nbBitsCalage);
								position += nbBitsCalage;
								j++;
							}
							
							else if(descripteurs[j] == D(0,1,1)
									&& radarsPresents!=nullptr)
							{
								position += lire_descripteur_section_4<unsigned char>(j,
											position,
											&numeroBlockOmm);
								radarAAjouter = true;
							}
							
							else if(descripteurs[j] == D(0,1,2)
									&& radarsPresents!=nullptr)
								position += lire_descripteur_section_4<unsigned short>(j,
											position,
											&numeroStationOmm);

							else	
								position += lire_descripteur_section_4<char>(j,
											position);

							if(radarAAjouter)
							{
								radarsPresents->push_back(
										numeroBlockOmm*1000
										+ numeroStationOmm);
								radarAAjouter = false;
							}
						}
					}
				}

				i = indiceMax;
			}
			
			// descripteur 0 30 021
			// nombre de portes
			else if(descripteurs[i] == D(0,30,21))
			{
				position += lire_descripteur_section_4<unsigned short>(i,
									position,
									&nbPortes);
			}
			
			// descripteur 0 30 022
			// nombre d'azimuts
			else if(descripteurs[i] == D(0,30,22))
			{
				position += lire_descripteur_section_4<unsigned short>(i,
									position,
									&nbAzimuts);
			}
			
			// autre descripteur dont on a pas besoin
			// on ne lit que la taille
			else
			{
				position += lire_descripteur_section_4<char>(i, position);
			}
		}
	}
	
	return data;
}










double DecodeurBUFR::convertir_valeur_reelle(const unsigned char & typeImage,
					const unsigned short & valeurInitiale)
{
	switch(typeImage)
	{
		case ZH:
			return (valeurInitiale != VALEUR_MANQUANTE_1_OCTET)
				? (double)valeurInitiale - 10.5
				: VALEUR_MANQUANTE_FLOAT;

		case ZDR:
			return (valeurInitiale != VALEUR_MANQUANTE_1_OCTET)
				? (double)valeurInitiale * 0.1 - 9.95
				: VALEUR_MANQUANTE_FLOAT;

		case RHOHV:
			return (valeurInitiale != VALEUR_MANQUANTE_1_OCTET)
				? (double)valeurInitiale * 0.01 + 0.305
				: VALEUR_MANQUANTE_FLOAT;

		case PHIDP:
			return (valeurInitiale != VALEUR_MANQUANTE_2_OCTETS)
				? (double)valeurInitiale + 0.5
				: VALEUR_MANQUANTE_FLOAT;

		case SIGMA:
			return (valeurInitiale != VALEUR_MANQUANTE_1_OCTET)
				? (double)valeurInitiale * 0.25 + 0.125
				: VALEUR_MANQUANTE_FLOAT;

		case LAME_EAU:
			return (valeurInitiale != VALEUR_MANQUANTE_2_OCTETS)
				? (double)valeurInitiale * 0.01
				: VALEUR_MANQUANTE_FLOAT;

		default:
			return (double)valeurInitiale;
	}
}
