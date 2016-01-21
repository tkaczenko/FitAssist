#include "FitnessCalc.h"
#include "ui_FitnessCalc.h"

FitnessCalc::FitnessCalc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FitnessCalc)
{
    ui->setupUi(this);

    if (!readProfile())
        ui->labelImage->setPixmap(QPixmap(":man&woman/images/man.png"));

    readMyDiet();

    // Drawing the color rectangles on the labels
    QPixmap *pixmap = new QPixmap(30,15);
    pixmap->fill(QColor("#5DA5DA"));
    ui->lblDrawProtein->setPixmap(*pixmap);

    pixmap->fill(QColor("#FAA43A"));
    ui->lblDrawFat->setPixmap(*pixmap);

    pixmap->fill(QColor("#B276B2"));
    ui->lblDrawCarbohydrate->setPixmap(*pixmap);
    delete pixmap;

    // Click on the listWidgetMenu
    connect(ui->listWidgetMenu, &QListWidget::currentRowChanged,  ui->stackedWidget,  &QStackedWidget::setCurrentIndex);

    // Click on the commandLinkButtonAdvices
    connect(ui->commandLinkButtonAdvices, &QCommandLinkButton::clicked, this, &FitnessCalc::showAdvices);

    // Click on the commandLinkButtonAbout
    connect(ui->commandLinkButtonAbout, &QCommandLinkButton::clicked, this, &FitnessCalc::clickedOnAboutDialog);

    // Click on the pushButtonApply
    connect(ui->pushButtonApply, &QPushButton::clicked, this, &FitnessCalc::setProfile);

    // Click on the radioButtonMale && radioButtonFemale
    connect(ui->radioButtonMale, &QRadioButton::toggled, this, &FitnessCalc::maleReactToToggle);

    // Click on the pushButtonAdd
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &FitnessCalc::clickedOnAddDialog);

    // Double click on the item of tableView
    connect(ui->tableView, &QTableView::doubleClicked, this, &FitnessCalc::doubleClickedEdit);

    // Click on the pushButtonEdit
    connect(ui->pushButtonEdit, &QPushButton::clicked, this, &FitnessCalc::clickedOnEditList);

    // Click on the pushButtonDel
    connect(ui->pushButtonDel, &QPushButton::clicked, this, &FitnessCalc::deleteRowFromMyDiet);

    // Сlick on the pushButtonClean
    connect(ui->pushButtonClean, &QPushButton::clicked, this, &FitnessCalc::removeAllRows);
}

FitnessCalc::~FitnessCalc()
{
    delete ui;

    delete dbase;
    delete model;
    delete profile;
}

void FitnessCalc::setProfile()
{
    if (!ui->radioButtonMale->isChecked() && !ui->radioButtonFemale->isChecked()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Пожалуйста, выберите Ваш пол."));
    } else {
        float weight = ui->doubleSpinBoxWeight->value();
        short height = ui->spinBoxHeight->value();
        short age = ui->spinBoxAge->value();
        float activity;
        switch (ui->comboBoxActivity->currentIndex()) {
        case 0:
            activity = 1.1;
            break;
        case 1:
            activity = 1.2;
            break;
        case 2:
            activity = 1.3;
            break;
        case 3:
            activity = 1.4;
            break;
        case 4:
            activity = 1.5;
            break;
        }
        QString gender;
        if (ui->radioButtonMale->isChecked())
            gender = "male";
        else
            gender = "female";
        int targetCalorie = ui->spinBoxTarget->value();

        profile->setWeigth(weight);
        profile->setHeigth(height);
        profile->setAge(age);
        profile->setGender(gender);
        profile->setActivity(activity);
        profile->setCalorie(targetCalorie);
        profile->calculateNorms();

        writeProfile();
        showNorms();
    }
}

void FitnessCalc::getPFC(double protein, double fat, double carbonhydrate)
{
    if (protein == protein && fat == fat && carbonhydrate == carbonhydrate) {
        ui->lblProtein->setText(QString::number(protein, 'g', 3) + " %");
        ui->lblFat->setText(QString::number(fat, 'g', 3) + " %");
        ui->lblCarbohydrate->setText(QString::number(carbonhydrate, 'g', 3) + " %");
    }
}

void FitnessCalc::getUpdateFlag(bool update)
{
    if (update) {
        model->select();
        showTotal();
    }
}

void FitnessCalc::doubleClickedEdit(const QModelIndex &index)
{
    EditDialog *edit = new EditDialog();
    edit->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(this, &FitnessCalc::sendModel, edit, &EditDialog::getModel);
    connect(edit, &EditDialog::sendUpdateFlag, this, &FitnessCalc::getUpdateFlag);

    emit sendModel(model);

    edit->showEdit(index);
}

void FitnessCalc::deleteRowFromMyDiet()
{
    model->removeRow(ui->tableView->currentIndex().row());
    getUpdateFlag(true);
}

void FitnessCalc::removeAllRows()
{
    for (int i = 0; i < model->rowCount(); i++)
        model->removeRow(i);
    getUpdateFlag(true);
}

void FitnessCalc::clickedOnEditList()
{
    EditListDialog *editList = new EditListDialog();
    editList->setAttribute(Qt::WA_DeleteOnClose, true);
    editList->show();
}

void FitnessCalc::clickedOnAboutDialog()
{
    aboutDialog *about = new aboutDialog();
    about->setAttribute(Qt::WA_DeleteOnClose, true);
    about->show();
}

void FitnessCalc::clickedOnAddDialog()
{
    addDialog *add = new addDialog();
    add->setAttribute(Qt::WA_DeleteOnClose, true);

    // Update the model when the signal come
    connect(add, &addDialog::sendUpdateFlag, this, &FitnessCalc::getUpdateFlag);

    add->show();
}

void FitnessCalc::showAdvices()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void FitnessCalc::maleReactToToggle(bool checked)
{
    if (checked)
        ui->labelImage->setPixmap(QPixmap(":man&woman/images/man.png"));
    else
        ui->labelImage->setPixmap(QPixmap(":man&woman/images/woman.png"));
}

bool FitnessCalc::readProfile()
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    } else {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_5);

        in >> *profile;

        file.close();

        if (profile->getGender() == "male") {
            ui->radioButtonMale->setChecked(true);
            ui->labelImage->setPixmap(QPixmap(":man&woman/images/man.png"));
        }
        else {
            ui->radioButtonFemale->setChecked(true);
            ui->labelImage->setPixmap(QPixmap(":man&woman/images/woman.png"));
        }
        ui->doubleSpinBoxWeight->setValue(profile->getWeigth());
        ui->spinBoxHeight->setValue(profile->getHeigth());
        ui->spinBoxAge->setValue(profile->getAge());
        float activity = profile->getActivity() * 10;
        ui->spinBoxTarget->setValue(profile->getCalorie());

        if (activity == 11)
            ui->comboBoxActivity->setCurrentIndex(0);
        else if (activity == 12)
            ui->comboBoxActivity->setCurrentIndex(1);
        else if (activity == 13)
            ui->comboBoxActivity->setCurrentIndex(2);
        else if (activity == 14)
            ui->comboBoxActivity->setCurrentIndex(3);
        else
            ui->comboBoxActivity->setCurrentIndex(4);

        showNorms();
        return true;
    }
}

void FitnessCalc::writeProfile()
{
    if (!QDir("profile").exists())
        QDir().mkdir("profile");

    QFile file(fileName);
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_5);

    if(file.open(QIODevice::WriteOnly))
        out << *profile;

    file.close();
}

void FitnessCalc::showNorms()
{
    ui->labelTarget->setText(QString::number(profile->getCalorie()));
    ui->labelTargetCcal->setText(" Ккал");
    ui->labelBMI->setText(QString::number(profile->getBMI(),'g',3));
    ui->labelConformity->setText(profile->getConclusion());
    QString res = QString::number(profile->getOptimalWeightMin(),'g',3) + " - " + QString::number(profile->getOptimalWeightMax(),'g',4);
    ui->labelOptimalWeight->setText(res);
    ui->labelCalorieNorm->setText(QString::number(profile->getBMR(),'g',4));
    ui->labelWaterNorm->setText(QString::number(profile->getWater(),'g',3));
}

void FitnessCalc::readMyDiet()
{
    dbase->setDatabaseName(QDir::currentPath() + "/base/MyDiet.sqlite");
    dbase->open();

    QSqlQuery query;
    QString str = "CREATE TABLE Рацион ("
                  "Наименование text, "
                  "Вес double, "
                  "Белки double, "
                  "Жиры double, "
                  "Углеводы double, "
                  "Ккал integer"
                  ");";
    query.exec(str);

    model->setTable("Рацион");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Update the total PFC and redraw the pie chart
    connect(this, &FitnessCalc::sendPFC, ui->widget, &PieChart::getPFC);
    connect(ui->widget, &PieChart::sendPFC, this, &FitnessCalc::getPFC);

    showTotal();
}

void FitnessCalc::showTotal()
{
    int weight = 0;
    int calorie = 0;
    double protein = 0;
    double fat = 0;
    double carbonhydrate = 0;

    for (int i = 0; i < model->rowCount(); i++) {
        weight += model->record(i).value("Вес").toInt();
        protein += model->record(i).value("Белки").toDouble();
        fat += model->record(i).value("Жиры").toDouble();
        carbonhydrate += model->record(i).value("Углеводы").toDouble();
        calorie += model->record(i).value("Ккал").toInt();
    }

    ui->lblWeight->setText(QString::number(weight));
    ui->lblProtein_Conclusion->setText(QString::number(protein));
    ui->lblFat_Conclusion->setText(QString::number(fat));
    ui->lblCarbohydrate_Conlusion->setText(QString::number(carbonhydrate));
    ui->lblCalorie_Conclusion->setText(QString::number(calorie));
    ui->labelRecieved->setText(QString::number(calorie));
    ui->labelRecievedCcal->setText(" Ккал");

    int targetCalorie = ui->labelTarget->text().toInt();
    if (calorie > 0 && targetCalorie != 0) {
        ui->labelLefted->setText(QString::number(targetCalorie - calorie));
        ui->labelLeftedCcal->setText(" Ккал");
    } else {
        ui->labelLefted->setText("0");
        ui->labelLeftedCcal->setText(" Ккал");
    }

    emit sendPFC(protein, fat, carbonhydrate);
}