#ifndef COMPTEMANAGER_H
#define COMPTEMANAGER_H

#include<string>
#include<iostream>
#include<QString>
#include <QTextStream>


class CompteManager {
    QString type; // initialisé par défaut à ""
    friend class CompteRacine;
    CompteManager(const CompteManager& c) = default;
    CompteManager& operator=(const CompteManager& c) = default;
    CompteManager(const QString& ty) :
        type(ty) {}
    CompteManager() = default;
    ~CompteManager()=default;
public:
    QString getType() const { return type; }
};



#endif // COMPTEMANAGER_H
