#ifndef COMPTERACINE_H
#define COMPTERACINE_H

#include "comptemanager.h"
#include<QString>
#include <QTextStream>
#include<iostream>
#include <QDebug>

using namespace std;
class CompteRacineException {
public:
    CompteRacineException(const QString& message) :info(message) {}
    QString getInfo() const { return info; }
private:
    QString info;
};







class CompteRacine
{
    CompteManager* comptesManager=nullptr;
    unsigned int nbMax=0;
    unsigned int nb=0;
    QString filename = "ComptesManager.dat";//il fqudrait plus tard faire en sorte que Il recupere le dernier fichier automatiquement
    CompteRacine() = default;
    ~CompteRacine() { save();  delete[] comptesManager; }
    // constructeur de rCompteRacineecopie
    CompteRacine(CompteRacine& m)=delete;
    // opérateur d'affectation
    CompteRacine& operator=(CompteRacine& m)=delete;
public:
    //fonction static pour avoir qu'un compteRacine tt le temps
    static CompteRacine& getRacine() { static CompteRacine m;  return m; }
        CompteManager& creerCompteManager(const QString& type);
        //Accesseur sur Compte manager en recherche du type
        CompteManager& getCompteManager(const QString& n);
        const CompteManager& getCompteManager(const QString& n) const;
        int getNb()const{ return nb; }

        // destructeur
        void setFilename(const QString& f) { filename = f; }
        void load(const QString& f);
        void save() const;

        //ITERATEUR DE COMPTE RACINE
        class Iterator {
            friend CompteRacine;
            CompteRacine* instance=nullptr;
            unsigned int i = 0;
            Iterator(CompteRacine* i):instance(i){}
        public:
            Iterator() = default;
            bool isDone() const { return instance == nullptr || i == instance->nb; }
            void next() {if (isDone()) throw CompteRacineException("erreur, next sur un iterateur en fin de sequence d'iteration");
                i++;
            }
            CompteManager& currentItem() const {
                if (isDone())
                    throw CompteRacineException("erreur, indirection sur un iterateur en fin de sequence d'iteration");
                return instance->comptesManager[i];
            }
        };
        Iterator getIterator() { return Iterator(this); }


        //ITERATEUR DE COMPTE RACINE CONST(itere de comptes manager)
        class ConstIterator {
            friend CompteRacine;
            const CompteRacine* instance = nullptr;
            unsigned int i = 0;
            ConstIterator(const CompteRacine* i) :instance(i) {}
        public:
            ConstIterator() = default;
            bool isDone() const { return instance == nullptr || i == instance->nb; }
            void next() {
                if (isDone()) throw CompteRacineException("erreur, next sur un iterateur en fin de sequence d'iteration");
                i++;
            }
            const CompteManager& currentItem() const {
                if (isDone())
                    throw CompteRacineException("erreur, indirection sur un iterateur en fin de sequence d'iteration");
                return instance->comptesManager[i];
            }
            int getI()const{ return i;}
        };
        ConstIterator getConstIterator() const { return ConstIterator(this); }
        ConstIterator getIterator() const { return ConstIterator(this);
        }


        //ITERATEUR DE COMPTE MANAGER
        class iteratorMan {
            friend class CompteRacine;
            CompteManager* courant=nullptr;
            iteratorMan(CompteManager* t) :courant(t) {}
        public:
            iteratorMan() = default;
            iteratorMan& operator++() { courant++; return *this; }
            CompteManager& operator*() const { return *courant;  }
            bool operator!=(iteratorMan it) const { return courant != it.courant; }
        };
        iteratorMan begin() { return iteratorMan(comptesManager); }
        iteratorMan end() { return iteratorMan(comptesManager+nb); }

         //ITERATEUR DE COMPTE MANAGER CONST
        class const_iteratorMan {
            friend class CompteRacine;
            const CompteManager* courant = nullptr;
            const_iteratorMan(const CompteManager* t) :courant(t) {}
        public:
            const_iteratorMan() = default;
            const_iteratorMan& operator++() { courant++; return *this; }
            const CompteManager& operator*() const { return *courant; }
            bool operator!=(const_iteratorMan it) const { return courant != it.courant; }
        };
        const_iteratorMan begin() const { return const_iteratorMan(comptesManager); }
        const_iteratorMan end() const { return const_iteratorMan(comptesManager + nb); }
        const_iteratorMan cbegin() const { return const_iteratorMan(comptesManager); }
        const_iteratorMan cend() const { return const_iteratorMan(comptesManager + nb); }
};

#endif // COMPTERACINE_H
