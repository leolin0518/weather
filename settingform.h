#ifndef SETTINGFORM_H
#define SETTINGFORM_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>



namespace Ui {
class SettingForm;
}

class SettingForm : public QWidget
{
    Q_OBJECT

public:
    explicit SettingForm(QWidget *parent = nullptr);
    ~SettingForm();


    QStringList cityList;
    QStringList cityList_id;

private slots:
    void on_chaxun_pushButton_clicked();

    void getReplyCityInfo(QNetworkReply *reply);


    void on_comboBox_01_activated(const QString &arg1);

    void on_lineEdit_City_textChanged(const QString &arg1);

    void on_save_pushButton_clicked();

signals:
    void sendDataFromSettingToWidget(QStringList);   //用来传递数据的信号

private:
    Ui::SettingForm *ui;
};

#endif // SETTINGFORM_H
