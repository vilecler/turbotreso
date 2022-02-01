#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //CompteRacine& rac=CompteRacine::getRacine();

    ui->setupUi(this);

    //=====================arbre a gauche
    ui->TreeCompteMan;
                QStandardItemModel *standardModel = new QStandardItemModel ;//racine de l'arbre
                QStandardItem *rootNode = standardModel->invisibleRootItem();

                //defining a couple of items
                QStandardItem *americaItem = new QStandardItem("America");
                QStandardItem *mexicoItem =  new QStandardItem("Canada");
                QStandardItem *usaItem =     new QStandardItem("USA");
                QStandardItem *bostonItem =  new QStandardItem("Boston");
                QStandardItem *europeItem =  new QStandardItem("Europe");
                QStandardItem *italyItem =   new QStandardItem("Italy");
                QStandardItem *romeItem =    new QStandardItem("Rome");
                QStandardItem *veronaItem =  new QStandardItem("Verona");



                qDebug() << QString("Affichage du Debug dans le constructeur ");



                //building up the hierarchy
                rootNode->    appendRow(americaItem);
                rootNode->    appendRow(europeItem);
                americaItem-> appendRow(mexicoItem);
                americaItem-> appendRow(usaItem);
                usaItem->     appendRow(bostonItem);
                europeItem->  appendRow(italyItem);
                italyItem->   appendRow(romeItem);
                italyItem->   appendRow(veronaItem);

                //register the model
                ui->TreeCompteMan->setModel(standardModel);
                ui->TreeCompteMan->expandAll();

                //selection changes shall trigger a slot
                QItemSelectionModel *selectionModel= ui->TreeCompteMan->selectionModel();
                //connect(selectionModel, SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
                        //this, SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));






    connect(ui->actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::charger()
{
   CompteRacine& rac=CompteRacine::getRacine();
   QString filename = QFileDialog::getOpenFileName();
   rac.load(filename);
   refreshTreeCompte();


}

void MainWindow::sauvegarder(){
    CompteRacine& rac=CompteRacine::getRacine();
    for(auto it=CompteRacine::getRacine().getConstIterator(); !it.isDone(); it.next())
    {
    QString type=it.currentItem().getType();
    qDebug() << QString("Sauvegarde du compte %1").arg(type);
    }
    rac.save();
}

void MainWindow::refreshTreeCompte(){

    CompteRacine& rac=CompteRacine::getRacine();

    QStandardItemModel *standardModel = new QStandardItemModel ;//racine de l'arbre
    QStandardItem *rootNode = standardModel->invisibleRootItem();
    //affichage des comptesman
    QStandardItem * tabComptMan[rac.getNb()];//tableau de pointeurs vers les items des comptes man
    for(auto it=CompteRacine::getRacine().getConstIterator(); !it.isDone(); it.next())
    {
    QString type=it.currentItem().getType();
    qDebug() << QString("Affichage du compteMan dans mainwindow la %1").arg(type);
     tabComptMan[it.getI()] =  new QStandardItem(type);
     if(it.getI()==0) rootNode->    appendRow(tabComptMan[it.getI()]);
     else{
         tabComptMan[it.getI()-1]->    appendRow(tabComptMan[it.getI()]);
     }

    }

    //definition du titre de la colonne 1
    QStandardItem *titreHeader =  new QStandardItem("Comptes");
    standardModel->setHorizontalHeaderItem ( 0, titreHeader);

    //refresh
    ui->TreeCompteMan->setModel(standardModel);
    ui->TreeCompteMan->expandAll();

}


MainWindow::~MainWindow()
{
    delete ui;
}
