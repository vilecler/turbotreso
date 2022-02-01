#include <QApplication>
#include <QtWidgets>
#include <iostream>

#include "compte.h"
#include "transaction.h"

int main(int argc, char* argv[]) {
    try {
        QApplication app(argc, argv);

        // Tests :
        CompteRacine* rac = CompteRacine::getRacine();  // création du compte racine

        // Soit on crée les comptes à partir de la sauvegarde (comptes.xml)
        // rac.load();

        // Soit on les crée manuellement

        /*rac->creerCompte("racine", "ComptePassifs1", "passifs");  // puisqu'on crée depuis la racine, on précise le type
        rac->creerCompte("racine", "CompteDepenses1", "depenses");
        rac->creerCompte("racine", "CompteDepenses2", "depenses");
        rac->creerCompte("racine", "CompteDepenses3", "depenses");
        rac->creerCompte("CompteDepenses1", "ComptesDepenses4");
        rac->creerCompte("CompteDepenses1", "ComptesDepenses5");
        rac->creerCompte("ComptePassifs1", "ComptePassifs2");  // pas besoin de préciser le type, il est déduit
        // sauvegarde des comptes automatique à la destruction du compte racine
        rac->creerCompte("racine", "Comptenouveau", "recettes");

        TransactionManager& tm = TransactionManager::getTransactionManager();
        Date date1 = Date();
        Date date2 = Date(1, 1, 1, 7, 7, 2020);
        Date date3 = Date(1, 1, 1, 7, 7, 2022);
        ComptePassifs c1 = ComptePassifs("test1", nullptr);
        CompteActifs c2 = CompteActifs("test2", nullptr);
        CompteActifs c3 = CompteActifs("test3", nullptr);
        CompteActifs c4 = CompteActifs("transac2", nullptr);
        CompteActifs c5 = CompteActifs("transac2-2", nullptr);

        // test fonction setSoldeInitial(Compte_component& nouvCompte, float solde)
        qDebug() << "---Test fonction setSoldeInitial---";
        rac->creerCompte("racine", "Capitaux Propres", "passifs");  // on crée un nouveau compte de passifs
        rac->creerCompte("racine", "actifSI", "actifs");  // on crée un nouveau compte d'actifs
        tm.setSoldeInitial(*rac->localiser("actifSI"), 500);  // on souhaite initialiser son solde
        Triplet tripletA = Triplet(rac->localiser("actifSI"), 250, 0);
        Triplet tripletP = Triplet(rac->localiser("Capitaux Propres"), 0, 250);
        list<Triplet*> lAP = {&tripletA, &tripletP};
        QString idAP = "111";
        QString titreAP = "TEST GSL";
        tm.ajouterTransaction(date2, idAP, titreAP, lAP);

        qDebug() << "Test GSL : " << tm.getSoldeLive(*rac->localiser("actifSI"));
        qDebug() << "---Fin test fonction setSoldeInitial---";

        Triplet triplet1 = Triplet(&c1, 5, 0);
        Triplet triplet2 = Triplet(&c2, 0, 5);
        Triplet triplet3 = Triplet(&c3, 7, 0);
        Triplet triplet4 = Triplet(&c4, 56, 0);
        Triplet triplet5 = Triplet(&c5, 0, 56);

        //        list<Triplet> l1 = {triplet2, triplet1, triplet3};
        list<Triplet*> l1 = {&triplet1, &triplet2};
        list<Triplet*> l2 = {&triplet4, &triplet5};

        QString titre = "ok";
        QString id = "1234";
        QString titre2 = "transac2";
        QString id2 = "89";

        tm.ajouterTransaction(date1, id, titre, l1);
        tm.ajouterTransaction(date2, id2, titre2, l2);

        */
        qDebug() << "Tests";

        rac->creerCompte("racine", "Capitaux Propres", "passifs", false);
        rac->creerCompte("racine", "ComptePassifs1", "passifs");  // puisqu'on crée depuis la racine, on précise le type
        rac->creerCompte("racine", "CompteActif1", "actifs");
        rac->creerCompte("racine", "CompteActif2", "actifs", false);
        rac->creerCompte("CompteActif1", "CompteActif3", "actifs", false);
        rac->creerCompte("CompteDepenses1", "ComptesDepenses4", "", false);
        rac->creerCompte("CompteDepenses1", "ComptesDepenses5", "", false);
        rac->creerCompte("ComptePassifs1", "ComptePassifs2", "", false);

        TransactionManager& tm = TransactionManager::getTransactionManager();
        tm.setSoldeInitial(*rac->localiser("ComptePassifs2"), *rac->localiser("Capitaux Propres"), 1000);
        tm.setSoldeInitial(*rac->localiser("CompteActif2"), *rac->localiser("Capitaux Propres"), 300);
        tm.setSoldeInitial(*rac->localiser("CompteActif3"), *rac->localiser("Capitaux Propres"), 500);

        Date date1 = Date();
        std::list<list<std::tuple<QString, float, QString>>> BILAN = tm.Bilan_general(date1);
        for (auto it =BILAN.begin(); it !=BILAN.end();++it){
            for (auto ite =it->begin(); ite !=it->end();++ite){
                           qDebug()<<"BILAN : "<<std::get<0>(*ite)<<std::get<1>(*ite)<<std::get<2>(*ite);
            }
        }

        rac->creerCompte("racine", "CompteDepenses1", "depenses");
        rac->creerCompte("racine", "CompteDepenses2", "depenses", false);
        rac->creerCompte("CompteDepenses1", "CompteDepenses3", "depenses", false);
        rac->creerCompte("racine", "Compterecettes1", "recettes");
        rac->creerCompte("racine", "Compterecettes2", "recettes", false);
        rac->creerCompte("Compterecettes1", "Compterecettes3", "recettes", false);

        Date date2 = Date(1, 1, 1, 7, 7, 2020);
        Date date3 = Date(1, 1, 1, 7, 7, 2022);

        Triplet tripletA = Triplet(rac->localiser("CompteDepenses2"), 250, 0);
        Triplet tripletP = Triplet(rac->localiser("Compterecettes2"), 0, 250);

        Triplet triplet1 = Triplet(rac->localiser("CompteDepenses3"), 5, 0);
        Triplet triplet2 = Triplet(rac->localiser("CompteActif2"), 0, 5);


        list<Triplet*> l1 = {&triplet1, &triplet2};

        list<Triplet*> lAP = {&tripletA, &tripletP};

        QString idAP = "111";
        QString titreAP = "TEST GSL";

        QString titre = "ok";
        QString id = "1234";
        QString titre2 = "transac2";
        QString id2 = "89";

        tm.ajouterTransaction(date1, id, titre, l1);
        tm.ajouterTransaction(date2, idAP, titreAP, lAP);

        std::list<list<std::tuple<QString, float, QString>>> RELEVE = tm.resultat(date1, date3);

        for (auto it =RELEVE.begin(); it !=RELEVE.end();++it){
            for (auto ite =it->begin(); ite !=it->end();++ite){
                           qDebug()<<"RELEVE : "<<std::get<0>(*ite)<<std::get<1>(*ite)<<std::get<2>(*ite);

            }
        }

        Compte_component* Resultat = rac->localiser("Résultat");
        cout<<Resultat->getSolde();
        bool res=tm.cloture();
        cout<<Resultat->getSolde();
        cout<<res;
        //tm.afficherTransactions(cout);

        // tm.save();

        /* tm.load();
         cout<<"après le load de transactions------------\n";
         tm.afficherTransactions(cout);*/

        return 0;
    } catch (Exception exception) {
        qDebug() << exception.getInfo();
    }
}
