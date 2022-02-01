#ifndef COMPTE_H
#define COMPTE_H

/*!
 * \file compte.h
 * \brief Header, genere et gere tout les comptes
 * \author Boris Légal, Martin Boyer, Thomas Laurent, Vicor Kerjean
 * \version 0.1
 */

#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QTextStream>
#include <iostream>
#include <list>
#include <sstream>

using namespace std;

class Exception {  /// A MODIFIER A TERME POUR AVOIR UN SEUL TYPE D'EXEPTION
   public:
    Exception(const QString& message) : info(message) {}
    QString getInfo() const { return info; }

   private:
    QString info;
};

const QString NMois[] = {"janvier", "fevrier", "mars",      "avril",   "mai",      "juin",
                         "juillet", "aout",    "septembre", "octobre", "novembre", "decembre"};

class Date {
    int seconde;
    int minute;
    int heure;
    int jour;
    int mois;
    int annee;

   public:
    Date() {
        time_t actuel = time(0);
        tm* t = localtime(&actuel);
        seconde = t->tm_sec;
        minute = t->tm_min;
        heure = t->tm_hour;
        jour = t->tm_mday;
        mois = t->tm_mon;
        annee = 1900 + t->tm_year;
    }
    Date(int _seconde, int _minute, int _heure, int _jour, int _mois, int _annee);
    const int& getSeconde() const { return seconde; }
    const int& getMinute() const { return minute; }
    const int& getHeure() const { return heure; }
    const int& getJour() const { return jour; }
    const int& getMois() const { return mois; }
    const int& getAnnee() const { return annee; }
};

static Date dateInit(0, 0, 0, 1, 1, 1);

ostream& operator<<(ostream& f, const Date& d);

bool operator<=(const Date& d1, const Date& d2);

/*! \class Compte_component
 * \brief classe mere de tous les comptes, component du design pattern component.
 *
 *  La classe gere toutes les méthodes des classes filles(4 types de comptes + compte racine). ELle est abstraite, mais
 * sert particulièrement pour toutes les methodes récursives.
 */
class Compte_component  // Non instanciable : classe mère des 4 types de comptes + compte racine
{
   protected:
    QString c_nom; /*!< Nom du compte*/
    Compte_component* c_parent; /*!< Pointeur vers le compte parent utile pour le parcours dans l'abre.*/

    // attributs de gestion de d'arborescence
    std::list<Compte_component*> comptes_fils; /*!< Liste des comptes fils*/

    // attributs du compte
    float c_solde = 0; /*!< Solde initial*/

    // date du dernier rapprochement bancaire.
    Date& c_Date_Rapprochement = dateInit;
    bool c_estVirtuel;

   public:
    /*!
     *  \brief Constructeur
     *
     *  Constructeur de la classe Compte_component
     *
     *  \param nom : nom du compte
     *  \param parent : pointeur vers le parent, valeur par default vers nullptr
     *  \param Date_de_rapprochement : date du dernier rapprochement
     */
    Compte_component(QString nom, Compte_component* parent, bool estVirtuel = 1, Date& Date_de_rapprochement = dateInit);
    /*!
     *  \brief Destructeur
     *
     *  Destructeur de la classe CPlayer
     */
    virtual ~Compte_component() {}
    /*!
     *  \brief Constructeur par default
     *
     *  Constructeur de la classe Compte_component par default
     *
     */
    Compte_component() = default;

    /*!
     *  \brief Mutateur de parent
     *
     *  Methode qui permet de définir le parent d'un compte
     *
     *  \param parent : pointeur vers le nouveau parent
     */
    void setParent(Compte_component* parent);
    /*!
     *  \brief Accesseur de parent
     *
     *  Methode qui permet de recupérer le parent d'un compte
     *
     *  \return Un pointeur vers le parent du compte
     */
    Compte_component* getParent() const;
    /*!
     *  \brief Mutateur de nom
     *
     *  Methode qui permet de définir le nom d'un compte
     *
     *  \param nom : nouveau nom à definir
     */
    virtual void setNom(QString nom);
    /*!
     *  \brief Accesseur de nom
     *
     *  Methode qui permet de recupérer le nom d'un compte
     *
     *  \return Une chaine contenant le nom du compte.
     */
    virtual QString getNom();
    /*!
     *  \brief Accesseur de c_estVirtuel
     *
     *  Methode qui permet de savoir si le compte est virtuel ou non
     *
     *  \return Un booléen (0 => compte non virtuel | 1 => compte virtuel)
     */
    bool getVirtuel();

    void setDate_Rapprochement(Date& d1) {
        Date d2(d1.getSeconde(), d1.getMinute(), d1.getHeure(), d1.getJour(), d1.getMois(), d1.getAnnee());
        c_Date_Rapprochement = d2;
    }
    Date getDate_Rapprochement() { return c_Date_Rapprochement; }

    /*!
     *  \brief Ajoute un compte
     *
     *  Methode qui permet d'ajouter un compte dans la liste du compte appelant.
     *
     *  \param compteajout : pointeur vers le nouveau compte à ajouter
     */
    virtual void ajouter(Compte_component* compteajout);
    /*!
     *  \brief Supprime
     *
     *  Methode qui permet de supprimer un élément de la liste du compte appelant.
     *
     *  \param Compte_component : pointeur vers le compte à supprimer
     */
    virtual void supprimer(Compte_component* Compte_component);
    /*!
     *  \brief Cherche l'existance d'un compte dans l'arboresence du compte appelant
     *
     *  Methode qui permet de chercher l'existance d'un compte dans l'arboresence du compte appelant(dans toute sa
     *descendance). Appelé du compte racine, il cherche donc l'existance d'un compte par son nom dans tout l'arbre.
     *  \param nom : Nom du compte à rechercher.
     *	\return True si le compte est trouvé, false sinon.
     */
    virtual bool contient(QString nom);
    /*!
     *  \brief Creer un compte et le place dans l'arborescence.
     *
     *  Methode qui permet de creer un compte du type spécifié, et de le placer dans mes fils du compte père spécifié.
     *	Appelé avec comme père le compte racine, le type ne peut pas être déduit, il est donc nécessairement spécifié.
     *	Autrement, il est automatiquement déduit et n'a donc pas de raison d'être spécifié.
     *  \param nomCompteParent : Nom du compte parent dans lequel va être inséré le nouveau compte.
     *  \param nomNvCompte : Nom du nouveau compte.
     *  \param type : paramèetre uniquement utile lorsque le compte parent est racine.
     *	\return True si le compte est correctement créé(père trouvé etc), false sinon.
     */

    virtual bool creerCompte(QString nomCompteParent, QString nomNvCompte, bool estVirtuel = 1, QString type = "", Date& Date_de_rapprochement = dateInit);
    /*!
     *  \brief Outil de debugage-Affiche dans la console de debug tous les arbre et leur pere.
     *
     *  Parcours recursivement l'arbre en profondeur.
     *	\return True si on a pu parcourir tous les comptes correctement, false sinon.
     */
    virtual bool affichageQdebug();
    /*!
     *  \brief Fabrique une liste de tuple comptenant le nom du compte et celui de son père
     *
     *  Parcours recursivement l'arbre en profondeur et fourni une liste permettant d'afficher l'abre. On l'appellera de
     *racine pour avoir tous les comptes. \return retourne une liste avec son nom plus celui de son pere ainsi que tous
     *ceux de ses comptes fils.
     */
    virtual std::list<std::tuple<QString, QString>> affichageList();
    /*!
     *  \brief Vérifie si le compte appelant a des fils
     *
     *  Regarde simplement si sa liste est vide
     *	\return true si la liste n'est pas vide, false sinon
     */
    bool hasSon();
    /*!
     *  \brief Accesseur sur la liste des fils.
     *
     *
     *	\return La liste de pointeurs vers les comptes fils
     */
    std::list<Compte_component*> getFils();

    // fonctions de gestion du compte
    /*!
     *  \brief Renome un compte
     *
     *  Methode qui permet de renomer un compte présent dans l'arboresence du compte appelant.
     *
     *  \param ancienNom : nom du compte a renommer
     *  \param nvNom : nom qui remplacera l'ancien
     */
    virtual bool resetname(QString ancienNom, QString nvNom);
    float setSolde(float nvSolde);
    virtual float getSolde() const = 0;
    virtual float getSoldeLive() const = 0;
    virtual Compte_component& copie(float solde) = 0;
    void debiter(const float m);
    void crediter(const float m);
    /*!
     *  \brief Supprime un compte et toute sa descendance.
     *
     *  Methode qui permet de supprimer le compte appellant et toute sa descendance.
     * On l'utilisera en front end avec localiser(QString nom) qui permet de recuperer le compte a supprimer dans un
     * premier temps.
     *
     */
    virtual void effacer();
    /*!
     *  \brief Localise et donne le compte recherché
     *
     *  Methode qui parcourt l'arbre en profondeur.
     *
     *  \param nom : nom du compte à localiser/retourner
     *	\return Un pointeur vers le compte avec le nom spécifié
     */
    Compte_component* localiser(QString nom);
    /*!
     *  \brief Parcours les comptes de types passifs depuis la racine et les affiche
     *
     *  Méthode récursive qui parcours toute l'arborescence, affiche les comptes de type passifs
     *  La méthode va parcourir l'arborescence jusqu'au bout, même si un compte passif à été trouvé
     *
     *	\return True si au moins un compte de type passif est trouvé, false sinon.
     */
    bool parcoursPassifs();
};

/*! \class CompteRacine
 * \brief Classe ancetre de tous les comptes.
 *
 *  La classe gere toutes les comptes créés. Sa construction est inspirée d'un singleton pattern, bien que le
 * polymorphisme rende l'implémentation complète du singleton compliqué. On lancera toutes les methodes générales à
 * partir de l'objet unique compte racine.
 */
class CompteRacine : public Compte_component {
   protected:
    static CompteRacine* singleton; /*!< Pointeur vers un objet CompteRacine statique donc unique*/

    /*!
     *  \brief Constructeur
     *
     *  Constructeur de la classe CompteRacine. Hérite du construteur de Compte_component
     *
     *  \param nom : nom du compte
     *  \param parent : pointeur vers le parent, valeur par default vers nullptr
     */
    CompteRacine(QString nom, Compte_component* parent) : Compte_component(nom, parent) {}
    QString nomSauvegarde = ""; /*!< Chaine contenant le nom de la sauvegarde à charger*/

   public:
    /*!
     *  \brief Accesseur unique pour récupérer la racine
     *
     *  Inspiré d'un singleton pattern
     *
     *  \return Retourne le pointeur static vers le compte racine
     */
    static CompteRacine* getRacine();
    // Supprimer les fonctions pas implémentées et qui ont pas besoin de l'etre
    virtual float getSolde() const;  // somme des tous les comptes ?
    virtual float getSoldeLive() const;
    virtual Compte_component& copie(float solde = 0){};
    void bilan();
    void cloture();
    void create();
    /*!
     *  \brief Crée une arborescance de comptes à partir d'une sauvegarde
     *
     *  Methode qui permet de rétablir une trésorerie (arborescence de comptes et valeurs de leurs attributs) à partir
     * d'un fichier de sauvegarde en XML Si une arborescence de compte existe déjà, celle-ci sera supprimée
     *
     */
    void load();
    /*!
     *  \brief Amorce la sauvegarde de l'arborescence de comptes
     *
     *  Methode qui permet d'amorcer la sauvegarge de l'arborescence des comptes descendants du compte racine
     *  La méthode appelle saveUnder si le fichier de sauvegarde n'est pas désigné, sauvegarde les attributs du compte
     * racine puis appele saveRecursive sur ses fils
     *
     */
    void save();
    /*!
     *  \brief Sauvegarde récursive de l'arborescence de comptes
     *
     *  Methode qui permet de sauvegarder le compte passé en argument et qui s'appelle elle même sur les fils de
     * celui-ci La méthode utilise un dynamic_cast pour déterminer le type du compte à sauvegarder La sauvegarde le
     * compte dans un QDomDocument qui est passé en argument
     *
     *  \param doc : QDomDocument dans lequel le compte et ses attributs vont être ajoutés
     *  \param root : racine de l'arborescence de comptes du QDomDocument
     */
    void saveRecursive(Compte_component* compte, QDomDocument doc, QDomElement root);
    /*!
     *  \brief Permet la création graphique d'un fichier de sauvegarde XML
     */
    void saveUnder();
    ~CompteRacine() {
        // save(); // réfléchir à si on sauvegarde automatiquement à la destruction
        getRacine()->effacer();
    }
};

class ComptePassifs : public Compte_component {
   protected:
   public:
    /*!
     *  \brief Constructeur
     *
     *  Constructeur de la classe ComptePassifs. Hérite du construteur de Compte_component
     *
     *  \param nom : nom du compte
     *  \param parent : pointeur vers le parent, valeur par default vers nullptr
     */
    ComptePassifs(QString nom, Compte_component* parent, bool estVirtuel = 1, Date& Date_de_rapprochement = dateInit)
        : Compte_component(nom, parent, estVirtuel, Date_de_rapprochement) {
        qDebug() << "Création d'un ComptePassifs";
    }

    virtual float getSolde() const;
    virtual float getSoldeLive() const;
    virtual Compte_component& copie(float solde = 0);
};

class CompteActifs : public Compte_component {
   protected:
   public:
    /*!
     *  \brief Constructeur
     *
     *  Constructeur de la classe CompteActifs. Hérite du construteur de Compte_component
     *
     *  \param nom : nom du compte
     *  \param parent : pointeur vers le parent, valeur par default vers nullptr
     */
    CompteActifs(QString nom, Compte_component* parent, bool estVirtuel = 1, Date& Date_de_rapprochement = dateInit)
        : Compte_component(nom, parent, estVirtuel, Date_de_rapprochement) {
        qDebug() << "Création d'un CompteActifs";
    }
    virtual float getSolde() const;
    virtual float getSoldeLive() const;
    virtual Compte_component& copie(float solde = 0);
};

class CompteDepenses : public Compte_component {
   protected:
   public:
    /*!
     *  \brief Constructeur
     *
     *  Constructeur de la classe CompteDepenses. Hérite du construteur de Compte_component
     *
     *  \param nom : nom du compte
     *  \param parent : pointeur vers le parent, valeur par default vers nullptr
     */
    CompteDepenses(QString nom, Compte_component* parent, bool estVirtuel = 1, Date& Date_de_rapprochement = dateInit)
            : Compte_component(nom, parent, estVirtuel, Date_de_rapprochement) {
            qDebug() << "Création d'un CompteDepenses";
    }
    virtual float getSolde() const;
    virtual float getSoldeLive() const;
    virtual Compte_component& copie(float solde = 0);
};

class CompteRecettes : public Compte_component {
   protected:
   public:
   public:
    /*!
     *  \brief Constructeur
     *
     *  Constructeur de la classe CompteRecettes. Hérite du construteur de Compte_component
     *
     *  \param nom : nom du compte
     *  \param parent : pointeur vers le parent, valeur par default vers nullptr
     */
    CompteRecettes(QString nom, Compte_component* parent, bool estVirtuel = 1, Date& Date_de_rapprochement = dateInit)
            : Compte_component(nom, parent, estVirtuel, Date_de_rapprochement) {
            qDebug() << "Création d'un CompteRecettes";
    }
    virtual float getSolde() const;
    virtual float getSoldeLive() const;
    virtual Compte_component& copie(float solde = 0);
};

#endif  // COMPTE_H
