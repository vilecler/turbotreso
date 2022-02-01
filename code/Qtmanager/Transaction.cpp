#include<algorithm>
#include "Transaction.h"

using namespace std;

ostream& operator<<(ostream& stream, const QString& qstr)
{
    stream << qstr.toStdString();
    return stream;
}


void TransactionManager::load(const QString& f) {
        if (filename != f) save();
        filename = f;
        QFile fin(filename);
        if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) throw TresorerieException("Erreur sauvegarde fichier compte");
        QTextStream flux(&fin);
        QString ligne;
        while (!flux.atEnd()) {
            ligne = flux.readLine();
            QStringList list = ligne.split(";");
            QString seconde = list.at(0);
            QString minute = list.at(1);
            QString heure = list.at(2);
            QString jour = list.at(3);
            QString mois = list.at(4);
            QString annee = list.at(5);
            QString id = list.at(6);
            QString titre = list.at(7);
            QString snbTriplet = list.at(8);
            Date date(seconde.toInt(), minute.toInt(), heure.toInt(), jour.toInt(), mois.toInt(), annee.toInt());
            int nbTriplet = snbTriplet.toInt();
            Triplet* triplets[nbTriplet];
            for (int i = 0; i<nbTriplet; i++)
            {
                QString nomCompte = list.at(3*i+9);
                QString debit = list.at(3*i+10);
                QString credit = list.at(3*i+11);
                Compte& compte = *CompteManager::getCompte(nomCompte);
                triplets[i] = new Triplet(compte, credit.toInt(), debit.toInt());

            }
        }
        fin.close(); // close file
}

void TransactionManager::save(){
    TransactionManager tm = getTransactionManager();
        QFile fout(filename);
        if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) throw TresorerieException("Erreur sauvegarde fichier compte");
        QTextStream flux(&fout);
        flux<<nb<<";"<<nbMax<<";";
        for (unsigned int i = 0; i < nb; i++) {
            flux<< getTransactions()[i]->getDate().getSeconde()<<";"<<
                transactions[i]->date.getMinute()<<";"<<transactions[i]->date.getHeure()<<";"<<
                transactions[i]->date.getJour()<<";"<<transactions[i]->date.getMois()<<";"<<transactions[i]->date.getAnnee()<<";"<<
                transactions[i]->getReference()<<";"<<tm.transactions[i]->getTitre()<<";"<<transactions[i]->getNbTriplet()<<";";
            for (int j = 0; j < transactions[i]->nbTriplet; j++)
            {
                flux<<transactions[i]->triplets[j]->getCompte().getNom()<<";"<<transactions[i]->triplets[j]->getDebit()<<";"<<
                transactions[i]->triplets[j]->getCredit()<<";";
            }
            flux<<"\n";
        }
        fout.close();
}

void TransactionManager::ajouterPointeur(Transaction* t)
{
    if (nb == nbMax)
    {
        Transaction** newtab = new Transaction*[nbMax + 50];
                for (unsigned int i = 0; i < nb; i++) newtab[i] = transactions[i];
                nbMax = nbMax + 50;
                Transaction** old = transactions;
                transactions = newtab;
                delete[] old;
    }
        transactions[nb] = t;
        nb++;
}

void TransactionManager::ajouterTransaction(Date& date, QString& id, QString& titre, Triplet** triplets, int nbTriplet)
{
    int s = 0;
    for(int i = 0; i<nbTriplet; i++)
    {
        s += triplets[i]->getCredit();
        s -= triplets[i]->getDebit();
    }
    if (s!=0)
    {
        throw TresorerieException("Erreur, Transaction incorrect, verifiez les montants...");
    }
    Transaction* t = new Transaction(date, id, titre, triplets, nbTriplet);
    ajouterPointeur(t);
}

void TransactionManager::afficherTransactions(ostream& f)
{
    for (unsigned int i = 0; i<nb; i++)
    {
        f<<*transactions[i];
    }
}

Compte** Transaction::getEmetteurs() const
{
    Compte** emetteurs = new Compte*[nbTriplet];
    int n = 0;
    for (int i = 0; i<nbTriplet; i++)
    {
        if (triplets[i]->getDebit())
        {
            emetteurs[n] = &triplets[i]->getCompte();
                n++;
        }

    }
    emetteurs[n] = nullptr;
    return emetteurs;
}

Compte** Transaction::getRecepteurs() const
{
    Compte** recepteurs = new Compte*[nbTriplet];
    int n = 0;
    for (int i = 0; i<nbTriplet; i++)
    {
        if (triplets[i]->getCredit())
        {
            recepteurs[n] = &triplets[i]->getCompte();
            n++;
        }

    }
    recepteurs[n] = nullptr;
    return recepteurs;
}

void TransactionManager::figerTransactions()
{
    for (unsigned int i = 0; i<nb; i++)
    {
        transactions[i]->figer();
    }
}

ostream& operator<<(ostream& f, const Transaction& t)
{
    f<<"Transaction "<< t.getReference()<<" faite le "<< t.getDate() << " : \n";
    Triplet** triplets = t.getTriplets();
    for (int i = 0; i<t.getNbTriplet(); i++)
    {
        if(triplets[i]->getDebit())
        {
            f<<"Compte Emetteur "<< triplets[i]->getCompte().getNom() << " debit : " << triplets[i]->getDebit() << "\n";
        }
         else
         {
            f<<"Compte Recepteur "<< triplets[i]->getCompte().getNom() << " credit : " << triplets[i]->getCredit() << "\n";
         }
    }
    return f;
}

Transaction::~Transaction()
{
    cout<<"destruction transaction";
    for (int i = 0; i<nbTriplet; i++)
    {
        delete triplets[i];
    }
    delete[] triplets;
}




Date::Date(int _seconde, int _minute, int _heure, int _jour, int _mois, int _annee)
{
    if (_seconde>59)
    {
        throw TresorerieException("Valeur des secondes invalide");
    }
    else
    {
        seconde = _seconde;
    }
    if (_minute>59)
    {
        throw TresorerieException("Valeur des minutes invalide");
    }
    else
    {
        minute = _minute;
    }
    if (_heure>23 || _heure<0)
    {
        throw TresorerieException("Valeur des minutes invalide");
    }
    else
    {
        heure = _heure;
    }
    if (_jour>31 || _jour<1)
    {
        throw TresorerieException("Valeur du jour invalide");
    }
    else
    {
        jour = _jour;
    }
    if (_mois>12 || _mois<1)
    {
        throw TresorerieException("Valeur du mois invalide");
    }
    else
    {
        mois = _mois;
    }
    annee = _annee;
}

ostream& operator<<(ostream& f, const Date& d)
{
    f<<d.getJour()<< " " << NMois[d.getMois()]<< " " << d.getAnnee() << " a " << d.getHeure()
    << "h " << d.getMinute() << "min " << d.getSeconde() << "sec ";
    return f;
}

bool operator<=(const Date& d1, const Date& d2)
{
    if (d1.getAnnee() > d2.getAnnee())
    {
        return false;
    }
    if (d1.getMois() > d2.getMois())
    {
        return false;
    }
    if (d1.getJour() > d2.getJour())
    {
        return false;
    }
    if (d1.getHeure() > d2.getHeure())
    {
        return false;
    }
    if (d1.getMinute() > d2.getMinute())
    {
        return false;
    }
    if (d1.getSeconde() > d2.getSeconde())
    {
        return false;
    }
    return true;
}
