#ifndef TRANSACTION_H
#define TRANSACTION_H
#include<QString>
#include <QTextStream>
#include<iostream>
#include<time.h>
#include <QDebug>
#include <QFile>
#include <QIODevice>

using namespace std;

ostream& operator<<(ostream& stream, const QString& qstr);




const QString NMois[] = {"janvier", "fevrier", "mars"     , "avril"  , "mai"     , "juin"    ,
                          "juillet", "aout"   , "septembre", "octobre", "novembre", "decembre"};


class TresorerieException {
public:
        TresorerieException(const QString& message) :info(message) {}
        QString getInfo() const { return info; }
private:
        QString info;
};

class Date
{
    int seconde;
    int minute;
    int heure;
    int jour;
    int mois;
    int annee;
public:
    Date()
    {
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
    const int& getSeconde() const{return seconde;}
    const int& getMinute() const{return minute;}
    const int& getHeure() const{return heure;}
    const int& getJour() const{return jour;}
    const int& getMois() const{return mois;}
    const int& getAnnee() const{return annee;}
};

ostream& operator<<(ostream& f, const Date& d);

bool operator<=(const Date& d1, const Date& d2);

class Transaction
{
    friend class TransactionManager;
    Date& date;
    QString id;
    QString titre;
    bool modifiable = true;
    Triplet** triplets;
    int nbTriplet;
    Transaction(Date& _date, QString& _id, QString& _titre, Triplet** _triplets, int _nbTriplet):
        date(_date), id(_id), titre(_titre), triplets(_triplets), nbTriplet(_nbTriplet) {}
    ~Transaction();
    void figer()
    {
        modifiable = false;
    }
public:
    Date& getDate() const {return date;}
    QString getReference() const {return id;}
    QString getTitre() const {return titre;}
    Compte** getEmetteurs() const;
    Compte** getRecepteurs() const;
    int getNbTriplet() const {return nbTriplet;}
    Triplet** getTriplets() const {return triplets;}
};

ostream& operator<<(ostream& f, const Transaction& t);


class TransactionManager
{
    friend class FilterCompteIterator;
    Transaction** transactions = nullptr;
    QString filename = "transactions.txt";
    unsigned int nb=0;
    unsigned int nbMax=0;
    TransactionManager() = default;
    ~TransactionManager()
    {
        //save();
        for (unsigned int n = 0; n<nb; n++)
        {
            delete transactions[n];
        }
        delete[] transactions;
    }

public:
    static TransactionManager& getTransactionManager()
    {
        static TransactionManager tm;
        return tm;
    }
    int getNb(){return nb;}
    Transaction** getTransactions(){return transactions;}
    void ajouterTransaction(Date& date, QString& id, QString& titre, Triplet** triplets, int nbTriplet);
    void ajouterPointeur(Transaction* transaction);
    void afficherTransactions(ostream& f);
    void figerTransactions();
    void save();
    void load (const QString& f);
    class Iterator
    {
        friend class TransactionManager;
        const TransactionManager& instance = TransactionManager::getTransactionManager();
        unsigned int i = 0;
    public:
        Iterator() = default;
        bool isDone() const
        {
            return i == instance.nb;
        }
        void next()
        {
            if(isDone())
            {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            i++;
        }
        Transaction& currentItem()
        {
            if(isDone())
            {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            if(!instance.transactions[i]->modifiable)
            {
                throw TresorerieException("Erreur, transaction figée");
            }
            else
            {
                return *instance.transactions[i];
            }
        }

        const Transaction& currentItemConst() const
        {
            if(isDone())
            {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            return *instance.transactions[i];
        }
    };
    Iterator getIterator()
    {
        return Iterator();
    }

    class FilterCompteIterator
    {
        friend class TransactionManager;
        const TransactionManager& instance = getTransactionManager();
        unsigned int i = 0;
        Compte& compte;
    public:
        void goToNextTransaction()
        {
            for (; !isDone(); i++)
            {
                for (int j = 0; j< instance.transactions[i]->nbTriplet; j++)
                {
                    if (instance.transactions[i]->triplets[j]->getCompte().getId() == compte.getId())
                    {
                        return;
                    }
                }
            }
        }
        FilterCompteIterator(Compte& c): compte(c)
        {
            goToNextTransaction();
        }

    public:
        bool isDone() const
        {
            return i == instance.nb;
        }
        void next()
        {
            if(isDone())
            {
                throw TresorerieException("parcours terminé");
            }
            i++;
            goToNextTransaction();
        }
        Transaction& currentItem()
        {
            if(isDone())
            {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            if(!instance.transactions[i]->modifiable)
            {
                throw TresorerieException("Erreur, transaction figée");
            }
            else
            {
                return *instance.transactions[i];
            }
        }

        const Transaction& currentItemConst() const
        {
            if(isDone())
            {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            return *instance.transactions[i];
        }
    };
    FilterCompteIterator getFilterCompteIterator(Compte& c)
    {
        return FilterCompteIterator(c);
    }

    class FilterDateIterator
    {
        friend class TransactionManager;
        const TransactionManager& instance = TransactionManager::getTransactionManager();
        unsigned int i = 0;
        const Date& date;
        void goToNextTransaction()
        {
            for (;i<instance.nb; i++)
            {
                if (instance.transactions[i]->date <= date)
                {
                    return;
                }
            }
        }
        FilterDateIterator(const Date& _date) : date(_date)
        {
            goToNextTransaction();
        }

    public:
        bool isDone() const
        {
            return i == instance.nb;
        }
        void next()
        {
            if(isDone())
            {
                throw TresorerieException("parcours terminé");
            }
            i++;
            goToNextTransaction();
        }
        Transaction& currentItem() const
        {
            if(isDone())
            {
                throw TresorerieException("parcours terminé");
            }
            if(instance.transactions[i]->modifiable)
            {
                return *instance.transactions[i];
            }
            else
            {
                return *instance.transactions[i];
            }
        }
        const Transaction& currentItemConst() const
        {
            if(isDone())
            {
                throw TresorerieException("parcours terminé");
            }
            return *instance.transactions[i];
        }
    };

    FilterDateIterator getFilterDateIterator(Date& d)
    {
        return FilterDateIterator(d);
    }


};



#endif // TRANSACTION_H