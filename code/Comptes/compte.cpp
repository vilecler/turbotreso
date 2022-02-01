#include "compte.h"

CompteRacine* CompteRacine::singleton = nullptr;

///  DATE
///  ==========================================================================================================================================================
Date::Date(int _seconde, int _minute, int _heure, int _jour, int _mois, int _annee) {
    if (_seconde > 59) {
        throw Exception("Valeur des secondes invalide");
    } else {
        seconde = _seconde;
    }
    if (_minute > 59) {
        throw Exception("Valeur des minutes invalide");
    } else {
        minute = _minute;
    }
    if (_heure > 23 || _heure < 0) {
        throw Exception("Valeur des minutes invalide");
    } else {
        heure = _heure;
    }
    if (_jour > 31 || _jour < 1) {
        throw Exception("Valeur du jour invalide");
    } else {
        jour = _jour;
    }
    if (_mois > 12 || _mois < 1) {
        throw Exception("Valeur du mois invalide");
    } else {
        mois = _mois;
    }
    annee = _annee;
}

ostream& operator<<(std::ostream& f, const Date& d) {
    f << d.getJour() << "/";
    // const QString Mois =NMois[d.getMois()-1];
    f << d.getMois() - 1;
    f << "/" << d.getAnnee();
    f << " a " << d.getHeure() << "h " << d.getMinute() << "min " << d.getSeconde() << "sec ";
    return f;
}

/*
bool operator<=(const Date& d1, const Date& d2) {
return (d1.getSeconde()+60*d1.getMinute()+3600*d1.getHeure()
       +86400*d1.getJour()+2592000*d1.getMois()+31104000*d1.getAnnee()<=
       d2.getSeconde()+60*d2.getMinute()+3600*d2.getHeure()
                  +86400*d2.getJour()+2592000*d2.getMois()+31104000*d2.getAnnee());
}
*/

bool operator<=(const Date& d1, const Date& d2) {
    if (d1.getAnnee() < d2.getAnnee()) return true;
    if (d1.getAnnee() > d2.getAnnee()) return false;
    if (d1.getMois() < d2.getMois()) return true;
    if (d1.getMois() > d2.getMois()) return false;
    if (d1.getJour() < d2.getJour()) return true;
    if (d1.getJour() > d2.getJour()) return false;
    if (d1.getHeure() < d2.getHeure()) return true;
    if (d1.getHeure() > d2.getHeure()) return false;
    if (d1.getMinute() < d2.getMinute()) return true;
    if (d1.getMinute() > d2.getMinute()) return false;
    if (d1.getSeconde() < d2.getSeconde()) return true;
    if (d1.getSeconde() > d2.getSeconde()) return false;
    return true;
}

/// METHODES COMPTESCOMPOSITE=================================================
Compte_component::Compte_component(QString nom, Compte_component* parent, bool estVirtuel, Date& Date_de_rapprochement) {
    c_nom = nom;
    c_parent = parent;
    /*if (parent!=nullptr)
    parent->ajouter(this);*/ //utilisé pour les test dans main
    c_Date_Rapprochement = Date_de_rapprochement;
    c_estVirtuel = estVirtuel;
}
void Compte_component::setParent(Compte_component* parent) { this->c_parent = parent; }

Compte_component* Compte_component::getParent() const { return this->c_parent; }

void Compte_component::setNom(QString nom) { c_nom = nom; }
QString Compte_component::getNom() { return c_nom; }
bool Compte_component::getVirtuel() { return c_estVirtuel; }

void Compte_component::ajouter(Compte_component* compteajout) {
    if (!c_estVirtuel) {
            throw Exception("Impossible, le compte n'est pas virtuel");
    }
    comptes_fils.push_back(compteajout);
    qDebug() << "Compte au nom de " << compteajout->getNom() << "ajouté à " << this->getNom();
    compteajout->setParent(this);
}

void Compte_component::supprimer(Compte_component* Compte_suppr) {
    comptes_fils.remove(Compte_suppr);
    Compte_suppr->setParent(nullptr);
    delete Compte_suppr;
}

bool Compte_component::contient(QString nom) {  // a tester pas sur que ca marche
    if (c_nom == nom) {
        qDebug() << "Compte trouvé";
        return true;
    }

    else if (comptes_fils.empty()) {
        return false;
    }
    for (std::list<Compte_component*>::const_iterator ite = comptes_fils.cbegin(); ite != comptes_fils.end(); ite++) {
        if ((*ite)->contient(nom)) {
            return true;
        }
    }
    return false;
}

Compte_component* Compte_component::localiser(QString nom) {
    /**
     * Retourne un pointeur sur le compte possédant le nom fournit en argument si celui-ci existe (NULL sinon)
     */
    if (c_nom == nom) {
        return this;
    }

    else if (comptes_fils.empty()) {
        return nullptr;
    }
    for (std::list<Compte_component*>::const_iterator it = comptes_fils.cbegin(); it != comptes_fils.end(); it++) {
        if ((*it)->localiser(nom) != nullptr) {
            return (*it)->localiser(nom);
        }
    }
    return nullptr;
}

bool Compte_component::affichageQdebug() {
    if (this->getParent() != nullptr)
        qDebug() << "Compte : " << this->getNom() << ", fils de : " << this->getParent()->getNom();
    else
        qDebug() << "Compte : " << this->getNom() << ", fils de personne ";

    if (comptes_fils.empty()) {
        return false;
    }
    for (std::list<Compte_component*>::const_iterator ite = comptes_fils.cbegin(); ite != comptes_fils.end(); ite++) {
        if ((*ite)->affichageQdebug()) {
            return true;
        }
    }
    return false;
}

std::list<std::tuple<QString, QString>> Compte_component::affichageList() {
    QString nom_courant = this->getNom();
    QString nom_pere = "";
    if (this->getParent() != nullptr) nom_pere = this->getParent()->getNom();

    auto actualtuple = std::make_tuple(nom_courant, nom_pere);

    std::list<std::tuple<QString, QString>> listcourante;

    for (std::list<Compte_component*>::iterator ite = comptes_fils.begin(); ite != comptes_fils.end(); ite++) {
        listcourante.merge((*ite)->affichageList());
    }
    if (nom_pere != "") listcourante.push_front(actualtuple);  // on ne push pas la racine
    return listcourante;
}

bool Compte_component::resetname(QString ancienNom, QString nvNom) {
    if (c_nom == ancienNom) {
        setNom(nvNom);
        qDebug() << "Compte trouvé et nom changé";
        return true;
    } else if (comptes_fils.empty()) {
        return false;
    }
    for (std::list<Compte_component*>::iterator ite = comptes_fils.begin(); ite != comptes_fils.end(); ite++) {
        if ((*ite)->resetname(ancienNom, nvNom)) {
            return true;
        }
    }
    return false;
}

float Compte_component::setSolde(float nvSolde) { return c_solde = nvSolde; }

bool Compte_component::hasSon() {
    if (comptes_fils.size() == 0) {
        return false;
    }
    return true;
}

void Compte_component::debiter(const float m) {
    if (m < 0) throw Exception("montant a debiter negatif");
    c_solde -= m;
}

void Compte_component::crediter(const float m) {
    if (m < 0) throw Exception("montant a crediter negatif");
    c_solde += m;
}

bool Compte_component::creerCompte(QString nomCompteParent, QString nomNvCompte, bool estVirtuel, QString type, Date& Date_de_rapprochement) {
    // Remarque : ici, le type ne peut pas être déduit, on le précise donc en argument
    if (contient(nomNvCompte)) {
        throw Exception("Ce compte Existe deja!");
        return false;
    }

    else if (c_nom == nomCompteParent) {  // le compte est fils de CompteRacine
        if (!c_estVirtuel) {
            throw Exception("Impossible, le compte parent n'est pas virtuel");
        }
        if (dynamic_cast<ComptePassifs*>(this)) {
            ajouter(new ComptePassifs(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
            return true;
        } else if (dynamic_cast<CompteActifs*>(this)) {
            ajouter(new CompteActifs(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
            return true;
        } else if (dynamic_cast<CompteDepenses*>(this)) {
            ajouter(new CompteDepenses(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
            return true;
        } else if (dynamic_cast<CompteRecettes*>(this)) {
            ajouter(new CompteRecettes(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
            return true;
        } else {  // le parent est le singleton compte racine
            if (type == "passifs") {
                ajouter(new ComptePassifs(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
                return true;
            } else if (type == "actifs") {
                ajouter(new CompteActifs(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
                return true;
            } else if (type == "depenses") {
                ajouter(new CompteDepenses(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
                return true;
            } else if (type == "recettes") {
                ajouter(new CompteRecettes(nomNvCompte, this, estVirtuel, Date_de_rapprochement));
                return true;
            } else {
                throw Exception("type inconnu");
            }
        }
        return true;
    }

    else if (comptes_fils.empty()) {  // fin de la récurrence
        return false;
    }

    else {  // récurrence : on appelle creerCompte chez chacun des fils
        for (std::list<Compte_component*>::const_iterator ite = comptes_fils.cbegin(); ite != comptes_fils.end();
             ite++) {
            if ((*ite)->creerCompte(nomCompteParent, nomNvCompte, estVirtuel, type, Date_de_rapprochement)) {
                return true;
            }
        }
    }
}

void Compte_component::effacer() {
    while (!comptes_fils.empty()) {
        comptes_fils.front()->effacer();
    }
    if (c_parent != nullptr) {
        qDebug() << "on supprime ";
        this->affichageQdebug();
        c_parent->supprimer(this);
    }
}

std::list<Compte_component*> Compte_component::getFils() { return comptes_fils; }

bool Compte_component::parcoursPassifs() {
    bool res = false;
    if (dynamic_cast<ComptePassifs*>(this)) {
        qDebug() << "Compte passif existant : " << this->getNom();
        res = true;
    }
    for (std::list<Compte_component*>::const_iterator ite = comptes_fils.cbegin(); ite != comptes_fils.end(); ite++) {
        if ((*ite)->parcoursPassifs()) {
            res = true;
        }
    }
    return res;
}

/// METHODES COMPTEPASSIFS================================================================

float ComptePassifs::getSolde() const {
    if (c_solde == 0) {
        return c_solde;
    } else {
        return -c_solde;
    }
}

float ComptePassifs::getSoldeLive() const {}  // à implémenter -> à suppr
Compte_component& ComptePassifs::copie(float NouveauSolde) {
    Compte_component* copie = new ComptePassifs(c_nom, c_parent);
    copie->setSolde(NouveauSolde);
    return *copie;
}

/// METHODES COMPTEACTIFS================================================================

float CompteActifs::getSolde() const { return c_solde; }

float CompteActifs::getSoldeLive() const {}  // à implémenter -> à suppr

Compte_component& CompteActifs::copie(float NouveauSolde) {
    Compte_component* copie = new CompteActifs(c_nom, c_parent);
    copie->setSolde(NouveauSolde);
    return *copie;
}

/// METHODES COMPTEDEPENSES================================================================

float CompteDepenses::getSolde() const {
    if (c_solde == 0) {
        return c_solde;
    } else {
        return -c_solde;
    }
}
Compte_component& CompteDepenses::copie(float NouveauSolde) {
    Compte_component* copie = new CompteDepenses(c_nom, c_parent);
    copie->setSolde(NouveauSolde);
    return *copie;
}
float CompteDepenses::getSoldeLive() const {}  // à implémenter -> à suppr

/// METHODES COMPTERECETTES================================================================

float CompteRecettes::getSolde() const { return c_solde; }

float CompteRecettes::getSoldeLive() const {}  // à implémenter -> à suppr

Compte_component& CompteRecettes::copie(float NouveauSolde) {
    Compte_component* copie = new CompteRecettes(c_nom, c_parent);
    copie->setSolde(NouveauSolde);
    return *copie;
}

/// METHODES COMPTERACINE================================================================

CompteRacine* CompteRacine::getRacine() {
    if (singleton == nullptr) {
        singleton = new CompteRacine("racine", nullptr);
        qDebug() << "Creation nv compte racine";
    }
    return singleton;
}

float CompteRacine::getSolde() const {}  // à implémenter -> à suppr
float CompteRacine::getSoldeLive() const {}  // à implémenter -> à suppr

void CompteRacine::create() {
    /**
     * create est une méthode de CompteRacine, il faut donc instancier le singleton avant d'y faire appel
     * create est appelée pour créer une nouvelle trésorierie, à partir de rien (sans ouvrir de sauvegarde)
     */

    // si besoin, on sauvegarde l'état actuel
    if (hasSon()) {  // le compte racine a des fils => sauvegarde de l'arborescence avant d'en ouvrir une autre
        // save(); // sauvegarde automatique
        /* j'ai (temporairement) désactivé la sauvegarde automtique
        il faudra voir si c'est vraiment une bonne chose : je pense qu'un avertissement "Voulez vous quitter sans
        enregistrer ?" serait préférable */

        effacer();  // suppression de l'arborescence
        nomSauvegarde = "";
    }

    // on peut maintenant créer une nouvelle arborescence
}

void CompteRacine::load() {
    /**
     * load est une méthode de CompteRacine, il faut donc instancier le singleton avant d'y faire appel
     * load permet d'ouvrir graphiquement un fichier de sauvegarde
     * si une arborescence existe dans le programme, on la supprime pour la remplacer par celle lue dans le
     * fichier de sauvegarde
     */

    // si besoin, on sauvegarde l'état actuel
    if (hasSon()) {  // le compte racine a des fils => sauvegarde de l'arborescence avant d'en ouvrir une autre
        // save(); // sauvegarde automatique
        /* j'ai (temporairement) désactivé la sauvegarde automtique
        il faudra voir si c'est vraiment une bonne chose : je pense qu'un avertissement "Voulez vous quitter sans
        enregistrer ?" serait préférable */

        effacer();  // suppression de l'arborescence
    }

    // on peut maintenant restaurer une sauvegarde

    QString filename = QFileDialog::getOpenFileName(nullptr, "Ouverture de la sauvegarde", "", "XML files (*.xml)");

    // ouverture du fichier à partir du nom de fichier
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Exception("Exception : Erreur dans la lecture du fichier de sauvegarde !");
        return;
    }

    // création du document et parsing du fichier xml
    QDomDocument xmlDoc;
    xmlDoc.setContent(&file);

    /* Ouverture automatique du fichier de sauvegarde : deprecated
    qDebug() << "Lecture de la sauvegarde ...";
    // ouverture du fichier en lecture
    QFile* file = new QFile("./comptes.xml");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Exception("Exception : Erreur dans la lecture du fichier de sauvegarde !");
        return;
    }
    // création du QDomDocument
    QDomDocument xmlDoc;
    xmlDoc.setContent(file); */

    nomSauvegarde = filename;  // sauvegarde du nom du fichier afin de pouvoir sauvegarder automatiquement

    // recherche de tags "Comptes" dans le QDomDocument
    QDomNodeList comptes = xmlDoc.elementsByTagName("Compte");

    // restauration du compte racine
    QDomElement cptnode = comptes.item(0).toElement();
    c_nom = cptnode.attribute("c_nom", "");

    // parcours des Comptes, lecture des attributs
    for (int i = 1; i < comptes.size(); i++) {
        QDomElement cptnode = comptes.item(i).toElement();  // QDomElement correspondant au ième compte
        Date* date = new Date(cptnode.attribute("c_Date_Rapprochement_seconde", "").toUInt(),
                              cptnode.attribute("c_Date_Rapprochement_minute", "").toUInt(),
                              cptnode.attribute("c_Date_Rapprochement_heure", "").toUInt(),
                              cptnode.attribute("c_Date_Rapprochement_jour", "").toUInt(),
                              cptnode.attribute("c_Date_Rapprochement_mois", "").toUInt(),
                              cptnode.attribute("c_Date_Rapprochement_annee", "").toUInt());
        if (creerCompte(cptnode.attribute("c_parent", ""), cptnode.attribute("c_nom", ""), cptnode.attribute("c_estVirtuel", "").toUInt(),
                        cptnode.attribute("c_type", ""), *date)) {
            localiser(cptnode.attribute("c_nom", ""))->setSolde(cptnode.attribute("c_solde", "").toUInt());
        } else {
            throw Exception("Exception : Erreur dans la création des comptes !");
        }
    }
    // fermeture du fichier de sauvegarde
    // file->close(); // deprecated
    file.close();
}

void CompteRacine::save() {
    /**
     * save est une méthode de CompteRacine, il faut donc instancier le singleton avant d'y faire appel
     * save sauvegarde les comptes en format XML (mode attributes) dans le fichier de sauvegarde ouvert
     */

    // vérification qu'un fichier de sauvegarde existe afin de pouvoir l'écraser
    while (nomSauvegarde == "") {  // aucun fichier de sauvegarde n'a été spécifié
        saveUnder();
        return;
    }

    qDebug() << "Sauvegarde ...";
    QDomDocument doc;  // création de l'élément document
    QDomElement root = doc.createElement("Comptes");  // tag principal
    doc.appendChild(root);
    QDomElement cptElement = doc.createElement("Compte");
    cptElement.setAttribute("c_nom", getNom());
    cptElement.setAttribute("c_parent", "null");
    cptElement.setAttribute("c_solde", "null");
    cptElement.setAttribute("c_type", "null");
    cptElement.setAttribute("c_estVirtuel", "0");
    cptElement.setAttribute("c_Date_Rapprochement_seconde", "null");
    cptElement.setAttribute("c_Date_Rapprochement_minute", "null");
    cptElement.setAttribute("c_Date_Rapprochement_heure", "null");
    cptElement.setAttribute("c_Date_Rapprochement_jour", "null");
    cptElement.setAttribute("c_Date_Rapprochement_mois", "null");
    cptElement.setAttribute("c_Date_Rapprochement_annee", "null");

    root.appendChild(cptElement);

    // parcours de l'arborescence avec sauvegarde des comptes
    for (std::list<Compte_component*>::const_iterator it = comptes_fils.cbegin(); it != comptes_fils.end(); it++) {
        CompteRacine::saveRecursive(*it, doc, root);
    }

    // ouverture du fichier en lecture
    QFile* outfile = new QFile(nomSauvegarde);
    if (!outfile->open(QIODevice::ReadWrite)) {
        throw Exception("Exception : Erreur dans la lecture du fichier de sauvegarde !");
        return;
    }

    QTextStream stream(outfile);  // création d'un flux pour gérer la sortie
    stream << doc.toString();  // export du document
    outfile->close();  // fermeture du fichier
}

void CompteRacine::saveRecursive(Compte_component* compte, QDomDocument doc, QDomElement root) {
    /**
     * saveRecursive est une méthode de CompteRacine, il faut donc instancier le singleton avant d'y faire appel
     * saveRecursive est appelée depuis save qui permet de parcourir et sauvegarder toute l'arborescence
     */
    // sauvegarde des comptes de type P/A/D/R
    QDomElement cptElement = doc.createElement("Compte");
    cptElement.setAttribute("c_nom", compte->getNom());
    cptElement.setAttribute("c_parent", compte->getParent()->getNom());
    cptElement.setAttribute("c_solde", compte->getSolde());
    cptElement.setAttribute("c_solde", compte->getSolde());
    cptElement.setAttribute("c_estVirtuel", compte->getVirtuel());
    cptElement.setAttribute("c_Date_Rapprochement_seconde", compte->getDate_Rapprochement().getSeconde());
    cptElement.setAttribute("c_Date_Rapprochement_minute", compte->getDate_Rapprochement().getMinute());
    cptElement.setAttribute("c_Date_Rapprochement_heure", compte->getDate_Rapprochement().getHeure());
    cptElement.setAttribute("c_Date_Rapprochement_jour", compte->getDate_Rapprochement().getJour());
    cptElement.setAttribute("c_Date_Rapprochement_mois", compte->getDate_Rapprochement().getMois());
    cptElement.setAttribute("c_Date_Rapprochement_annee", compte->getDate_Rapprochement().getAnnee());

    // std::stringstream c_Date_Rapprochement;
    // c_Date_Rapprochement << compte->getDate_Rapprochement();
    // cptElement.setAttribute("c_Date_Rapprochement", QString::fromStdString(c_Date_Rapprochement.str()));


    // détermination du type du compte sauvegardé
    if (dynamic_cast<ComptePassifs*>(compte)) {
        cptElement.setAttribute("c_type", "passifs");
    } else if (dynamic_cast<CompteActifs*>(compte)) {
        cptElement.setAttribute("c_type", "actifs");
    } else if (dynamic_cast<CompteDepenses*>(compte)) {
        cptElement.setAttribute("c_type", "depenses");
    } else if (dynamic_cast<CompteRecettes*>(compte)) {
        cptElement.setAttribute("c_type", "recettes");
    } else {
        throw Exception("Exception : Erreur dans la lecture du type du compte !");
    }
    root.appendChild(cptElement);

    // on itère sur les descendants du compte
    if (compte->hasSon()) {
        std::list<Compte_component*> listeFils = compte->getFils();
        for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
            CompteRacine::saveRecursive(*it, doc, root);
        }
    }
}

void CompteRacine::saveUnder() {
    /**
     * saveUnder est une méthode de CompteRacine, il faut donc instancier le singleton avant d'y faire appel
     * saveUnder permet de créer graphiquement un nouveau fichier de sauvegarde
     */
    nomSauvegarde = QFileDialog::getSaveFileName(nullptr, "Création de la sauvegarde", "", "XML files (*.xml)");
    save();
}
