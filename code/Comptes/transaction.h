#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <time.h>

#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QTextStream>
#include <iostream>

#include "compte.h"
/*!
 * \file transaction.h
 * \brief Header, génère et gère toutes les transactions
 * \author Martin Boyer, Bastien Hubasher, Victor Kerjean
 * \version 0.1
 */

using namespace std;

ostream& operator<<(ostream& stream, const QString& qstr);

/*! \class Triplets
 * \brief Classe permettant de créer et gérer des triplets, stockés dans les transactions
 *  Les triplets qui prennent en paramètre un compte, un montant de débit et un montant de crédit permettent de stocker
 * des opérations dans une transaction. Chaque triplet représente le débit ou le crédit d'un compte
 */
class Triplet {
    friend class Transaction;

   private:
    QString T_compte; /*!< Pointeur vers le compte. Le fait d'utiliser un pointeur ici permet de pouvoir faire des dynamics
                     cast opour rettrouver le type du compte, ce qui est nécessaire à la sauvegarde*/
    float T_debit; /*!< Montant du débit*/
    float T_credit; /*!< Montant du crédit*/

   public:
   void setName(QString nouv);
    /*!
     *  \brief Destructeur
     *  Destructeur de la classe Triplets
     */
    ~Triplet() = default;
    /*!
     *  \brief Constructeur
     *  Constructeur de la classe Triplets
     *  \param compte : ponteur vers le compte
     *  \param debit : Montant du débit
     * \param credit : Montant du crédit
     */
    Triplet(QString compte, const float debit, const float credit);
    Triplet(Compte_component* compte, const float debit, const float credit);
    /*!
     *  \brief Accesseur de compte
     *  Méthode permettant d'acceder au compte appartenant au triplet
     * \return un pointeur vers le compte
     */
    QString getCompte() { return T_compte; };
    /*!
     *  \brief Accesseur de Débit
     *  Méthode permettant d'acceder au montant du Débit
     * \return un flottant correspondant au montant
     */
    const float getDebit() { return T_debit; };
    /*!
     *  \brief Accesseur de Crédit
     *  Méthode permettant d'acceder au montant du crédit
     * \return un flottant correspondant au montant
     */
    const float getCredit() { return T_credit; };
};

/*! \class TresorerieException
 * \brief Classe permettant de gérer les exceptions liées aux transactions
 */
class TresorerieException {
   public:
    /*!
     *  \brief Constructeur de Tresorerieexception
     *  \param un message indiquant le motif du déclenchement de l'exception
     * \return un flottant correspondant au montant
     */
    TresorerieException(const QString& message) : info(message) {}
    QString getInfo() const { return info; }

   private:
    QString info;
};

ostream& operator<<(ostream& f, const Date& d);

bool operator<=(const Date& d1, const Date& d2);

/*! \class Transaction
 * \brief Classe permettant de créer des transactions
 *
 *Cette classe permet d'instancier des transactions. Ces transactions seront elle-même gérées par un manager de
 *transactions
 *
 */
class Transaction {
    friend class TransactionManager;
    Date& date; /*!< Date de la transaction*/
    QString id; /*!< ID de la transaction*/
    QString titre; /*!< Motif de la transaction*/
    bool modifiable = true; /*!< Verouillage de la transaction pemettant de la rendre modifiable ou non*/
    list<Triplet*> triplets; /*!< Liste de Triplets associés à la transaction*/
    /*!
     *  \brief Constructeur privé de Transaction
     *  Ce constructeur est privé car nous avons mis en place un Singleton. Le privilège de la création des transactions
     * et réservé au manager des transactions \param _date : date de la transaction \param _id : ID de la transaction
     * \param _titre : motif de la transaction
     * \param _triplets : liste de triplets associés à cette transaction
     */
    Transaction(Date& _date, QString& _id, QString& _titre, list<Triplet*> _triplets)
        : date(_date), id(_id), titre(_titre), triplets(_triplets) {}
    /*!
     *  \brief Verouillage de la Transaction
     *  Cette méthode permet de rendre non modifiable la transaction
     */
    void figer() { modifiable = false; }

   public:
   void detruire();
    /*!
     *  \brief Accesseur de date de transaction
     * \return la date associée à la transaction
     */
    Date& getDate() const { return date; }
    /*!
     *  \brief Accesseur d'ID de transaction
     * \return l'ID associé à la transaction
     */
    QString getReference() const { return id; }
    /*!
     *  \brief Accesseur de motif de transaction
     * \return le motif associé à la transaction
     */
    QString getTitre() const { return titre; }
    /*!
     *  \brief Accesseur de comptes emetteurs de la transaction
     * Cet accesseur permet d'obtenir une liste de tous les comptes emetteurs de la transaction, c'est à dire ceux ayant
     * réalisé un transfert d'argent. \return une liste de pointeurs sur tous les comptes emetteurs de la transaction
     */
    list<Compte_component*> getEmetteurs();
    /*!
     *  \brief Accesseur de comptes recepteurs de la transaction
     * Cet accesseur permet d'obtenir une liste de tous les comptes recepteurs de la transaction, c'est à dire les
     * comptes ayant reçu un transfert d'argent. \return une liste de pointeurs sur tous les comptes recepteurs de la
     * transaction
     */
    list<Compte_component*> getRecepteurs();
    /*!
     *  \brief Accesseur de triplets de la transaction
     * \return une liste de pointeurs sur tous les triplets associés à la transaction
     */
    list<Triplet*> getTriplets() const { return triplets; }
};

/*!
 *  \brief Surcharge de l'opérateur "<<" pour les transactions
 * Cette surcharge permet d'afficher une transaction
 * \param f :une référence vers un ostream
 * \param t :une référence vers une transaction
 * \return une liste de pointeurs sur tous les comptes recepteurs de la transaction
 */
ostream& operator<<(ostream& f, const Transaction& t);

/*! \class TransactionManager
 * \brief Classe permettant de gérer les Transactions
 * Cette classe permet de gérer les transactions et d'instancer le design pattern singleton
 */



class TransactionManager {
    QString nomSauvegarde = ""; /*!< emplacement du fichier de sauvegarde dans l'ordinateur*/
    friend class FilterCompteIterator;
    list<Transaction> transactions; /*!< Liste de toutes les transactions effectuées sur le logiciel*/
    QString filename = "transactions.txt";
    /*!
     *  \brief Constructeur de TransactionManager
     * Ce constructeur est privé car TransactionManager est un singleton
     */
    TransactionManager() = default;

   public:
    void detruire();
    Date genererDateTest();
    /*!
     *  \brief Sauvegarde des transactions
     * Cette méthode crée les éléments (comme la balise principale) du fichier de sauvegarde puis parcourt l'ensemble
     * des transactions associée au TransactionManager, et pour chacune d'entre elles appelle
     * saveTransactions(Transaction t,QDomDocument docTransac,QDomElement rootTransac) pour créer lélément correspondant
     * à la transaction. Elle va ensuite écrire les éléments sur le fichier xml de sauvegarde
     */
    void save();
    /*!
     *  \brief choix du fichier de sauvegarde
     * Cette méthode permet à l'utilisateur de naviger parmis les fichiers de son ordinateur puis de créer ou choisir un
     * fichier pour la sauvegarde, dont le chemin est stocké dans nomSauvegarde
     */
    void saveUnder();
    /*!
     *  \brief Sauvegarde des triplets
     *  Cette méthode permet de sauvegarder des triplets en xml. Elle est appelée par saveTransaction() pour caque
     * triplet associé à une transaction \param trip : le triplet à sauvegarder \param docTransac : Le QDomDocument de
     * la transaction à laquelle le triplet est associé sur lequel on va creer l'élément triplet \param rootTransac : Le
     * QdomElement qui englobe l'ensemble du fichier de sauvegarde soit tout ce qui est délimité par la balise
     * "Transactions"
     */
    void saveTriplets(Triplet trip, QDomDocument docTransac, QDomElement rootTransac);
    /*!
     *  \brief Sauvegarde des Transactions
     *  Cette méthode permet de sauvegarder des transactions en xml. Elle stocke les informations de la transaction puis
     * fait appel à saveTriplets pour chaque triplet \param t : la transaction a sauvegarder \param docTransat : Le
     * QDomDocument qui englobe l'ensemble du fichier de sauvegarde \param rootTransac : Le QdomElement qui englobe
     * l'ensemble du fichier de sauvegarde soit tout ce qui est délimité par la balise "Transactions"
     */
    void saveTransactions(Transaction t, QDomDocument docTransac, QDomElement rootTransac);
    /*!
     *  \brief Chargement des triplets associés à une transaction
     *  Cette méthode appélée par load() permet d'aller récupérer tous les triplets associés à une transaction dans le
     * fichier de sauvegarde. La méthode identifie le type de compte associé à chaque triplet pour la création du
     * triplet
     * \param root : Le QDomElement correspondant à la transaction en question \param tag : la tag
     * d'dentification dans le fichier de sauvegarde d'un triplet. En utilisation ce sera donc ici toujours "Triplets"
     * car c'est comme ça que le document est formaté dans les fonctions liées à la sauvegarde \param att : ID de la
     * transaction, utilisé pour l'affichage lors de la récupération d'un triplet dans nos tests
     */
    list<Triplet*> getElements(QDomElement root, QString tag, QString att);
    /*!
     *  \brief Chargement et création de l'ensemble des transactions contenues dans un fichier de sauvegarde
     *  Cette méthode permet de créer l'ensemble des transactions contenues dans un fichier de sauvegarde xml. Elle fait
     * appel à getElements(QDomElement root, QString tag, QString att) pour la création des listes de triplets associé à
     * chaque transaction
     */
    void load();
    float getSoldeLive(Compte_component& c) {
        float SoldeRetour = 0;
        if (c.hasSon()) {
            // parcours des fils
            std::list<Compte_component*> listeFils = c.getFils();
            for (std::list<Compte_component*>::const_iterator it = listeFils.cbegin(); it != listeFils.end(); it++) {
                // récursivité;
                Compte_component* pt = dynamic_cast<Compte_component*>(*it);
                SoldeRetour = SoldeRetour + (this->getSoldeLive(*pt));
            }
        }

        for (TransactionManager::FilterCompteIterator it = this->getFilterCompteIterator(c); !it.isDone(); it.next()) {
            if (c.getDate_Rapprochement() <= it.currentItem().getDate()) {
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
    float getSoldeLive(Compte_component& c, Date& d1);
    float getSoldeLive(Compte_component& c, Date& d1, Date& d2);

    /*!
     *  \brief Affectation d'un solde initial (!= 0 car 0 est la valeur par défaut) à un compte de type actifs ou
     * passifs La méthode commence par vérifier qu'au moins un compte de passif existe. Si c'est le cas l'utilisateur
     * est invité à en sélectionner un. Sinon, on lui propose d'en créer un nouveau. Ceci fait, on établit une les
     * triplets et on enregistre la transaction.
     *  \param nouvCompte : Le compte auquel on veut affecter un solde initial
     *  \param solde : Le montant de ce solde initial
     */
    void setSoldeInitial(Compte_component& nouvCompte, Compte_component& comptePropre, float solde = 0);
    std::list<std::tuple<QString, float, QString>> Bilan(Compte_component& c, Date& date, std::list<std::tuple<QString, float, QString>> listeInit);
    std::list<list<std::tuple<QString, float, QString>>> Bilan_general(Date& date);

    std::list<std::tuple<QString, float, QString>>releve(Compte_component& c, Date& date_debut, Date& date_fin,list<std::tuple<QString, float, QString>> liste);
    std::list<list<std::tuple<QString, float, QString>>> resultat(Date& date_debut, Date& date_fin);

    bool cloture();

    void Rapprochement(Compte_component& c );

    /*!
     *  \brief Accesseur de TransactionManager
     * Cet accesseur est déclaré comme static pour implémenter le design pattern singleton
     * \return une référence vers le manager des transactions
     */
    static TransactionManager& getTransactionManager() {
        static TransactionManager tm;
        return tm;
    }
    /*!
     *  \brief Accesseur des transactions gérées par le TransactionManager
     * \return la liste de transactions associée au manager
     */
    list<Transaction> getTransactions() { return transactions; }
    /*!
     *  \brief Méthode pour la création des transactions
     *  Les transactions sont toutes crées par le TransactionManager afin de garantir l'unicité de la racine : les
     * transactions sont nécessairement crées par un TransactionManager, qui est en réalité unique car c'est un
     * singleton.
     */
    void ajouterTransaction(Date& date, QString& id, QString& titre, list<Triplet*> triplets);
    void ajouterPointeur(Transaction* transaction);
    /*!
     *  \brief Méthode pour l'affichage des transactions en ligne de commande
     * Méthode utilisée pour nos tests du back-end
     * \param f : un flux de sortie, en utilisation on choisira cout
     */
    void afficherTransactions(ostream& f);
    void figerTransactions();

    class Iterator {
        friend class TransactionManager;
        TransactionManager& instance = TransactionManager::getTransactionManager();
        list<Transaction>::iterator it = instance.transactions.begin();

       public:
        Iterator() = default;
        bool isDone() const { return it == instance.transactions.end(); }
        void next() {
            if (isDone()) {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            it++;
        }
        Transaction& currentItem() {
            if (isDone()) {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            if (!it->modifiable) {
                throw TresorerieException("Erreur, transaction figée");
            } else {
                return *it;
            }
        }

        const Transaction& currentItemConst() const {
            if (isDone()) {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            return *it;
        }
    };
    Iterator getIterator() { return Iterator(); }

    class FilterCompteIterator {
        friend class TransactionManager;
        TransactionManager& instance = getTransactionManager();
        Compte_component& compte;
        list<Transaction>::iterator it = instance.transactions.begin();
        void goToNextTransaction() {
            while (it != instance.transactions.end()) {
                list<Triplet*>::iterator itTriplet = it->triplets.begin();
                while (itTriplet != it->triplets.end()) {
                    Triplet* tr;
                    tr = dynamic_cast<Triplet*>(*itTriplet);
                    if (tr->getCompte() == compte.Compte_component::getNom() && tr) {
                        return;
                    }
                    itTriplet++;
                }
                it++;
            }
        }
        FilterCompteIterator(Compte_component& c) : compte(c) { goToNextTransaction(); }

       public:
        bool isDone() const { return it == instance.transactions.end(); }
        void next() {
            if (isDone()) {
                throw TresorerieException("parcours terminé");
            }
            it++;
            goToNextTransaction();
        }
        Transaction& currentItem() {
            if (isDone()) {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            if (!it->modifiable) {
                throw TresorerieException("Erreur, transaction figée");
            } else {
                return *it;
            }
        }

        const Transaction& currentItemConst() const {
            if (isDone()) {
                throw TresorerieException("Erreur, iterateur en fin de sequence");
            }
            return *it;
        }
    };
    FilterCompteIterator getFilterCompteIterator(Compte_component& c) { return FilterCompteIterator(c); }

    class FilterDateIterator {
        friend class TransactionManager;
        TransactionManager& instance = TransactionManager::getTransactionManager();
        const Date& date;
        list<Transaction>::iterator it = instance.transactions.begin();
        void goToNextTransaction() {
            while (it != instance.transactions.end()) {
                if (it->date <= date) {
                    return;
                }
                it++;
            }
        }
        FilterDateIterator(const Date& _date) : date(_date) { goToNextTransaction(); }

       public:
        bool isDone() const { return it == instance.transactions.end(); }
        void next() {
            if (isDone()) {
                throw TresorerieException("parcours terminé");
            }
            it++;
            goToNextTransaction();
        }
        Transaction& currentItem() const {
            if (isDone()) {
                throw TresorerieException("parcours terminé");
            }
            if (!it->modifiable) {
                throw TresorerieException("Erreur, transaction figée");
            } else {
                return *it;
            }
        }
        const Transaction& currentItemConst() const {
            if (isDone()) {
                throw TresorerieException("parcours terminé");
            }
            return *it;
        }
    };

    FilterDateIterator getFilterDateIterator(Date& d) { return FilterDateIterator(d); }
};

#endif  // TRANSACTION_H
