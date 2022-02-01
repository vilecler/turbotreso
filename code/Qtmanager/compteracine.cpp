#include "compteracine.h"
#include<fstream>
#include<algorithm>
#include<QFile>
#include<QFileInfo>
#include<QDebug>


CompteManager& CompteRacine::creerCompteManager(const QString& type){
    for (unsigned int i = 0; i < nb; i++)
        if (comptesManager[i].getType() == type)
            throw CompteRacineException("Compte deja existant");
    if (nb == nbMax) { // agrandissement
        CompteManager* newtab = new CompteManager[nbMax*2+1]; // création d'un tableau plus grand
        // recopie des éléments
        for (unsigned int i = 0; i < nb; i++) newtab[i] = comptesManager[i];
        // mise a jour des attributs
        nbMax = nbMax * 2 + 1;
        CompteManager* old = comptesManager;
        comptesManager = newtab;
        // destruction de l'ancien tableau
        delete[] old;
    }
    // ajout du nouveau compte
    comptesManager[nb++] = CompteManager(type);
    return comptesManager[nb - 1];
}

CompteManager& CompteRacine::getCompteManager(const QString& n) {
    for (unsigned int i = 0; i < nb; i++)
        if (comptesManager[i].getType() == n)
            return comptesManager[i];
    throw CompteRacineException("Compte inexistant");
}

const CompteManager& CompteRacine::getCompteManager(const QString& n) const {
    for (unsigned int i = 0; i < nb; i++)
        if (comptesManager[i].getType() == n)
            return comptesManager[i];
    throw CompteRacineException("Compte inexistant");
}


void CompteRacine::load(const QString& f) {
    qDebug()<<"Chargement des comptes dans "<<f<<"\n";
    if (filename != f) save();
    filename=f;

    QFile fichier(filename);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"erreur ouverture lecture fichier compte";
        throw CompteRacineException("erreur ouverture lecture fichier compte");
    }

    QTextStream flux(&fichier);
    //qDebug()<<"pos="<<flux.pos()<<"\n";
    //if (flux.atEnd()) qDebug()<<"at end !";
    QString ligne;
    while(!flux.atEnd()){
         ligne = flux.readLine();
         qDebug()<<"l="<<ligne<<"\n";
         QStringList list = ligne.split(';');
         QString type=list.at(0);
         /*//Il faudra changer ca pour recuperer les attribus de chaque compte
         QString nom=list.at(1);
         double solde=list.at(2).toDouble();
         */
         if (type!=""){
             creerCompteManager(type);
             qDebug() << QString("Chargement du compte %1").arg(getCompteManager(type).getType());
          }
    }
    fichier.close();
}


void CompteRacine::save() const {
    // Création d'un objet QFile
    QFile file(filename);
    // On ouvre notre fichier en ecriture seule et on vérifie l'ouverture
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) throw CompteRacineException("Erreur sauvegarde fichier compte");
    QTextStream flux(&file);
    for (unsigned int i = 0; i < nb; i++) {
        flux << comptesManager[i].getType() << ";\n";

    }
    file.close();
}



