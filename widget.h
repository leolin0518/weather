#ifndef WIDGET_H
#define WIDGET_H


#include <QtCharts>
QT_CHARTS_USE_NAMESPACE

#include <QWidget>
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

struct WeatherInfo{
    QString currCity;
    QString date;
    QString week;
    QString type;
    QString curTemp;
    QString hightemp;
    QString lowtemp;
    QString fengli;
    QString fengxiang;
    QString aqi;
};


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void init();
    void ui_init();

    void setNetworkRequest(QNetworkRequest &request, QString cityName);//设置网络请求
    void setNetworkRequestCityInfo(QNetworkRequest &request, QString Name);//设置网络请求获取省市区信息

    void getHistoryWeatherInfo(QJsonObject data);                   //获取历史天气信息
    void getTodayWeatherInfo(QJsonObject data);                     //获取当日天气信息
    void getForecastWeatherInfo(QJsonObject data);                  //获取未来天气信息
    void getOtherInfo(QJsonObject data);                            //获取其他天气信息
    void setAqi(QString &strAqi);                                   //设置空气质量指数显示
    void refreshWeather(QString str);
    void refreshCityInfo(QString str);

    void getProvinceList();    //显示省份信息
    void getCityList(QJsonObject data);
    void getAreaList(QJsonObject data);


    void setUI_information();   //设置界面显示信息，如当前温度，空气指数等
    void splineChart(QStringList valueList);//简单的线条曲线图 http://doc.qt.io/qt-5/qtcharts-splinechart-example.html


private slots:
    void getReplyFinished(QNetworkReply *reply);

    void getReplyFinishedCityInfo(QNetworkReply *reply);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_showSetting_pushButton_clicked();//显示cityList

    void on_save_pushButton_clicked();//保存

    void on_city_comboBox_p_activated(const QString &arg1);

    void on_city_comboBox_c_activated(const QString &arg1);

private:
    Ui::Widget *ui;
    QNetworkAccessManager *manage;
    QNetworkAccessManager *manage_cityInfo;

public:
    WeatherInfo todayInfo;
    WeatherInfo forecastInfo;
    //WeatherInfo todayInfo;
    QStringList provinceList;               //省份列表
    QStringList cityList;                   //城市列表
    QStringList areaList;                   //区域列表
    QStringList forecastInfo_wenduMax;                   //城市列表
};

#endif // WIDGET_H
