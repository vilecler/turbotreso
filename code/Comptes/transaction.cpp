#include "transaction.h"

#include <algorithm>

using namespace std;

void Triplet::setName(QString nouv){
    T_compte=nouv;
}

Triplet::Triplet(QString compte, const float debit, const float credit) : T_compte(compte) {
    if (CompteRacine::getRacine()->localiser(compte)->getVirtuel()) {
        throw Exception("Impossible d'intervenir dans un triplet, le compte est virtuel");
    }
    float cr = 0;
    float db = 0;
    if (credit >= 0)
        cr += credit;
    else
        db -= credit;
    if (debit >= 0)
        db += debit;
    else
        cr -= debit;

    if (cr != 0 && db != 0) {
        T_credit = cr - min(cr, db);
        T_debit = db - min(cr, db);
    } else {
        T_credit = cr;
        T_debit = db;
    }
};

Triplet::Triplet(Compte_component* compte, const float debit, const float credit) : T_compte(compte->getNom()) {
    if (compte->getVirtuel()) {
        throw Exception("Impossible d'intervenir dans un triplet, le compte est virtuel");
    }
    float cr = 0;
    float db = 0;
    if (credit >= 0)
        cr += credit;
    else
        db -= credit;
    if (debit >= 0)
        db += debit;
    else
        cr -= debit;

    if (cr != 0 && db != 0) {
        T_credit = cr - min(cr, db);
        T_debit = db - min(cr, db);
    } else {
        T_credit = cr;
        T_debit = db;
    }
};

ostream& operator<<(ostream& stream, const QString& qstr) {
    stream << qstr.toStdString();
    return stream;
}

void Transaction::detruire(){
    for(list<Triplet*>::iterator it=triplets.begin();it!=triplets.end();it++){
        (*(*it)).~Triplet();
    }
}

void TransactionManager::detruire(){
    for(list<Transaction>::iterator it=transactions.begin();it!=transactions.end();it++){
        (*it).detruire();
        transactions.erase(it);
    }
}

void TransactionManager::save() {
    TransactionManager tm = getTransactionManager();
    while (nomSauvegarde == "") {  // aucun fichier de sauvegarde n'a été spécifié
        saveUnder();
        return;
    }
    qDebug() << "Sauvegarde ...";
    QDomDocument docTransac;  // création de l'élément document
    QDomElement rootTransac = docTransac.createElement("Transactions");  // tag principal
    docTransac.appendChild(rootTransac);
    std::list<Transaction>::iterator it = transactions.begin();
    while (it != transactions.end()) {
        Transaction trans = (*it);
        cout << "in Sauvegarde\n";
        TransactionManager::saveTransactions(trans, docTransac, rootTransac);
        it++;
    }

    QFile* outfile = new QFile(nomSauvegarde);
    if (!outfile->open(QIODevice::ReadWrite)) {
        throw Exception("Exception : Erreur dans la lecture du fichier de sauvegarde !");
        return;
    }

    QTextStream stream(outfile);  // création d'un flux pour gérer la sortie
    stream << docTransac.toString();  // export du document
    outfile->close();  // fermeture du fichier
}

void TransactionManager::saveTransactions(Transaction t, QDomDocument docTransac, QDomElement rootTransac) {
    cout << "in saveTransactions\n";
    QDomElement cptElement = docTransac.createElement("Transaction");
    cptElement.setAttribute("Titre", t.getTitre());
    cptElement.setAttribute("ID", t.getReference());
    rootTransac.appendChild(cptElement);
    int size = t.getTriplets().size();
    std::list<Triplet*>::iterator it = t.getTriplets().begin();
    cout << "size triplets : " << size << "\n";
    // Pour une raison que je n'explique pas ca beug quand je fait l'itération normale pour des list, j'ai donc mis en
    // place une boucle for se basant sur la taille de la liste

    for (int j = 0; j < size; j++) {
        cout << "before triplets\n";
        TransactionManager::saveTriplets(*(*it), docTransac, rootTransac);
        cout << "after triplets\n";
        if (it != t.getTriplets().end()) {
            cout << "bien rentre dans la boucle\n";
            it++;
        }
    }
}

void TransactionManager::saveTriplets(Triplet trip, QDomDocument docTransac, QDomElement rootTransac) {
    cout << "-----entrerrrrrr saveTriplets\n";
    QDomElement cptTriplets = docTransac.createElement("Triplets");
    CompteRacine* rac = CompteRacine::getRacine();
    cptTriplets.setAttribute("Compte", rac->localiser(trip.getCompte())->getNom());
    cptTriplets.setAttribute("Credit", trip.getCredit());
    cptTriplets.setAttribute("Debit", trip.getDebit());
    if (dynamic_cast<ComptePassifs*>(rac->localiser(trip.getCompte()))) {
        cptTriplets.setAttribute("Type", "Passif");
    } else if (dynamic_cast<CompteActifs*>(rac->localiser(trip.getCompte()))) {
        cptTriplets.setAttribute("Type", "Actif");
    } else if (dynamic_cast<CompteDepenses*>(rac->localiser(trip.getCompte()))) {
        cptTriplets.setAttribute("Type", "Depenses");
    } else if (dynamic_cast<CompteRecettes*>(rac->localiser(trip.getCompte()))) {
        cptTriplets.setAttribute("Type", "Recettes");
    }
    rootTransac.appendChild(cptTriplets);
}
void TransactionManager::saveUnder() {
    /**
     * saveUnder est une méthode de CompteRacine, il faut donc instancier le singleton avant d'y faire appel
     * saveUnder permet de créer graphiquement un nouveau fichier de sauvegarde
     */
    nomSauvegarde = QFileDialog::getSaveFileName(nullptr, "Création de la sauvegarde", "", "XML files (*.xml)");
    save();
}

void TransactionManager::load() {
    // h!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // -----------Reste à gérer la date, pouyr le moment on met une date arbitraire car pas de date en string, pareil
    // pour save----------

    QString filename = QFileDialog::getOpenFileName(nullptr, "Ouverture de la sauvegarde", "", "XML files (*.xml)");

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Exception("Exception : Erreur dans la lecture du fichier de sauvegarde !");
        return;
    }

    QDomDocument xmlDoc;
    xmlDoc.setContent(&file);
    nomSauvegarde = filename;
    Date date1 = Date();
    TransactionManager& tm = TransactionManager::getTransactionManager();
    QDomNodeList transactions = xmlDoc.elementsByTagName("Transaction");

    for (int i = 0; i < transactions.count(); i++) {
        QDomNode transacnode = transactions.at(i);
        if (transacnode.isElement()) {
            QDomElement transac = transacnode.toElement();
            QString titre = transac.attribute(("Titre"));
            QString id = transac.attribute("ID");
            QString trip = "Triplets";
            list<Triplet*> t = tm.getElements(transac, trip, id);
            tm.ajouterTransaction(date1, id, titre, t);
        }
    }

    file.close();
}

list<Triplet*> TransactionManager::getElements(QDomElement root, QString tag, QString att) {
    QDomNodeList nodes = root.elementsByTagName(tag);
    list<Triplet*> l1;
    for (int i = 0; i < nodes.count(); i++) {
        QDomNode elm = nodes.at(i);
        if (elm.isElement()) {
            QDomElement e = elm.toElement();
            qDebug() << e.attribute(att);
            float credit = e.attribute("Credit").toFloat();
            float debit = e.attribute("Debit").toFloat();
            QString nom = e.attribute("Compte");
            if (e.attribute("Type") == "Passif") {
                ComptePassifs cpt = ComptePassifs("nom", nullptr);
                Triplet t = Triplet(&cpt, debit, credit);
                l1.push_back(&t);
            } else if (e.attribute("Type") == "Actifs") {
                CompteActifs cpt = CompteActifs("nom", nullptr);
                Triplet t2 = Triplet(&cpt, debit, credit);
                l1.push_back(&t2);
            } else if (e.attribute("Type") == "Depenses") {
                CompteDepenses cpt = CompteDepenses("nom", nullptr);
                Triplet t3 = Triplet(&cpt, debit, credit);
                l1.push_back(&t3);
            } else {
                CompteRecettes cpt = CompteRecettes("nom", nullptr);
                Triplet t4 = Triplet(&cpt, debit, credit);
                l1.push_back(&t4);
            }
        }
    }
    return l1;
}

void TransactionManager::ajouterTransaction(Date& date, QString& id, QString& titre, list<Triplet*> &triplets) {
    int s = 0;
    list<Triplet*>::iterator itTriplet = triplets.begin();
    while (itTriplet != triplets.end()) {
        s += (*itTriplet)->getCredit();
        s -= (*itTriplet)->getDebit();
        itTriplet++;
    }
    if (s != 0) {
        throw Exception("Erreur, Transaction incorrect, verifiez les montants...");
    }
    Transaction* t = new Transaction(date, id, titre, triplets);
    transactions.push_back(*t);
}

void TransactionManager::afficherTransactions(ostream& f) {
    list<Transaction>::iterator it = transactions.begin();
    while (it != transactions.end()) {
        f << *it;
        it++;
    }
}

list<Compte_component*> Transaction::getEmetteurs() {
    list<Compte_component*> emetteurs;
    list<Triplet*>::iterator it = triplets.begin();
    while (it != triplets.end()) {
        if ((*it)->T_debit != 0) {
            CompteRacine* rac = CompteRacine::getRacine();
            Compte_component* nouv = rac->localiser((*it)->getCompte());
            emetteurs.push_back(nouv);
        }
        it++;
    }
    return emetteurs;
}

list<Compte_component*> Transaction::getRecepteurs() {
    list<Compte_component*> recepteurs;
    list<Triplet*>::iterator it = triplets.begin();
    while (it != triplets.end()) {
        if ((*it)->T_credit != 0) {
            CompteRacine* rac = CompteRacine::getRacine();
            Compte_component* nouv = rac->localiser((*it)->getCompte());
            recepteurs.push_back(nouv);
        }
        it++;
    }
    return recepteurs;
}

void TransactionManager::figerTransactions() {
    list<Transaction>::iterator it = transactions.begin();
    while (it != transactions.end()) {
        it->figer();
        it++;
    }
}

void TransactionManager::Rapprochement(Compte_component& c) {
    // partie front-end
    qDebug() << "Vous voulez rapprocher le compte " << c.getNom() << " de solde actuel : " << this->getSoldeLive(c)
             << "€ et avec un solde de dernier rapprochement : " << c.getSolde() << "€.";
    qDebug() << "Validez-vous le rapprochement ?";
    // s'il est validé.
    c.setSolde(this->getSoldeLive(c));
    Date date = Date();
    c.setDate_Rapprochement(date);
    for (TransactionManager::FilterCompteIterator it = this->getFilterCompteIterator(c); !it.isDone(); it.next()) {
        if (it.currentItem().getDate() <= date) {
            it.currentItem().figer();
        }
    }
    // s'il n'est pas validé
    return;
}

ostream& operator<<(ostream& f, Transaction& t) {
    f << "Transaction " << t.getReference() << " faite le " << t.getDate() << " : \n";
    list<Triplet*> triplets = t.getTriplets();
    list<Triplet*>::iterator itTriplet = triplets.begin();
    while (itTriplet != triplets.end()) {
        if ((*itTriplet)->getDebit()) {
            f << "Compte Emetteur " << (*itTriplet)->getCompte() << " debit : " << (*itTriplet)->getDebit() << "\n";
        } else {
            f << "Compte Recepteur " << (*itTriplet)->getCompte() << " credit : " << (*itTriplet)->getCredit() << "\n";
        }
        itTriplet++;
    }
    return f;
}
float TransactionManager::getSoldeLive(Compte_component& c, Date& date) {
    float SoldeRetour = 0;
    if (c.hasSon()) {
        //"parcours des fils";
        std::list<Compte_component*> listeFils = c.getFils();
        for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
            // récursivité;
            Compte_component* pt = dynamic_cast<Compte_component*>(*it);
            SoldeRetour = SoldeRetour + (this->getSoldeLive(*pt, date));
        }
    }
    for (TransactionManager::FilterCompteIterator it = this->getFilterCompteIterator(c); !it.isDone(); it.next()) {
        if (it.currentItem().getDate() <= date) {
            list<Triplet*> triplets = it.currentItem().getTriplets();
            float credit = 0;
            float Debit = 0;
            for (list<Triplet*>::iterator ite = triplets.begin(); ite != triplets.end(); ite++) {
                Triplet* tr;
                tr = dynamic_cast<Triplet*>(*ite);
                CompteRacine* rac = CompteRacine::getRacine();
                if (rac->localiser(tr->getCompte()) == (&c) && tr) {
                    credit += tr->getCredit();
                    Debit += tr->getDebit();
                }
            }
            if (typeid(c) == typeid(CompteActifs) || typeid(c) == typeid(CompteDepenses)) {
                SoldeRetour = SoldeRetour - (credit) + (Debit);
            } else
                SoldeRetour = SoldeRetour + (credit) - (Debit);
        }
    }
    return SoldeRetour;
};

float TransactionManager::getSoldeLive(Compte_component& c, Date& d1, Date& d2) {
    float SoldeRetour = 0;
    if (c.hasSon()) {
        // parcours des fils
        std::list<Compte_component*> listeFils = c.getFils();
        for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
            // récursivité;
            Compte_component* pt = dynamic_cast<Compte_component*>(*it);
            SoldeRetour = SoldeRetour + (this->getSoldeLive(*pt, d1, d2));
        }
    }

    for (TransactionManager::FilterCompteIterator it = this->getFilterCompteIterator(c); !it.isDone(); it.next()) {
        if ((it.currentItem().getDate() <= d2) && (d1 <= it.currentItem().getDate())) {
            list<Triplet*> triplets = it.currentItem().getTriplets();
            float credit = 0;
            float Debit = 0;
            for (list<Triplet*>::iterator ite = triplets.begin(); ite != triplets.end(); ite++) {
                Triplet* tr;
                tr = dynamic_cast<Triplet*>(*ite);
                CompteRacine* rac = CompteRacine::getRacine();
                if (rac->localiser(tr->getCompte()) == (&c) && tr) {
                    credit += tr->getCredit();
                    Debit += tr->getDebit();
                }
            }
            if (typeid(c) == typeid(CompteActifs) || typeid(c) == typeid(CompteDepenses)) {
                SoldeRetour = SoldeRetour - (credit) + (Debit);
            } else
                SoldeRetour = SoldeRetour + (credit) - (Debit);
        }
    }
    return SoldeRetour;
};

void TransactionManager::setSoldeInitial(Compte_component& nouvCompte, Compte_component& comptePropre, float solde) {
    // sélection d'un compte de passifs pour faire le transfert initial
    // CompteRacine* rac = CompteRacine::getRacine();
    // QString nomComptePassif;
    // if (rac->parcoursPassifs()) {  // au moins un compte de passifs existe
    //     // partie front-end help boris
    //     qDebug() << "Depuis quel compte passif effectuer la transaction ?";
    //     nomComptePassif = "Capitaux Propres";
    //     // je fais comme si un compte "Capitaux Propres" existe et est sélectionné par l'utilisateur

    // } else {  // pas de compte de passifs : on doit en créer un
    //     // partie front-end help boris
    //     qDebug() << "Nom du nouveau compte ?";
    //     nomComptePassif = "Capitaux Propres";  // nom compte entré par l'utilisateur
    //     rac->creerCompte(rac->getNom(), nomComptePassif, "passifs");  // création du nouveau compte de dépenses
    // }

    // // ici, on est sûr de disposer d'un compte de passif pour faire le virement initial

    // Compte_component* comptePropre = rac->localiser(nomComptePassif);  // on récupère un pointeur sur ce compte;

    // création des triplets
    std::list<Triplet*> Triplets;

    if (typeid(nouvCompte) == typeid(CompteActifs)) {
        Triplet* t1 = new Triplet(nouvCompte.getNom(), solde, 0);
        Triplet* t2 = new Triplet(comptePropre.getNom(), 0, solde);
        Triplets.push_back(t1);
        Triplets.push_back(t2);
    } else {
        Triplet* t1 = new Triplet(nouvCompte.getNom(), 0, solde);
        Triplet* t2 = new Triplet(comptePropre.getNom(), solde, 0);
        Triplets.push_back(t1);
        Triplets.push_back(t2);
    }

    // création de la transaction
    QString id = "TX0";
    QString titre = "Versement initial";
    Date* date = new Date();
    *date = Date();
    this->ajouterTransaction(*date, id, titre, Triplets);

    // on fait une sorte de rapprochement initial
    nouvCompte.setSolde(solde);
    nouvCompte.setDate_Rapprochement(*date);

    // cout<<compte_propre.getNom();
    // this->afficherTransactions(cout);
};

std::list<std::tuple<QString, float, QString>> TransactionManager::Bilan(
    Compte_component& c, Date& date, std::list<std::tuple<QString, float, QString>> listeInit) {
    if (typeid(c) == typeid(CompteActifs) || typeid(c) == typeid(ComptePassifs)) {
        std::list<std::tuple<QString, float, QString>> liste;
        float SoldeRetour = this->getSoldeLive(c, date);
        auto actualtuple = std::make_tuple(c.getNom(), SoldeRetour, c.getParent()->getNom());
        listeInit.push_back(actualtuple);

        // qDebug() << "-> Compte" << c.getNom() << "SoldeRetour :" << SoldeRetour;
        if (c.hasSon()) {
            // qDebug() << "|";  // passage au fils
            // qDebug() << "v";  // passage au fils

            // parcours des fils
            std::list<Compte_component*> listeFils = c.getFils();
            for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
                // récursivité;
                Compte_component* pt = dynamic_cast<Compte_component*>(*it);
                // if (typeid(*it) == typeid(CompteActifs) || typeid(*it) == typeid(ComptePassifs))
                listeInit.merge(this->Bilan(*pt, date, liste));
            }
        }
        return listeInit;
    }
}

std::list<list<std::tuple<QString, float, QString>>> TransactionManager::Bilan_general(Date& date) {
    float SoldeFinal = 0;
    CompteRacine* rac = CompteRacine::getRacine();
    std::list<std::tuple<QString, float, QString>> listeInit;
    std::list<list<std::tuple<QString, float, QString>>> listcourante;
    std::list<Compte_component*> listeFils = rac->getFils();
    for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
        Compte_component* pt = dynamic_cast<CompteActifs*>(*it);
        if (!pt) {
            Compte_component* pt = dynamic_cast<ComptePassifs*>(*it);
            if (pt && pt->getNom() != "Capitaux Propres") {
                std::list<std::tuple<QString, float, QString>> listeComptePassifs = this->Bilan(*pt, date, listeInit);
                listcourante.push_back(listeComptePassifs);
                SoldeFinal = SoldeFinal - this->getSoldeLive(*pt, date);
            }
        } else {
            std::list<std::tuple<QString, float, QString>> listeCompteActifs = this->Bilan(*pt, date, listeInit);
            listcourante.push_back(listeCompteActifs);
            SoldeFinal = SoldeFinal + this->getSoldeLive(*pt, date);
        }
    }
    auto actualtuple = std::make_tuple("Balance", SoldeFinal, "Actifs - Passifs");
    listeInit.push_back(actualtuple);
    listcourante.push_back(listeInit);
    return listcourante;
}

list<std::tuple<QString, float, QString>> TransactionManager::releve(
    Compte_component& c, Date& date_debut, Date& date_fin, list<std::tuple<QString, float, QString>> listeInit) {
    if (typeid(c) == typeid(CompteRecettes) || typeid(c) == typeid(CompteDepenses)) {
        if (date_fin <= date_debut) throw TresorerieException("ce n'est pas une période correct");
        list<std::tuple<QString, float, QString>> liste;
        float soldeCompte = this->getSoldeLive(c, date_debut, date_fin);
        auto actualtuple = std::make_tuple(c.getNom(), soldeCompte, c.getParent()->getNom());
        listeInit.push_back(actualtuple);
        if (c.hasSon()) {
            //"parcours des fils ";
            std::list<Compte_component*> listeFils = c.getFils();
            for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
                //"récursivité ";
                Compte_component* pt = dynamic_cast<CompteRecettes*>(*it);
                if (!pt) {
                    Compte_component* pt = dynamic_cast<CompteDepenses*>(*it);
                    if (pt) listeInit.merge(this->releve(*pt, date_debut, date_fin, liste));
                } else
                    listeInit.merge(this->releve(*pt, date_debut, date_fin, liste));
            }
        }
        return listeInit;
    }
}

std::list<list<std::tuple<QString, float, QString>>> TransactionManager::resultat(Date& date_debut, Date& date_fin) {
    float SoldeFinal = 0;

    CompteRacine* rac = CompteRacine::getRacine();
    std::list<std::tuple<QString, float, QString>> listeInit;
    std::list<list<std::tuple<QString, float, QString>>> listcourante;
    std::list<Compte_component*> listeFils = rac->getFils();

    for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
        Compte_component* pt = dynamic_cast<CompteRecettes*>(*it);
        if (!pt) {
            Compte_component* pt = dynamic_cast<CompteDepenses*>(*it);
            if (pt && pt->getNom() != "Capitaux Propres") {
                std::list<std::tuple<QString, float, QString>> listeCompteDepenses =
                    this->releve(*pt, date_debut, date_fin, listeInit);
                listcourante.push_back(listeCompteDepenses);
                SoldeFinal = SoldeFinal - this->getSoldeLive(*pt, date_debut, date_fin);
            }
        } else {
            std::list<std::tuple<QString, float, QString>> ListeCompteRecettes =
                this->releve(*pt, date_debut, date_fin, listeInit);
            listcourante.push_back(ListeCompteRecettes);
            SoldeFinal = SoldeFinal + this->getSoldeLive(*pt, date_debut, date_fin);
        }
    }
    auto actualtuple = std::make_tuple("Balance", SoldeFinal, "Recettes - Dépenses");
    listeInit.push_back(actualtuple);
    listcourante.push_back(listeInit);
    if (!(rac->contient("Résultat")))
        rac->creerCompte(rac->getNom(), "Résultat", false, "passifs");  // création du compte résultat
    Compte_component* Resultat = rac->localiser("Résultat");
    //this->setSoldeInitial(*Resultat, SoldeFinal);
    // est-ce bien correct de faire ça ??? (Victor)
    Resultat->setSolde(SoldeFinal);
    return listcourante;
}

bool TransactionManager::cloture() {
    float ResultatCredit = 0;
    float ResultatDebit = 0;
    Date date = Date();
    std::list<Triplet*> TripletsR;
    std::list<Triplet*> TripletsD;
    std::list<list<std::tuple<QString, float, QString>>> RELEVE = this->resultat(dateInit, date);
    CompteRacine* rac = CompteRacine::getRacine();
    for (auto it = RELEVE.begin(); it != RELEVE.end(); ++it) {
        for (auto ite = it->begin(); ite != it->end(); ++ite) {
            Compte_component* c = rac->localiser(std::get<0>(*ite));
            Compte_component* pt = dynamic_cast<CompteRecettes*>(c);
            if (pt && !(pt->hasSon())) {
                TripletsR.push_back(new Triplet(c, std::get<1>(*ite), 0));
                ResultatCredit = ResultatCredit + std::get<1>(*ite);
            } else {
                Compte_component* pt = dynamic_cast<CompteDepenses*>(c);
                if (pt && !(pt->hasSon())) {
                    TripletsD.push_back(new Triplet(c, 0, std::get<1>(*ite)));
                    ResultatDebit = ResultatDebit + std::get<1>(*ite);
                }
            }
        }
    }
    Compte_component* res = rac->localiser("Résultat");
    TripletsR.push_back(new Triplet(res, 0, ResultatCredit));
    TripletsD.push_back(new Triplet(res, ResultatDebit, 0));
    QString id1 = "CLD";  // en front end -> c'est l'utilisateur qui choisit l'id et le titre
    QString titre1 = "Cloture";
    this->ajouterTransaction(date, id1, titre1, TripletsD);
    QString id2 = "CLR";  // en front end -> c'est l'utilisateur qui choisit l'id et le titre
    QString titre2 = "Cloture";
    this->ajouterTransaction(date, id2, titre2, TripletsR);
    float consomation = ResultatCredit - ResultatDebit;
    if (consomation >= 0) {
        if (!(rac->contient("Excédent")))
            rac->creerCompte(rac->getNom(), "Excédent",false, "passifs");  // création du compte Excédent
        Compte_component* Exc = rac->localiser("Excédent");
        std::list<Triplet*> Excedent;
        Excedent.push_back(new Triplet(res, 0, consomation));
        Excedent.push_back(new Triplet(Exc, consomation, 0));
        QString id3 = "cons";  // en front end -> c'est l'utilisateur qui choisit l'id et le titre
        QString titre3 = "Constatation";
        this->ajouterTransaction(date, id3, titre3, Excedent);
        return true;
    } else {
        if (!(rac->contient("Déficit")))
            rac->creerCompte(rac->getNom(), "Déficit", false, "passifs");  // création du compte Excédent
        Compte_component* Def = rac->localiser("Déficit");
        std::list<Triplet*> Deficit;
        Deficit.push_back(new Triplet(res, 0, consomation));
        Deficit.push_back(new Triplet(Def, consomation, 0));
        QString id3 = "cons";  // en front end -> c'est l'utilisateur qui choisit l'id et le titre
        QString titre3 = "Constatation";
        this->ajouterTransaction(date, id3, titre3, Deficit);
        return false;
    }
    // il reste à mettre à jour le solde du compte résultat et déficit grâce au rapprochement des comptes.
}
