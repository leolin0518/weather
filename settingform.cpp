#include "settingform.h"
#include "ui_settingform.h"
#include <QFile>
#include <QMessageBox>

QNetworkRequest request_cityinfo; //获取天气信息的request


extern QString global_city; //声明外部变量
extern QString global_city_id; //声明外部变量
extern QString global_data_file_path; //声明外部变量


SettingForm::SettingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingForm)
{
    ui->setupUi(this);
    ui->lineEdit_City->show();
    ui->lineEdit_City->setPlaceholderText(tr("请输入要查询的城市？"));

}

SettingForm::~SettingForm()
{
    delete ui;
}

void SettingForm::create_data_file(QString city, QString cityID)
{
    QString AppDirPath  = QApplication::applicationDirPath();
    qDebug() << "AppDirPath:" << AppDirPath;
    QString fileName = AppDirPath + "/" + "data.txt";
    qDebug() << "fileName:" << fileName;
    global_data_file_path = fileName;
    QFile f(fileName);

    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()  << "Open failed.";
        QMessageBox::warning(this,"file error","can't open",QMessageBox::Yes);
    }
    qDebug()  << __LINE__ <<  "AAAAAAAAA" << city << cityID;

    QTextStream in(&f);
    //in << str;//没有回车
    in << city << endl;//有回车
    in << cityID << endl;//有回车

//    for(int i=0; i< str.size(); i++)
//    {
//        qDebug()  << __LINE__ <<  str.at(i);
//        in << str.at(i) << endl;//有回车
//    }

    f.close();

}

//搜索输入的城市的信息
void SettingForm::on_chaxun_pushButton_clicked()
{
    QNetworkRequest request;
    QNetworkAccessManager *manage_city_info;
    manage_city_info = new QNetworkAccessManager(this);
    connect(manage_city_info, SIGNAL(finished(QNetworkReply *)), this, SLOT(getReplyCityInfo(QNetworkReply*)));

    QString cityName = ui->lineEdit_City->text().trimmed();

    QString requst_url_str = QString("https://geoapi.qweather.com/v2/city/lookup?location=%1&key=930cc953111c43c6924f88ebda8b00df").arg(cityName);
    //qDebug() << __LINE__ <<  "requst_url_str------------------:"  << requst_url_str;

    request.setUrl(QUrl(QString(requst_url_str)));

    //get
    manage_city_info->get(request);
}

void SettingForm::getReplyCityInfo(QNetworkReply *reply)
{
    QJsonObject json_citydata = QJsonDocument::fromJson(reply->readAll()).object();
    qDebug() << __LINE__ << "Json 城市信息:" << json_citydata;

    cityList.clear();
    cityList_id.clear();


    QJsonArray cityinfo = json_citydata.value("location").toArray();
    int size = cityinfo.size();
    qDebug() << "cityinfo.......................size;" << size;
    for(int i=0; i < size; i++)
    {
        QJsonObject tmp = cityinfo.at(i).toObject();
        QString district_cn = tmp.value("name").toString();
       // qDebug() << __LINE__ << "district_cn" << district_cn;
        cityList << district_cn;

        QString city_id_tmp = tmp.value("id").toString();
       // qDebug() << __LINE__ << "city_id_tmp" << city_id_tmp;
        cityList_id << city_id_tmp;


    }

//    qDebug() << __LINE__ << "cityList" << cityList;
//    qDebug() << __LINE__ << "cityList_id" << cityList_id;

    if(cityList.size() > 0)
    {
        QString city = cityList.first();

        if(city.isEmpty())
        {
          // ui->currCity_label->clear();
        }
        else
        {
           //ui->currCity_label->setText(city);

        }
    }

    if(cityList_id.size() > 0)
    {
        QString city_id = cityList_id.first();

        if(city_id.isEmpty())
        {
          // ui->currCityID_label->clear();
        }
        else
        {
         //  ui->currCityID_label->setText(city_id);

        }
    }

    ui->comboBox_01->clear();
    //ui->comboBox_01->addItems(cityList);

    for (int i=0; i< cityList.size(); i++)
    {
        QString city_tmp = cityList.at(i);
        QString city_id_tmp = cityList_id.at(i);
        ui->comboBox_01->addItem(city_tmp, city_id_tmp);

    }
}

//点击下拉框槽函数
void SettingForm::on_comboBox_01_activated(const QString &arg1)
{
//    qDebug() << __LINE__ << "arg1" << arg1;
//    qDebug() << __LINE__ << "arg1" << ui->comboBox_01->currentText();
//    qDebug() << __LINE__ << "arg1" << ui->comboBox_01->currentData().toString();
    global_city = ui->comboBox_01->currentText();
    global_city_id = ui->comboBox_01->currentData().toString();
//    qDebug() << __FILE__ << __LINE__ << "global_city" << global_city;
//    qDebug() << __FILE__ << __LINE__ << "global_city_id" << global_city_id;

}

void SettingForm::on_lineEdit_City_textChanged(const QString &arg1)
{

   // qDebug() << __FILE__ << __LINE__ << "arg1" << arg1;

    on_chaxun_pushButton_clicked();
}

void SettingForm::on_save_pushButton_clicked()
{
    global_city =  ui->comboBox_01->currentText();
    global_city_id = ui->comboBox_01->currentData().toString();

    QStringList sentData;
    sentData << global_city << global_city_id;
    create_data_file(global_city, global_city_id);
    emit sendDataFromSettingToWidget(sentData);  //获取lineEdit的输入并且传递出去
    this->close();

}
