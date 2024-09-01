#ifndef DECODEUR_BUFR_H
#define DECODEUR_BUFR_H

#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <cstring>
#include <map>
#include <list>
#include <fstream>
#include <zlib.h>
#include <bitset>
#include <chrono>



// taille maximale d'un fichier ou d'une concatenation BUFR
#define MAXLENGTH (unsigned long)20000000

#define TABLE_BUFR (unsigned char)0
#define TABLE_LOCALE (unsigned char)1

#define ZH (unsigned char)0
#define ZDR (unsigned char)15
#define RHOHV (unsigned char)16
#define PHIDP (unsigned char)17
#define SIGMA (unsigned char)10
#define LAME_EAU (unsigned char)(22)

#define ZBAS (unsigned char)0
#define HBAS (unsigned char)1

#define CUMUL (unsigned char)0
#define QUALITE (unsigned char)1

#define VALEUR_MANQUANTE_1_OCTET (unsigned short)255
#define VALEUR_MANQUANTE_2_OCTETS (unsigned short)65535

#define VALEUR_MANQUANTE_FLOAT (double)99999.99999

// pour pouvoir utiliser la comparaison avec la surcharge de == et !=
// on ecrira le descripteur comme D(f,x,y)
#define D(...) std::initializer_list<unsigned char>{__VA_ARGS__}



struct Descripteur
{
	unsigned char f;
	unsigned char x;
	unsigned char y;
	unsigned short nbBits;
	short facteur;
	long long offset;
};

struct LigneDescripteur
{
	unsigned char x;
	unsigned char y;
	short facteur;
	long long offset;
	unsigned short nbbits;
	std::string unite;
};



using PixmapValeursEntieres = std::vector<std::vector<unsigned short>>;
using PixmapValeursFlottantes = std::vector<std::vector<double>>;
using TableB = std::set<LigneDescripteur, std::less<>>;
using TableD = std::map<Descripteur, std::vector<Descripteur>>;



bool operator<(const LigneDescripteur & lhs, const LigneDescripteur & rhs);
bool operator<(const Descripteur & lhs, const LigneDescripteur & rhs);
bool operator<(const LigneDescripteur & lhs, const Descripteur & rhs);
bool operator<(const Descripteur & lhs, const Descripteur & rhs);

/*
 * @brief Compare un descripteur avec ses valeurs de f, x et y
 * Surcharge l'operateur ==
 *
 * @param descripteur Descripteur a comparer
 * @param fxy Liste des valeurs de f, x et y (ex: D(0,10,2))
 *
 * @return bool Le descripteur et les valeurs de f, x et y correspondent
 */
bool operator==(const Descripteur & descripteur, const std::initializer_list<unsigned char> fxy);
bool operator==(const std::initializer_list<unsigned char> fxy, const Descripteur & descripteur);

/*
 * @brief Compare un descripteur avec ses valeurs de f, x et y
 * Surcharge l'operateur !=
 *
 * @param descripteur Descripteur a comparer
 * @param fxy Liste des valeurs de f, x et y (ex: D(0,10,2))
 *
 * @return bool Le descripteur et les valeurs de f, x et y ne correspondent pas
 */
bool operator!=(const Descripteur & descripteur, const std::initializer_list<unsigned char> fxy);
bool operator!=(const std::initializer_list<unsigned char> fxy, const Descripteur & descripteur);



class DecodeurBUFR
{
	private: 
		TableB tableB;
		TableD tableD;
		// cle : version de table
		std::map<unsigned char, TableB> tablesBBUFR;
		// cle : version de table
		std::map<unsigned char, TableD> tablesDBUFR;
		// cle : numero de centre OMM et version de table
		std::map<std::pair<unsigned short, unsigned char>, TableB> tablesBLocales;
		// cle : numero de centre OMM et version de table
		std::map<std::pair<unsigned short, unsigned char>, TableD> tablesDLocales;
		std::vector<unsigned char> buffer;
		std::vector<Descripteur> descripteurs;
		std::map<Descripteur, long long> changementsOffset;
		short modifNbBits;
		short modifNbOctetsCcitt;
		short modifEchelle;
		std::string dossierTables;
		unsigned short iDescripteurRepetitionEnCours;
		unsigned short nbDescripteursATraiter;
	


		/*
		 * @brief Indique si le buffer a partir de la position indiquee correspond a un
		 * message BUFR
		 *
		 * @param position
		 *
		 * @return bool
		 */
		bool is_bufr(const unsigned long & position);
	


		/*
		 * @brief Decode une table B internationale ou locale
		 *
		 * @param type Table BUFR ou locale
		 * @param numero Numero de table
		 * @param centre Identifiant du centre OMM
		 */
		void lire_table_b(const unsigned char & type, const unsigned char & numero,
				const unsigned short & centre=0);
		
		/*
		 * @brief Decode une table D internationale ou locale
		 *
		 * @param type Table BUFR ou locale
		 * @param numero Numero de table
		 * @param centre Identifiant du centre OMM
		 */
		void lire_table_d(const unsigned char & type, const unsigned char & numero,
				const unsigned short & centre=0);

		/*
		 * @brief Charge un ensemble de tables necessaires au decodage d'un buffer
		 * Tables B et D BUFR et locales
		 * Ces tables sont stockees dans tableB et tableD
		 *
		 * @param numeroTableBUFR Numero de table BUFR
		 * @param numeroTableLocale Numero de table locale
		 * @param centre Identifiant du centre OMM
		 */
		void charger_tables(const unsigned char & numeroTableBUFR,
				const unsigned char & numeroTableLocale,
				const unsigned short & centre);


		
		/*
		 * @brief Decode une sequence de descripteurs de repetition (f=3)
		 * a partir de la table D
		 *
		 * @param descripteurSequence Descripteur a decoder
		 */
		void decoder_sequence(Descripteur descripteurSequence);


		
		/*
		 * @brief Lit et stocke l'ensemble des descripteurs d'un BUFR
		 *
		 * @param position Position ou se trouve le premier desripteur
		 * @param nbDescripteurs Nombre de descripteurs a lire
		 */
		void lire_descripteurs(unsigned long position, const unsigned long & nbDescripteurs);
		
		
		
		/*
		 * @brief Lit des donnees dans le BUFR entre le bit debut et le bit fin
		 * Les bits debut et fin seront tous les deux lus, ainsi que ceux
		 * qu'ils delimitent
		 * On travaille sur des fichiers en big-endian
		 * ainsi, 4096 est stocke en memoire sur : 00010000 00000000
		 * en little-endian, il l'aurait ete comme suivant : 00000000 00010000
		 *
		 * @param debut Indice du premier bit a lire
		 * @param fin Indice du dernier bit a lire
		 *
		 * @return uint64_t Valeur extraite du BUFR, telle quelle
		 */	
		uint64_t lire_donnee(unsigned long long debut, unsigned long long fin);
		
		
		
		/*
		 * @brief Lit la valeur correspondante a un descripteur dans la section 4
		 * Si ce n'est pas un descripteur 0, on se contente de renvoyer la taille
		 * du descripteur
		 *
		 * @param i Indice du descripteur dans le vector descripteurs
		 * dont il faut aller chercher la valeur
		 * @param position Position a laquelle se situe la valeur dans le BUFR
		 * @param valeur Pointeur vers la valeur qu'il faudra remplir
		 *
		 * @return unsigned short Taille de la donnee correspondante au descripteur
		 * dans la section 4, en bits
		 */		
		template <typename TypeRetour>
		unsigned long lire_descripteur_section_4(unsigned long & i,
							unsigned long long position,
							TypeRetour* valeur=nullptr);
	


	public:
		/*
		 * Constructeur
		 *
		 * @param _dossierTables Dossier vers les tables BUFR
		 * Il est conseille de mettre un chemin absolu
		 */
		DecodeurBUFR(const std::string & _dossierTables);

		
		/*
		 * @brief Charge un fichier BUFR (ou une concatenation) dans le decodeur
		 *
		 * @nomFichier Nom du fichier a charger
		 * Il est conseille de mettre un chemin absolu
		 *
		 * @return char Reussite de l'operation
		 * 0 : operation reussie
		 * 1 : echec lors de l'ouverture du fichier
		 * -1 : echec lors de la lecture du fichier
		 */
		char ouvrir_fichier(const std::string & nomFichier);
	

		
		// La double definition des deux fonctions suivantes est certes violente
		// Cependant, utiliser un template et faire differentes actions dans une
		// seule fonction en fonction du type fourni est a s'arracher les cheveux
		// Deja essaye, a ne pas retenter

		/*
		 * @brief Lit une pixmap provenant d'une concatenation PAM ou PAG
		 * On renvoie une pixmap de codes
		 *
		 * @param numeroParametre Numero du sous-type de donnees
		 * ZH=0 | SIGMA=10 | VITESSE=5 | ZDR=15 | RHOHV=16 | PHIDP=17 | ADVECTION=18
		 *
		 * @return Pixmap correspondante au parametre demande
		 */
		PixmapValeursEntieres lire_pixmap_pam_pag_code(
							const unsigned char & numeroParametre,
							float* facteurHydram=nullptr);
	
		/*
		 * @brief Lit une pixmap provenant d'une concatenation PAM ou PAG
		 * On renvoie une pixmap de valeurs reelles decodees
		 * (sauf les valeurs manquantes, aller voir convertir_valeur_reelle())
		 *
		 * @param numeroParametre Numero du sous-type de donnees
		 * ZH=0 | SIGMA=10 | VITESSE=5 | ZDR=15 | RHOHV=16 | PHIDP=17 | ADVECTION=18
		 *
		 * @return Pixmap correspondante au parametre demande
		 */
		PixmapValeursFlottantes lire_pixmap_pam_pag_decode(
							const unsigned char & numeroParametre,
							float* facteurHydram=nullptr);
	


		/*
		 * @brief Lit une pixmap provenant d'un BUFR de ZBAS
		 *
		 * @param numeroParametre Parametre voulu
		 * ZBAS=0 | HBAS=1
		 * @param radarsPresents Tableau qui contiendra les numero OMM des radars
		 * inclus dans la mosaique
		 *
		 * @return PixmapValeursFlottantes Pixmap correspondante au parametre demande
		 */
		PixmapValeursFlottantes lire_pixmap_zhbas(const unsigned char & numeroParametre,
						std::vector<unsigned long>* radarsPresents=nullptr);



		/*
		 * @brief Lit une pixmap provenant d'un BUFR de lame d'eau
		 * On renvoie une pixmap de codes
		 *
		 * @param numeroParametre Parametre voulu
		 * CUMUL=0 | QUALITE=1
		 * @param radarsPresents Tableau qui contiendra les numero OMM des radars
		 * inclus dans la mosaique
		 *
		 * @return PixmapValeursEntieres Pixmap correspondante au parametre demande
		 */
		PixmapValeursEntieres lire_pixmap_lame_eau_code(const unsigned char & numeroParametre,
						std::vector<unsigned long>* radarsPresents=nullptr);



		/*
		 * @brief Convertit une valeur codee provenant d'un PAM ou PAG
		 * en valeur reelle
		 * Les marqueurs de valeur manquante (par exemple 255 pour un ZH)
		 * restent tels quels
		 *
		 * @param typeImage Type de l'image (ZH, SIGMA...)
		 * @param valeurInitiale Valeur codee
		 *
		 * @return double Valeur reelle
		 */
		static double convertir_valeur_reelle(const unsigned char & typeImage,
						const unsigned short & valeurInitiale);
};

#endif // DECODEUR_BUFR_H
